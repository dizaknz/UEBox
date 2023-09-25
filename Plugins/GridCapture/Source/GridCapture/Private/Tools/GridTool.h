#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "GridTool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGenerateGridEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRemoveGridEvent);

UCLASS()
class GRIDCAPTURE_API UGridToolBuilder : public UInteractiveToolBuilder
{
    GENERATED_BODY()

public:
    virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override 
    { 
        return true; 
    }
    virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


UCLASS(Transient)

class GRIDCAPTURE_API UGridToolProperties : public UInteractiveToolPropertySet
{
    GENERATED_BODY()
public:
    UGridToolProperties();

    UPROPERTY(EditAnywhere, Category = "Generate Options", meta = (DisplayName = "Grid Cell Size"))
    int32 GridSize;

    UPROPERTY(EditAnywhere, Category = "Generate Options", meta = (DisplayName = "Grid Actor Mesh"))
    UStaticMesh* Mesh;

    FGenerateGridEvent GenerateGridEvent;
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Grid Actions", meta = (DisplayName = "Generate grid"))
    void Generate() {
        GenerateGridEvent.Broadcast();
    }
    FGenerateGridEvent RemoveGridEvent;
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Grid Actions", meta = (DisplayName = "Remove grid"))
    void Remove() {
        RemoveGridEvent.Broadcast();
    }


};


UCLASS()
class GRIDCAPTURE_API UGridTool : public USingleClickTool
{
    GENERATED_BODY()

public:
    UGridTool();

    virtual void SetWorld(UWorld* World);

    virtual void Setup() override;

    virtual void OnClicked(const FInputDeviceRay& ClickPos);

    virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

    UFUNCTION(BlueprintCallable, Category = "Capture")
    void Generate();

    UFUNCTION(BlueprintCallable, Category = "Capture")
    void Remove();

private:
    void GenerateGridPoints();

protected:
    UPROPERTY()
    TObjectPtr<UGridToolProperties> Properties;

    UPROPERTY()
    TArray<FVector> GridPoints;

    TObjectPtr<UWorld> TargetWorld;

    static FString ActorPrefix;

private:
    FCriticalSection Mutex;

};
