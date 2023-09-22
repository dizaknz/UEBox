// Copyright The Boundary 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCaptureBeginEvent);

UCLASS()
class IMAGECAPTURE_API ACaptureActor : public AActor
{
    GENERATED_BODY()
    
public:    
    ACaptureActor();
    virtual void BeginPlay() override;

public:    
    UFUNCTION(BlueprintCallable, Category = "Capture")
    void Capture(const FVector& Location, const FString& Name);

    UFUNCTION(BlueprintCallable, Category = "Capture")
    bool Initialise(enum EOutputFormat Format, const FString& Directory);

public:
    UPROPERTY(VisibleAnyWhere, Category = "Capture")
    FCaptureBeginEvent CaptureBeginEvent;    

protected:
    UPROPERTY(VisibleAnyWhere, Category = "Capture")
    class UCaptureComponent* CaptureComp;

};
