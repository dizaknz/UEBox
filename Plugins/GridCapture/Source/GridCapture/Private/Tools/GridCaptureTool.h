#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "GridCaptureTool.generated.h"

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

	UPROPERTY(VisibleAnywhere, Category = "Capture Options", meta = (DisplayName = "Capture on grid"))
	class UButton* CaptureButton;
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

	UFUNCTION(BlueprintCallable, Category = "Capture Grid")
	void GenerateGridPoints();
protected:
	UPROPERTY()
	TObjectPtr<UGridCaptureToolProperties> Properties;

	UPROPERTY()
	TArray<FIntPoint> GridPoints;

protected:
	UWorld* TargetWorld;
};
