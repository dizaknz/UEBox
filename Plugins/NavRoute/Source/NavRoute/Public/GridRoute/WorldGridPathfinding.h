#pragma once

#include "CoreMinimal.h"
#include "GraphAStar.h"
#include "WorldGrid.h"

class NAVROUTE_API WorldGridGraphQueryFilter
{
public:
    WorldGridGraphQueryFilter()
        : WorldGrid(nullptr)
        , AllowPartialPaths(false)
    {}

    void SetWorldGrid(AWorldGrid* InWorldGrid)
    {
        WorldGrid = InWorldGrid;
    }

    float GetHeuristicScale() const
    {
        return 1;
    }

    float GetHeuristicCost(const FIntPoint StartNodeRef, const FIntPoint EndNodeRef) const
    {
        return FMath::Abs(StartNodeRef.X - EndNodeRef.X) + FMath::Abs(StartNodeRef.Y - EndNodeRef.Y);
    }

    float GetTraversalCost(const FIntPoint StartNodeRef, const FIntPoint EndNodeRef) const
    {
        // TODO: implement proper weights
        return 1;
    }

    bool IsTraversalAllowed(const FIntPoint NodeA, const FIntPoint NodeB) const
    {
        if (!WorldGrid->IsGridCellWalkable(NodeA) || !WorldGrid->IsGridCellWalkable(NodeB))
            return false;

        static const FIntPoint kXOffset(1, 0);
        static const FIntPoint kYOffset(0, 1);

        if (NodeB.X < NodeA.X)
        {
            if (NodeB.Y < NodeA.Y)
            {
                if (!WorldGrid->IsGridCellWalkable(NodeA - kXOffset) || !WorldGrid->IsGridCellWalkable(NodeA - kYOffset))
                    return false;
            }
            else if (NodeB.Y > NodeA.Y)
            {
                if (!WorldGrid->IsGridCellWalkable(NodeA - kXOffset) || !WorldGrid->IsGridCellWalkable(NodeA + kYOffset))
                    return false;
            }
        }
        else if (NodeB.X > NodeA.X)
        {
            if (NodeB.Y < NodeA.Y)
            {
                if (!WorldGrid->IsGridCellWalkable(NodeA + kXOffset) || !WorldGrid->IsGridCellWalkable(NodeA - kYOffset))
                    return false;
            }
            else if (NodeB.Y > NodeA.Y)
            {
                if (!WorldGrid->IsGridCellWalkable(NodeA + kXOffset) || !WorldGrid->IsGridCellWalkable(NodeA + kYOffset))
                    return false;
            }
        }
    
        return true;
    }

    bool WantsPartialSolution() const
    {
        return AllowPartialPaths;
    }

    void SetAllowPartialPaths(bool allow)
    {
        AllowPartialPaths = allow;
    }

private:
    AWorldGrid* WorldGrid;
    bool AllowPartialPaths;
};

class NAVROUTE_API WorldGridGraph
{
public:
    void SetWorldGrid(TObjectPtr<AWorldGrid> InWorldGrid)
    {
        WorldGrid = InWorldGrid;
    }

    typedef FIntPoint FNodeRef;

    int32 GetNeighbourCount(FNodeRef NodeRef) const;
    bool IsValidRef(FNodeRef NodeRef) const;
    FNodeRef GetNeighbour(const FNodeRef NodeRef, const int32 NeighbourIndex) const;

private:
    UPROPERTY()
    TObjectPtr<AWorldGrid> WorldGrid;
};
