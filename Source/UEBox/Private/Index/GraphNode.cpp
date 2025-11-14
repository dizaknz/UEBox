#include "Index/GraphNode.h"

int32 UGraphNode::GetNeighbourCount() const { 
    return Neighbours.Num(); 
}

void UGraphNode::AddNeighbour(UGraphNode* NewNeighbour)
{
    if (NewNeighbour && !Neighbours.Contains(NewNeighbour))
    {
        Neighbours.Add(NewNeighbour);
    }
}

void UGraphNode::RemoveNeighbour(UGraphNode* Neighbour)
{
    if (Neighbour)
    {
        Neighbours.Remove(Neighbour);
    }
}

