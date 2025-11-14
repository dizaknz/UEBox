// Graph.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphNode.h"
#include "Graph.generated.h"

/**
 * A simple directed graph that indexes UStructs.
 * 
 * // TODO:
 * // - struct must associated context tag with actor
 * // - or each node is context tag that links to other context tags each with list of actors per node
 * 
 * The graph is a thin wrapper over a TMap of nodes.
 * All public functions are BlueprintCallable for convenience.
 */
UCLASS(BlueprintType, EditInlineNew, ClassGroup = (Custom))
class UEBOX_API UGraph : public UObject
{
    GENERATED_BODY()

public:
    /** 
     * @brief Create a new node if it does not exist yet
     * @param Id The node identifier
     * @param StructRef The struct that this node represents
     * @return The node
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    UGraphNode* AddNode(const FName& Id, UStruct* StructRef = nullptr);

    /** 
     * @brief Retrieve a node – returns nullptr if the node is missing
     * @param Id The node identifier
     * @return The node
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    UGraphNode* GetNode(const FName& Id) const;

    /** 
     * @brief Remove a node and all edges that reference it
     * @param Id The node identifier
     * @return true if the node was removed
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    bool RemoveNode(const FName& Id);

    /** 
     * @brief Add a directed edge from `FromId` to `ToId`. Creates missing nodes automatically
     * @param FromId The node identifier of the source
     * @param ToId The node identifier of the target
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    void AddEdge(const FName& FromId, const FName& ToId);

    /** 
     * @brief Remove a directed edge
     * @param FromId The node identifier of the source
     * @param ToId The node identifier of the target
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    void RemoveEdge(const FName& FromId, const FName& ToId);

    /** 
     * @brief Get all neighbour IDs for a node
     * @param Id The node identifier
     * @return The neighbour IDs
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    TArray<FName> GetNeighbours(const FName& Id) const;

    /** 
     * @brief Breadth‑first search – returns true if a path exists
     * @param StartId The node identifier of the start
     * @param EndId The node identifier of the end
     * @return true if a path exists
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    bool IsReachable(const FName& StartId, const FName& EndId) const;

    /** 
     * @brief Returns the shortest path as an array of node IDs – empty if no path
     * @param StartId The node identifier of the start
     * @param EndId The node identifier of the end
     * @return The path as an array of node IDs
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    TArray<FName> FindShortestPath(const FName& StartId, const FName& EndId) const;

    /**
     * @brief Takes an array of UStructs and creates a node for each one.
     * The node’s `NodeId` is set to the struct’s `GetFName()`, existing nodes with the same 
     * ID are left untouched (you can overwrite the `StructRef` if you want)
     * @param Structs The array of structs
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    void IndexStructs(const TArray<UStruct*>& Structs);

    /**
     * @brief Returns the node that represents the supplied struct – or nullptr if it isn’t indexed.
     * @param Struct The struct
     * @return The node for the struct
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    UGraphNode* GetNodeForStruct(UStruct* Struct) const;

    /** 
     * @brief Returns an array of all node IDs – handy for UI lists
     * @return The array of node IDs
     */
    UFUNCTION(BlueprintCallable, Category = "Graph")
    TArray<FName> GetAllNodeIds() const;

public:
    /** 
     * @brief All nodes indexed by their ID
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graph")
    TMap<FName, UGraphNode*> Nodes;
};

