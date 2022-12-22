#pragma once

#include "CoreMinimal.h"
#include "ImageWrite.generated.h"

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
