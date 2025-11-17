#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGrid.generated.h"

UCLASS()
class NAVROUTE_API AWorldGrid : public AActor
{
    GENERATED_BODY()
    
public:    
    UFUNCTION(BlueprintCallable)
    FVector GetWorldPosForGridCell(const FIntPoint& pos) const;

    UFUNCTION(BlueprintCallable)
    FVector GetWorldPosForGridCellCentre(const FIntPoint& pos) const;

    UFUNCTION(BlueprintCallable)
    bool GetGridCellForWorldPos(const FVector& worldPos, FIntPoint& gridPos) const;

    UFUNCTION(BlueprintCallable)
    bool IsValidGridCell(const FIntPoint& Location) const;

    UFUNCTION(BlueprintCallable)
    bool IsGridCellWalkable(const FIntPoint& Location) const;

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FIntPoint> Cells;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FIntPoint MapSize;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D CellSize;
};
