#include "GridRoute/WorldGridNavigationData.h"
#include "AI/Navigation/NavigationTypes.h"
#include "GridRoute/WorldGrid.h"
#include "EngineUtils.h"

AWorldGridNavigationData::AWorldGridNavigationData(const FObjectInitializer& ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		FindPathImplementation = FindPath;
		FindHierarchicalPathImplementation = FindPath;

		Pathfinder = new FGraphAStar<WorldGridGraph>(Graph);
	}
}

void AWorldGridNavigationData::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AWorldGrid> It(GetWorld(), AWorldGrid::StaticClass()); It; ++It)
	{
		WorldGridActor = *It;
		Graph.SetWorldGrid(WorldGridActor);
		break;
	}
}

bool AWorldGridNavigationData::ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& Extent, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	FIntPoint gridPos;
	if (WorldGridActor->GetGridCellForWorldPos(Point, gridPos))
	{
		OutLocation = FNavLocation(Point);
		return true;
	}

	return false;
}

FPathFindingResult AWorldGridNavigationData::FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query)
{
 const ANavigationData* Self = Query.NavData.Get();
 check(Self != nullptr);
 const AWorldGridNavigationData* AStar = (const AWorldGridNavigationData*)Self;
 check(AStar != nullptr);

	if (AStar == nullptr)
		return ENavigationQueryResult::Error;

	FPathFindingResult Result(ENavigationQueryResult::Error);
	Result.Path = Query.PathInstanceToFill.IsValid() ? Query.PathInstanceToFill : Self->CreatePathInstance<FNavigationPath>(Query);

	FNavigationPath* NavPath = Result.Path.Get();

	if (NavPath != nullptr)
	{
		if ((Query.StartLocation - Query.EndLocation).IsNearlyZero())
		{
			Result.Path->GetPathPoints().Reset();
			Result.Path->GetPathPoints().Add(FNavPathPoint(Query.EndLocation));
			Result.Result = ENavigationQueryResult::Success;
		}
		else if (Query.QueryFilter.IsValid())
		{
			FIntPoint myGridPos, targetGridPos;
			AStar->WorldGridActor->GetGridCellForWorldPos(Query.StartLocation, myGridPos);
			AStar->WorldGridActor->GetGridCellForWorldPos(Query.EndLocation, targetGridPos);

			WorldGridGraphQueryFilter queryFilter;
			queryFilter.SetWorldGrid(AStar->WorldGridActor);
			queryFilter.SetAllowPartialPaths(Query.bAllowPartialPaths);

			TArray<FIntPoint> Path;
			EGraphAStarResult aStarResult = AStar->Pathfinder->FindPath(myGridPos, targetGridPos, queryFilter, Path);

			if (aStarResult == EGraphAStarResult::SearchFail || aStarResult == EGraphAStarResult::InfiniteLoop)
			{
				UE_LOG(LogTemp, Warning, TEXT("AWorldGridNavigationData A* fail: %d"), aStarResult);

				Result.Result = ENavigationQueryResult::Fail;
				return Result;
			}

			Path.Insert(myGridPos, 0);

			for (auto& point : Path)
			{
				NavPath->GetPathPoints().Add(FNavPathPoint(AStar->WorldGridActor->GetWorldPosForGridCellCentre(point)));
			}

			UE_LOG(LogTemp, Warning, TEXT("WorldGridNav path (%d points):"), Path.Num());
			for (int i = 0; i < Path.Num(); i++)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s (%s)"), *Path[i].ToString(), *NavPath->GetPathPoints()[i].Location.ToString());
			}
			UE_LOG(LogTemp, Warning, TEXT("WorldGridNav path end"));

			NavPath->MarkReady();
			Result.Result = ENavigationQueryResult::Success;
		}
	}

	return Result;
}
