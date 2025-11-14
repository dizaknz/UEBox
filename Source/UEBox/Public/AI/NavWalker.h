#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavWalker.generated.h"

/**
 *  A very small pawn that uses the Navigation System to
 *  find an A* path and move along it.
 *
 *  - Press Space to start a new path.
 *  - The pawn will automatically move along the computed points.
 */
UCLASS()
class UEBOX_API ANavWalker : public APawn
{
	GENERATED_BODY()

public:
	ANavWalker();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	/** 
	 * @brief Start a new path from the pawn's current location to the specified world location 
	 * @param Destination The location to move to
	 */
	void ComputePathToLocation(const FVector& Destination);

	/**
	 * @brief Move the pawn toward the next point in the path
	 * @param DeltaTime The time since the last tick
	 */
	void MoveAlongPath(float DeltaTime);

	/** 
	 * @brief Helper: Is the path still valid?
	 */
	bool IsPathValid() const;

	/** 
	 * @brief Called by input to trigger a new path to a random point
	 */
	void OnFindRandomPath();

protected:
	/** 
	 * @brief Speed in units per second
	 */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 300.f;

private:
	/** 
	 * @brief The navigation path we are following
	 */
	UPROPERTY()
	TObjectPtr<UNavigationPath> CurrentPath;

	/** 
	 * @brief Index of the next point in the path we’re heading toward
	 */
	int32 TargetPointIndex = 0;

};
