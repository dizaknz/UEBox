#include "ImageCapturer.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "ShowFlags.h"

#include "RHICommandList.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"

AImageCapturer::AImageCapturer()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AImageCapturer::EndPlay(const EEndPlayReason::Type endPlayReason)
{
    if (!bIsCapturing)
    {
        return;
    }
    FFunctionGraphTask::CreateAndDispatchWhenReady(
        [&]() 
        {
            FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().RemoveAll(this);
        }, 
        TStatId(), 
        NULL, 
        ENamedThreads::GameThread);
}

/**
 * @brief Setup the backbuffer capture delegate.
 */
void AImageCapturer::Setup()
{
    FFunctionGraphTask::CreateAndDispatchWhenReady(
        [&]() 
        {
            if (bIsCapturing)
            {
                return;
            }
            if (FSlateApplication::IsInitialized())
               {
                FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().
                    AddUObject(this, &AImageCapturer::OnBackBufferReady);
            }
        }, 
        TStatId(), 
        NULL, 
        ENamedThreads::GameThread);
}

struct FReadSurfaceContext{
    FTexture2DRHIRef Texture2D;
    TArray<FColor>* OutData;
    FIntRect Rect;
    FReadSurfaceDataFlags Flags;
};

/**
 * @brief OnBackBufferReady copies the backbuffer for Capture 
 * 
 * @param SlateWindow 
 * @param BackBuffer 
 */
void AImageCapturer::OnBackBufferReady(SWindow& SlateWindow, const FTexture2DRHIRef& BackBuffer)
{
    check(IsInRenderingThread());

#if WITH_EDITOR
    if (bHasCapturedMultiple && SlateWindow.GetType() != EWindowType::Normal)
    {
        return;
    }

    if (bHasCapturedMultiple && 
        BackBuffer->GetSizeX() > CachedTexture->GetSizeX() &&
        BackBuffer->GetSizeY() > CachedTexture->GetSizeY())
    {
        UE_LOG(LogTemp,
            Verbose, 
            TEXT("AImageCapturer::OnBackBufferReady - Ignoring backbuffer, size=%d, %d"),
                BackBuffer->GetSizeX(),
                BackBuffer->GetSizeY());
         // ignore the larger backbuffer captured for editor, retain smaller PIE only
        return;
    }

    // workaround for capturing backbuffers for different type of editor runs
    if (!bHasCapturedMultiple &&
        CachedTexture != nullptr &&
        CachedTexture->GetSizeX() != BackBuffer->GetSizeX() &&
        CachedTexture->GetSizeY() != BackBuffer->GetSizeY())
    {
        // different size capture implies a detached PIE, we ignore the larger editor backbuffer
        bHasCapturedMultiple = true;
    }
    if (CachedTexture == nullptr || 
        CachedTexture->GetSizeX() > BackBuffer->GetSizeX() ||
        CachedTexture->GetSizeY() > BackBuffer->GetSizeY())
    {
        FRHIResourceCreateInfo Info; 
        CachedTexture = RHICreateTexture2D(
            BackBuffer->GetSizeX(),
            BackBuffer->GetSizeY(),
            BackBuffer->GetFormat(), 
            1, 
            1,
            TexCreate_SRGB,
            Info);

        bIsCapturing = true;
    }
#else
    if (CachedTexture == nullptr)
    {
        FRHIResourceCreateInfo Info; 
        CachedTexture = RHICreateTexture2D(
            BackBuffer->GetSizeX(),
            BackBuffer->GetSizeY(),
            BackBuffer->GetFormat(), 
            1, 
            1,
            TexCreate_SRGB,
            Info);

        bIsCapturing = true;
    }
 #endif

    static IRendererModule* RendererModule = &FModuleManager::GetModuleChecked<IRendererModule>("Renderer");
    FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
    FScopeLock ScopeLock(&CriticalSection);
    bHasNewFrame = true;
    RHICmdList.CopyToResolveTarget(BackBuffer, CachedTexture, FResolveParams{});
}

/**
 * @brief Capture converts the cached backbuffer texture to an FColor array, 
 * then queues an image writer task to write it to disk (async on next Tick) as 
 * the file name provided.
 * 
 * @param FileName name of the image to save the current locked backbuffer
 */
void AImageCapturer::Capture(FString FileName)
{
    if (!bIsCapturing)
    {
        return;
    }
    if (!bHasNewFrame)
    {
        return;
    }
    FScopeLock ScopeLock(&CriticalSection);
    UE_LOG(LogTemp,
        Verbose, 
        TEXT("AImageCapturer::Capture - Capturing from cached back buffer, size=%d, %d"),
        CachedTexture->GetSizeX(),
        CachedTexture->GetSizeY());

    FWriteRequest* WriteRequest = new FWriteRequest();
    WriteRequest->FileName = FileName;
    WriteRequest->Width = CachedTexture->GetSizeX();
    WriteRequest->Height = CachedTexture->GetSizeY();

    FReadSurfaceContext ReadSurfaceContext = {
        CachedTexture,
        &(WriteRequest->Image),
        FIntRect(
            0,
            0,
            WriteRequest->Width,
            WriteRequest->Height),
        FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
    };

    ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
        [ReadSurfaceContext](FRHICommandListImmediate& RHICmdList)
        {
            RHICmdList.ReadSurfaceData(
                ReadSurfaceContext.Texture2D,
                ReadSurfaceContext.Rect,
                *ReadSurfaceContext.OutData,
                ReadSurfaceContext.Flags);
        }
    );

    WriteRequestQueue.Enqueue(WriteRequest);
    WriteRequest->RenderFence.BeginFence();
    bHasNewFrame = false;
}

/**
 * @brief Tick takes care of processing the next image write request in the writer queue, it 
 * converts the request's FColor array to an image array and schedules an asynchronous 
 * task to write it to disk.
 * 
 * @param DeltaTime 
 */
void AImageCapturer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsCapturing)
    {
        // nothing to do
        return;
    }

    if (WriteRequestQueue.IsEmpty())
    {
        // no images in queue
        return;
    }
    FWriteRequest* WriteRequest;
    WriteRequestQueue.Peek(WriteRequest);
    if (WriteRequest && WriteRequest->RenderFence.IsFenceComplete())
    {
        RunWriterTask(WriteRequest->Image, WriteRequest->Width, WriteRequest->Height, WriteRequest->FileName);
        WriteRequestQueue.Pop();
        delete WriteRequest;
    }
}

/**
 * @brief Starts a background writer task to write an image array to the provided filename.
 * 
 * @param Image the image array to write
 * @param FileName image file name
 */
void AImageCapturer::RunWriterTask(TArray<FColor> Image, int ImageWidth, int ImageHeight, FString FileName)
{
    (new FAutoDeleteAsyncTask<FImageWriterTask>(Image, ImageWidth, ImageHeight, FileName))->StartBackgroundTask();
}

FImageWriterTask::FImageWriterTask(TArray<FColor> Image, int Width, int Height, FString FileName)
    : Image(Image), FileName(FileName), ImageWidth(Width), ImageHeight(Height)
    {}

/**
 * @brief Saves an image array to disk
 */
void FImageWriterTask::DoWork(){
    // ready and rendered
    static IImageWrapperModule& ImageWrapperModule =
        FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    static TSharedPtr<IImageWrapper> ImageWrapper =
        ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
    ImageWrapper->SetRaw(
        Image.GetData(),
        Image.GetAllocatedSize(),
        ImageWidth,
        ImageHeight,
        ERGBFormat::BGRA,
        8);
    const TArray64<uint8>& compressedImage = ImageWrapper->GetCompressed(0);

    FFileHelper::SaveArrayToFile(compressedImage, *FileName);
    UE_LOG(LogTemp, Verbose, TEXT("Written image: %s to disk"), *FileName);
}
