#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "ImageCapturer.generated.h"

USTRUCT()
struct FWriteRequest
{
    GENERATED_BODY()

    TArray<FColor> Image;
    FRenderCommandFence RenderFence;
    FString FileName;
    int Width;
    int Height;
};

UCLASS()
class AImageCapturer : public AActor
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

    virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

    void OnBackBufferReady(SWindow& SlateWindow, const FTexture2DRHIRef& backBuffer);

private:
    TQueue<FWriteRequest*> WriteRequestQueue;
    FCriticalSection CriticalSection;
    FTexture2DRHIRef CachedTexture;
    FThreadSafeBool bIsCapturing;
    FThreadSafeBool bHasNewFrame = false;
#if WITH_EDITOR
    // for PIE in preview window we need to differentiate backbuffers
    FThreadSafeBool bHasCapturedMultiple;
#endif
};

class FImageWriterTask : public FNonAbandonableTask
{
public:
    FImageWriterTask(TArray<FColor> Image, int Width, int Height, FString FileName);
    ~FImageWriterTask() = default;

    FORCEINLINE TStatId GetStatId() const
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(FImageWriterTask, STATGROUP_ThreadPoolAsyncTasks);
    }

protected:
    TArray<FColor> Image;
    FString FileName;
    int ImageWidth = 0;
    int ImageHeight = 0;

public:
    void DoWork();
};
