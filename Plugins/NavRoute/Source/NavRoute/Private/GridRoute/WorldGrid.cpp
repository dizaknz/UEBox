#include "GridRoute/WorldGrid.h"

bool AWorldGrid::GetGridCellForWorldPos(const FVector& WorldPos, FIntPoint& GridPos) const
{
    FVector Loc = GetActorLocation();

    GridPos.Y = (WorldPos.X - Loc.X) / CellSize.X;
    GridPos.X = (WorldPos.Y - Loc.Y) / CellSize.Y;

    return (GridPos.X >= 0 && GridPos.Y >= 0 && GridPos.X < MapSize.X && GridPos.Y < MapSize.Y);
}

FVector AWorldGrid::GetWorldPosForGridCell(const FIntPoint& Pos) const
{
    FVector Loc = GetActorLocation();
    return FVector(Pos.Y * CellSize.X + Loc.X, Pos.X * CellSize.Y + Loc.Y, Loc.Z);
}

FVector AWorldGrid::GetWorldPosForGridCellCentre(const FIntPoint& Pos) const
{
    return GetWorldPosForGridCell(Pos) + (FVector(CellSize.Y, CellSize.X, 0) * 0.5f);
}

bool AWorldGrid::IsValidGridCell(const FIntPoint& Location) const
{
    return (Location.X >= 0 && Location.Y >= 0 && Location.X < MapSize.X && Location.Y < MapSize.Y);
}

bool AWorldGrid::IsGridCellWalkable(const FIntPoint& Location) const
{
    return (!Cells.Contains(Location));
}
