#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"

#include "CaptureComponent.generated.h"

UENUM(BlueprintType)
enum class EOutputFormat : uint8
{
    NONE,
    JPEG,
    PNG,
    HDR
};

static const TMap<EOutputFormat, FString> OutputFormatExtension = {
    { EOutputFormat::JPEG, TEXT("jpg") },
    { EOutputFormat::PNG, TEXT("png") },
    { EOutputFormat::HDR, TEXT("hdr") }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class IMAGECAPTURE_API UCaptureComponent : public USceneComponent
{
    GENERATED_BODY()

public:    
    UCaptureComponent();

protected:
    virtual void BeginPlay() override;

public:    
    UFUNCTION(BlueprintCallable, Category = "Capture")
    bool Initialise(EOutputFormat Format, FString Directory);

    UFUNCTION(BlueprintCallable, Category = "Capture")
    bool Capture(FString CaptureName);

protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Capture")
    int32 CaptureSize;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Capture")
    EOutputFormat OutputFormat;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Capture")
    FString OutputDirectory;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "Capture")
    class UTextureRenderTargetCube* RenderTargetCube;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "Capture")
    class USceneCaptureComponentCube* CubeCapture;

private:
    FCriticalSection CaptureMutex;
    FThreadSafeBool bIsInitialised;
    TSharedPtr<IImageWrapper> ImageWrapper;
    
};
