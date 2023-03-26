#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "GridCaptureTool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCaptureStartEvent);

UCLASS()
class GRIDCAPTURE_API UGridCaptureToolBuilder : public UInteractiveToolBuilder
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
class GRIDCAPTURE_API UGridCaptureToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:
	UGridCaptureToolProperties();

	UPROPERTY(EditAnywhere, Category = "Capture Options", meta = (ClampMin = 100, ClampMax = 200, DisplayName = "Capture Grid Size"))
	int32 GridSize;

	UPROPERTY()
	FCaptureStartEvent CaptureStartEvent;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Capture Options", meta = (DisplayName = "Start Capture on grid"))
	void Capture() {
		CaptureStartEvent.Broadcast();
	}

};


UCLASS()
class GRIDCAPTURE_API UGridCaptureTool : public USingleClickTool
{
	GENERATED_BODY()

public:
	UGridCaptureTool();

	virtual void SetWorld(UWorld* World);

	virtual void Setup() override;

	virtual void OnClicked(const FInputDeviceRay& ClickPos);

	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void GenerateGridPoints();

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void Capture();

protected:
	UPROPERTY()
	TObjectPtr<UGridCaptureToolProperties> Properties;

	UPROPERTY()
	TArray<FVector> GridPoints;

protected:
	UWorld* TargetWorld;
};
