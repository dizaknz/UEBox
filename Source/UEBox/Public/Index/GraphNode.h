// GraphNode.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphNode.generated.h"

/**
 * @brief A node in the graph.
 * 
 * It holds a reference to the `UStruct` it represents and a list of outgoing edges.
 */
UCLASS(BlueprintType, EditInlineNew)
class UEBOX_API UGraphNode : public UObject
{
    GENERATED_BODY()

public:
    /** 
     * @brief Returns the neighbour count
     */
    UFUNCTION(BlueprintCallable, Category = "Graph Node")
    int32 GetNeighbourCount() const;

    /** 
     * @brief Add a neighbour if it isn’t already present
     * @param NewNeighbour The neighbour to add
     */
    UFUNCTION(BlueprintCallable, Category = "Graph Node")
    void AddNeighbour(UGraphNode* NewNeighbour);

    /** 
     * @brief Remove a neighbour
     * @param Neighbour The neighbour to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Graph Node")
    void RemoveNeighbour(UGraphNode* Neighbour);

public:
    /** 
     * @brief Unique identifier for the node – usually the struct’s FName.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graph Node")
    FName NodeId;

    /** 
     * @brief The struct that this node represents.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graph Node")
    UStruct* StructRef = nullptr;

    /** 
     * @brief Outgoing edges – list of neighbour nodes.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graph Node")
    TArray<UGraphNode*> Neighbours;
};

