#include "AI/NavWalker.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


ANavWalker::ANavWalker()
{
	PrimaryActorTick.bCanEverTick = true;

	// Simple movement component – no physics, just a speed value
	auto MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->UpdatedComponent = RootComponent;
}

void ANavWalker::BeginPlay()
{
	Super::BeginPlay();
	// Optionally compute a path right away
	OnFindRandomPath();
}

void ANavWalker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind the Space key to a new path request
	PlayerInputComponent->BindAction("FindPath", IE_Pressed, this, &ANavWalker::OnFindRandomPath);
}

void ANavWalker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsPathValid())
	{
		return; // Nothing to do if we have no valid path
	}

	MoveAlongPath(DeltaTime);
}

bool ANavWalker::IsPathValid() const
{
	return CurrentPath && CurrentPath->IsValid() && CurrentPath->GetPath()->GetPathPoints().Num() > 1;
}

/* --------------------------------------------------------------------------- */
/*  Input / Path Request                                                   */
/* --------------------------------------------------------------------------- */

void ANavWalker::OnFindRandomPath()
{

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
   	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("NavSystem not found!"));
		return;
	}

    FVector OriginLocation = GetActorLocation(); // Or any other origin point
    float SearchRadius = 1000.0f; // Define your search radius

    FNavLocation RandomNavigablePoint;
    if (NavSys->GetRandomPointInNavigableRadius(OriginLocation, SearchRadius, RandomNavigablePoint))
    {
        FVector RandomPoint = RandomNavigablePoint.Location;
	    ComputePathToLocation(RandomPoint);
    }
}

/* --------------------------------------------------------------------------- */
/*  Path Computation (A* under the hood)                                 */
/* --------------------------------------------------------------------------- */

void ANavWalker::ComputePathToLocation(const FVector& Destination)
{
	// Grab the navigation system for this world
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("NavSystem not found!"));
		return;
	}

	// Compute a synchronous path – this is where A* runs inside the engine
	CurrentPath = NavSys->FindPathToLocationSynchronously(
		GetWorld(),
		GetActorLocation(),
		Destination
	);

	if (CurrentPath && CurrentPath->IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Found path with %d points."), CurrentPath->GetPath()->GetPathPoints().Num());
		TargetPointIndex = 1; // 0 is the start location
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid path found."));
	}
}

/* --------------------------------------------------------------------------- */
/*  Movement along the computed path                                         */
/* --------------------------------------------------------------------------- */

void ANavWalker::MoveAlongPath(float DeltaTime)
{
	if (!CurrentPath || TargetPointIndex >= CurrentPath->GetPath()->GetPathPoints().Num())
	{
		return; // Reached end
	}

	const FVector TargetLoc = CurrentPath->GetPath()->GetPathPoints()[TargetPointIndex].Location;
	const FVector Dir = (TargetLoc - GetActorLocation()).GetSafeNormal();
	FVector NewLoc = GetActorLocation() + Dir * MoveSpeed * DeltaTime;

	// If we overshoot, clamp to the target point
	if (FVector::DistSquared(NewLoc, TargetLoc) < FMath::Square(MoveSpeed * DeltaTime))
	{
		NewLoc = TargetLoc;
		TargetPointIndex++; // Advance to next point
	}

	SetActorLocation(NewLoc);
}
