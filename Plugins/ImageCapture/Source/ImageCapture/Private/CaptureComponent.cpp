#include "Capture/CaptureComponent.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Engine/SceneCaptureCube.h"
#include "Components/SceneCaptureComponentCube.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "CubemapUnwrapUtils.h"
#include "Log.h"

UCaptureComponent::UCaptureComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    CaptureSize = 4096;
    OutputFormat = EOutputFormat::NONE;
    
    CubeCapture = CreateDefaultSubobject<USceneCaptureComponentCube>(TEXT("CubeCapture"));
    CubeCapture->SetupAttachment(this);

    SetMobility(EComponentMobility::Movable);
    bHiddenInGame = true;

}

void UCaptureComponent::BeginPlay()
{
    UE_LOG(LogImageCapturePlugin, Log, TEXT("Panorama capture component begin play"));
    Super::BeginPlay();

}

bool UCaptureComponent::Initialise(EOutputFormat Format, FString Directory)
{
    FScopeLock ScopeLock(&CaptureMutex);
    if (bIsInitialised)
    {
        UE_LOG(LogImageCapturePlugin,
            Error,
            TEXT("Internal error: Panorama capture component has already been initialized"));
        return false;
    }
    if (Format == EOutputFormat::NONE)
    {
        UE_LOG(LogImageCapturePlugin,
            Error, 
            TEXT("Internal error: no format provided for capturing"));
        return false;
    }
    if (Directory.IsEmpty())
    {
        UE_LOG(LogImageCapturePlugin,
            Error,
            TEXT("Internal error: no directory provided for capturing"));
        return false;
    }
    if (!OutputFormatExtension.Contains(Format))
    {
        UE_LOG(LogImageCapturePlugin,
            Error,
            TEXT("Invalid format: %d provided, no extension found for file format"),
            Format);
        return false;
    }

    OutputFormat = Format;
    OutputDirectory = Directory;
    
    CubeCapture->bCaptureEveryFrame = false;
    CubeCapture->bCaptureRotation = true;
    CubeCapture->bCaptureOnMovement = false;
    CubeCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    if (OutputFormat == EOutputFormat::HDR)
    {
        CubeCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
    }
    CubeCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
    CubeCapture->bAutoActivate = true;
    CubeCapture->bAlwaysPersistRenderingState = false;
    CubeCapture->ShowFlags.SetPostProcessing(true);
    CubeCapture->ShowFlags.SetTemporalAA(true);
    check(CubeCapture->TextureTarget == nullptr);

    RenderTargetCube = NewObject<UTextureRenderTargetCube>(this, TEXT("PanoramicRenderTargetCube"), RF_Transient);
    RenderTargetCube->SizeX = CaptureSize;
    RenderTargetCube->bHDR = OutputFormat == EOutputFormat::HDR;
    RenderTargetCube->ClearColor = FLinearColor::Black;
    RenderTargetCube->bForceLinearGamma = false;
    RenderTargetCube->InitAutoFormat(CaptureSize);
    RenderTargetCube->bCanCreateUAV = false;
    RenderTargetCube->TargetGamma = 1.6;
    RenderTargetCube->bNeedsTwoCopies = true;

    UE_LOG(LogImageCapturePlugin, Log, TEXT("Display gamma: %f"), GEngine->GetDisplayGamma());

    RenderTargetCube->CompressionNoAlpha = false;
    RenderTargetCube->CompressionNone = true;
    if (0)
    {
        // TODO
        RenderTargetCube->CompressionNone = false;
        RenderTargetCube->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        if (OutputFormat == EOutputFormat::HDR)
        {
            RenderTargetCube->CompressionSettings = TextureCompressionSettings::TC_HDR;
        }
    }
    RenderTargetCube->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    RenderTargetCube->UpdateResourceImmediate(true);
    CubeCapture->TextureTarget = RenderTargetCube;

    if (OutputFormat == EOutputFormat::HDR)
    {
        bIsInitialised = true;
        return bIsInitialised;
    }

    static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    switch (OutputFormat)
    {
        case EOutputFormat::JPEG:
            ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
            break;
        case EOutputFormat::PNG:
            ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
            break;
        default:
            UE_LOG(LogImageCapturePlugin, Error, TEXT("Unknown output format provided for capture"));
            return false;
    }
    check(ImageWrapper && ImageWrapper.IsValid());
    bIsInitialised = true;

    return bIsInitialised;
}

// TODO: write image on Task Thread
bool UCaptureComponent::Capture(FString CaptureName)
{
    FScopeLock ScopeLock(&CaptureMutex);
    if (!bIsInitialised)
    {
        UE_LOG(LogImageCapturePlugin,
            Error, 
            TEXT("Internal error: capture component has not been initialized"));
        return false;    
    }
    check(CubeCapture->TextureTarget);

    const FTransform Transform = GetComponentTransform();
    FVector Location = Transform.GetLocation();
    FRotator Rotator = Transform.GetRotation().Rotator();

    UE_LOG(LogImageCapturePlugin,
        Log,
        TEXT("Capturing at world position: [%g, %g, %g] with yaw: %g to: %s for %s"),
        Location.X, Location.Y, Location.Z,
        Rotator.Yaw,
        *OutputDirectory,
        *CaptureName);
    CubeCapture->CaptureScene();
    FlushRenderingCommands();

    const FString FileName = FString::Printf(TEXT("%s.%s"), *CaptureName, *OutputFormatExtension[OutputFormat]);
    const FString CaptureFileName = FPaths::Combine(OutputDirectory, FileName);

    if (OutputFormat == EOutputFormat::HDR)
    {
        TUniquePtr<FArchive> FileWriter(IFileManager::Get().CreateFileWriter(*CaptureFileName));
        if (FileWriter == nullptr)
        {
            UE_LOG(LogImageCapturePlugin, Error, TEXT("Internal error, failed to create file writer for HDR capture"));
            return false;
        }
        FImageUtils::ExportRenderTargetCubeAsHDR(RenderTargetCube, *FileWriter);
        return true;
    }
    check(ImageWrapper.IsValid());
    const int32 Width = CubeCapture->TextureTarget->SizeX*2;
    const int32 Height = CubeCapture->TextureTarget->SizeX;

    // raw image data
    TArray64<uint8> RawData;
    FIntPoint Size;
    EPixelFormat PixelFormat;
    if (!CubemapHelpers::GenerateLongLatUnwrap(RenderTargetCube, RawData, Size, PixelFormat))
    {
        UE_LOG(LogImageCapturePlugin, Error, TEXT("Failed to capture: %s, unable to unwrap cube"), *CaptureFileName); 
        return false;
    }
    UE_LOG(LogImageCapturePlugin, Log, TEXT("Unwrapped cube scene, size=%d (%d, %d) vs (%d, %d) format=%d"),
        RawData.GetAllocatedSize(),
        Width,
        Height,
        Size.X,
        Size.Y,
        PixelFormat);

    if (Width != Size.X || Height != Size.Y)
    {
        UE_LOG(LogImageCapturePlugin,
            Error, 
            TEXT("Unwrapping cube scene failed, size=(%d, %d) != (%d, %d)"),
            Width,
            Height,
            Size.X,
            Size.Y);
        return false;
    }

    if (OutputFormat == EOutputFormat::PNG)
    {
        for (int32 i = 3; i < RawData.Num(); i += 4)
        {
            RawData[i] = 255;
        }
    }

    UE_LOG(LogImageCapturePlugin, Log, TEXT("Saving capture to file: %s"), *CaptureFileName);
    if (!ImageWrapper->SetRaw(RawData.GetData(), RawData.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8))
    {
        UE_LOG(LogImageCapturePlugin, Error, TEXT("Failed to set image data for file: %s"), *CaptureFileName);
        return false;
    }

    int32 Quality = 100;
    if (!FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(Quality), *CaptureFileName))
    {
        UE_LOG(LogImageCapturePlugin, Error, TEXT("Failed to save image to file: %s"), *CaptureFileName);
        return false;
    }
    return true;
}
