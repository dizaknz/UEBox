#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "ImageCapturer.generated.h"

UCLASS()
class IMAGECAPTURE_API AImageCapturer : public AActor
{
    GENERATED_BODY()
    
public:    
    AImageCapturer();

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, meta = (Category = "Capture", DisplayName = "SetupImageCapturer"))
    void Setup();
    
    UFUNCTION(BlueprintCallable, meta = (Category = "Capture", DisplayName = "CaptureImage"))
    void Capture(FString FileName);

protected:
    void RunWriterTask(TArray<FColor> Image, int ImageWidth, int ImageHeight, FString FileName);

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void OnBackBufferReady(SWindow& SlateWindow, const FTexture2DRHIRef& BackBuffer);

private:
    void CaptureTextureFromBackBuffer(FRHICommandListImmediate& RHICmdList, const FTexture2DRHIRef& BackBuffer);

private:
    TQueue<struct FWriteRequest*> WriteRequestQueue;
    FCriticalSection CriticalSection;
    FTexture2DRHIRef CachedTexture;
    FThreadSafeBool bIsCapturing;
    FThreadSafeBool bHasNewFrame = false;
#if WITH_EDITOR
    // for PIE in preview window we need to differentiate backbuffers
    FThreadSafeBool bHasCapturedMultiple;
#endif
};
