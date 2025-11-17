#include "GridRoute/WorldGridPathfinding.h"

int32 WorldGridGraph::GetNeighbourCount(FNodeRef NodeRef) const
{
    return 8;
}

bool WorldGridGraph::IsValidRef(FNodeRef NodeRef) const
{
    return WorldGrid->IsValidGridCell(NodeRef);
}

WorldGridGraph::FNodeRef WorldGridGraph::GetNeighbour(const FNodeRef NodeRef, const int32 NeighbourIndex) const
{
    static const FIntPoint NeighbourOffset[8] =
    {
        { 0, 1 },
        { 1, 1 },
        { 1, 0 },
        { 1, -1 },
        { 0, -1 },
        { -1, -1 },
        { -1, 0 },
        { -1, 1 },
    };

    return NodeRef + NeighbourOffset[NeighbourIndex];
}
