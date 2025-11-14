#include "GridRoute/WorldGrid.h"

bool AWorldGrid::GetGridCellForWorldPos(const FVector& worldPos, FIntPoint& gridPos) const
{
	FVector myLoc = GetActorLocation();

	gridPos.Y = (worldPos.X - myLoc.X) / CellSize.X;
	gridPos.X = (worldPos.Y - myLoc.Y) / CellSize.Y;

	return (gridPos.X >= 0 && gridPos.Y >= 0 && gridPos.X < MapSize.X && gridPos.Y < MapSize.Y);
}

FVector AWorldGrid::GetWorldPosForGridCell(const FIntPoint& pos) const
{
	FVector myLoc = GetActorLocation();
	return FVector(pos.Y * CellSize.X + myLoc.X, pos.X * CellSize.Y + myLoc.Y, myLoc.Z);
}

FVector AWorldGrid::GetWorldPosForGridCellCentre(const FIntPoint& pos) const
{
	return GetWorldPosForGridCell(pos) + (FVector(CellSize.Y, CellSize.X, 0) * 0.5f);
}

bool AWorldGrid::IsValidGridCell(const FIntPoint& Location) const
{
	return (Location.X >= 0 && Location.Y >= 0 && Location.X < MapSize.X && Location.Y < MapSize.Y);
}

bool AWorldGrid::IsGridCellWalkable(const FIntPoint& Location) const
{
	return (!Cells.Contains(Location));
}
