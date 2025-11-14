#include "Index/Graph.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

UGraphNode* UGraph::AddNode(const FName& Id, UStruct* StructRef)
{
    if (Nodes.Contains(Id))
    {
        return Nodes[Id];
    }

    UGraphNode* NewNode = NewObject<UGraphNode>(this, UGraphNode::StaticClass(), NAME_None, RF_Public | RF_Transactional);
    NewNode->NodeId = Id;
    NewNode->StructRef = StructRef;
    Nodes.Add(Id, NewNode);
    return NewNode;
}

UGraphNode* UGraph::GetNode(const FName& Id) const
{
    return Nodes.Contains(Id) ? Nodes[Id] : nullptr;
}

bool UGraph::RemoveNode(const FName& Id)
{
    UGraphNode** NodePtr = Nodes.Find(Id);
    if (!NodePtr) return false;

    UGraphNode* Node = *NodePtr;

    // Remove this node from all neighbours
    for (auto& Pair : Nodes)
    {
        if (Pair.Value != Node)
        {
            Pair.Value->RemoveNeighbour(Node);
        }
    }

    Nodes.Remove(Id);
    return true;
}

void UGraph::AddEdge(const FName& FromId, const FName& ToId)
{
    UGraphNode* FromNode = AddNode(FromId);
    UGraphNode* ToNode   = AddNode(ToId);
    FromNode->AddNeighbour(ToNode);
}

void UGraph::RemoveEdge(const FName& FromId, const FName& ToId)
{
    UGraphNode* FromNode = GetNode(FromId);
    UGraphNode* ToNode   = GetNode(ToId);
    if (FromNode && ToNode)
    {
        FromNode->RemoveNeighbour(ToNode);
    }
}

TArray<FName> UGraph::GetNeighbours(const FName& Id) const
{
    TArray<FName> Result;
    UGraphNode* Node = GetNode(Id);
    if (Node)
    {
        for (UGraphNode* Neighbour : Node->Neighbours)
        {
            if (Neighbour)
            {
                Result.Add(Neighbour->NodeId);
            }
        }
    }
    return Result;
}

bool UGraph::IsReachable(const FName& StartId, const FName& EndId) const
{
    if (!Nodes.Contains(StartId) || !Nodes.Contains(EndId)) return false;

    TSet<FName> Visited;
    TQueue<FName> Queue;
    Queue.Enqueue(StartId);
    Visited.Add(StartId);

    while (!Queue.IsEmpty())
    {
        FName Current;
        Queue.Dequeue(Current);

        if (Current == EndId) return true;

        for (UGraphNode* Neighbour : Nodes[Current]->Neighbours)
        {
            if (Neighbour && !Visited.Contains(Neighbour->NodeId))
            {
                Visited.Add(Neighbour->NodeId);
                Queue.Enqueue(Neighbour->NodeId);
            }
        }
    }
    return false;
}

TArray<FName> UGraph::FindShortestPath(const FName& StartId, const FName& EndId) const
{
    TArray<FName> Result;

    if (!Nodes.Contains(StartId) || !Nodes.Contains(EndId)) return Result;

    // BFS with parent map
    TMap<FName, FName> Parent;
    TSet<FName> Visited;
    TQueue<FName> Queue;

    Queue.Enqueue(StartId);
    Visited.Add(StartId);

    bool bFound = false;

    while (!Queue.IsEmpty() && !bFound)
    {
        FName Current;
        Queue.Dequeue(Current);

        if (Current == EndId)
        {
            bFound = true;
            break;
        }

        for (UGraphNode* Neighbour : Nodes[Current]->Neighbours)
        {
            if (Neighbour && !Visited.Contains(Neighbour->NodeId))
            {
                Visited.Add(Neighbour->NodeId);
                Parent.Add(Neighbour->NodeId, Current);
                Queue.Enqueue(Neighbour->NodeId);
            }
        }
    }

    if (!bFound) return Result;

    // Walk back from EndId to StartId
    FName Trace = EndId;
    while (Trace != StartId)
    {
        Result.Insert(Trace, 0);
        Trace = Parent[Trace];
    }
    Result.Insert(StartId, 0);
    return Result;
}

void UGraph::IndexStructs(const TArray<UStruct*>& Structs)
{
    for (UStruct* Struct : Structs)
    {
        if (!Struct) continue;
        FName Id = Struct->GetFName();
        UGraphNode* Node = GetNode(Id);
        if (!Node)
        {
            AddNode(Id, Struct);
        }
        else
        {
            Node->StructRef = Struct; // optionally update
        }
    }
}

UGraphNode* UGraph::GetNodeForStruct(UStruct* Struct) const
{
    if (!Struct) return nullptr;
    return GetNode(Struct->GetFName());
}

TArray<FName> UGraph::GetAllNodeIds() const
{
    TArray<FName> NodeIds;
    Nodes.GetKeys(NodeIds);
    return NodeIds;
}

