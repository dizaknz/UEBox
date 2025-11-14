#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGrid.generated.h"

UCLASS()
class NAVROUTE_API AWorldGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	FVector GetWorldPosForGridCell(const FIntPoint& pos) const;
	FVector GetWorldPosForGridCellCentre(const FIntPoint& pos) const;
	UFUNCTION(BlueprintCallable)
	bool GetGridCellForWorldPos(const FVector& worldPos, FIntPoint& gridPos) const;
	bool IsValidGridCell(const FIntPoint& Location) const;
	bool IsGridCellWalkable(const FIntPoint& Location) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FIntPoint> Cells;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIntPoint MapSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D CellSize;
};
