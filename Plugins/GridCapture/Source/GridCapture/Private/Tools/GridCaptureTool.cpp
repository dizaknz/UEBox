#include "GridCaptureTool.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Components/Button.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Engine/StaticMeshActor.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "NavigationSystem.h"
#include "Editor/EditorEngine.h"
#include "Kismet/GameplayStatics.h"
#include "Spatial/PointHashGrid3.h"

#define LOCTEXT_NAMESPACE "GridCaptureTool"

FString UGridCaptureTool::ActorPrefix = "Grid-";

using namespace UE::Geometry;
using namespace UE::Math;

UInteractiveTool* UGridCaptureToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
    UGridCaptureTool* NewTool = NewObject<UGridCaptureTool>(SceneState.ToolManager);
    NewTool->SetWorld(SceneState.World);
    return NewTool;
}


UGridCaptureToolProperties::UGridCaptureToolProperties()
{
    // 10m
    GridSize = 1000;

}


UGridCaptureTool::UGridCaptureTool()
{
}


void UGridCaptureTool::SetWorld(UWorld* World)
{
    this->TargetWorld = World;
}


void UGridCaptureTool::Setup()
{
    USingleClickTool::Setup();

    Properties = NewObject<UGridCaptureToolProperties>(this);
    AddToolPropertySource(Properties);

    Properties->GenerateGridEvent.AddDynamic(this, &UGridCaptureTool::Generate);
    Properties->RemoveGridEvent.AddDynamic(this, &UGridCaptureTool::Remove);

}


void UGridCaptureTool::OnClicked(const FInputDeviceRay& ClickPos)
{
    FVector NewActorPos = FVector::ZeroVector;

    FVector RayStart = ClickPos.WorldRay.Origin;
    FVector RayEnd = ClickPos.WorldRay.PointAt(99999999.f);
    FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
    FHitResult Result;
    if (TargetWorld->LineTraceSingleByObjectType(Result, RayStart, RayEnd, QueryParams))
    {
        if (AActor* ClickedActor = Result.GetActor())
        {
            FText ActorInfoMsg = FText::Format(LOCTEXT("BasicActorInfo", "Name: {0}"), FText::FromString(Result.GetActor()->GetName()));
            FText Title = LOCTEXT("ActorInfoDialogTitle", "Actor Info");
            FMessageDialog::Open(EAppMsgType::Ok, ActorInfoMsg, &Title);
        }
    }
}

void UGridCaptureTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
}

void UGridCaptureTool::GenerateGridPoints()
{
    TUniquePtr<UE::Geometry::TPointHashGrid3<int, float>> SpatialIndex = TUniquePtr<TPointHashGrid3<int, float>>(
        new TPointHashGrid3<int, float>(Properties->GridSize * 10, 0));

    UE_LOG(LogTemp, Log, TEXT("Generating grid points"));

    UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
    check(EditorActorSubsystem);

    FBox GridBounds;
    GridBounds.Init();
    bool bFoundNavMesh = false;

    float ZLevel = FLT_MAX;

    for (AActor* LevelActor : EditorActorSubsystem->GetAllLevelActors())
    {
        FVector Origin;
        FVector Extent;
        UE_LOG(LogTemp, Log, TEXT("Level actor: %s"), *(LevelActor->GetName()));
        if (LevelActor->IsA(ANavMeshBoundsVolume::StaticClass()))
        {
            UE_LOG(LogTemp, Log, TEXT("Found nav mesh: %s"), *(LevelActor->GetName()));
            bFoundNavMesh = true;
            GridBounds = LevelActor->GetComponentsBoundingBox(true, false);
            ZLevel = LevelActor->GetActorLocation().Z;
            break;
        }
        if (LevelActor->IsA(AStaticMeshActor::StaticClass()))
        {
            // find max bounds of all static actors in level
            const FBox Bounds = LevelActor->GetComponentsBoundingBox(true, false);
            // no Z, just X/Y
            if (Bounds.Min.X < GridBounds.Min.X || Bounds.Min.Y < GridBounds.Min.Y ||
                Bounds.Max.X > GridBounds.Max.X || Bounds.Max.Y > GridBounds.Max.Y)
            {
                GridBounds += Bounds;
            }
            if (Bounds.Min.Z < ZLevel)
            {
                ZLevel = Bounds.Min.Z;
            }
        }
    }

    // generate grid
    FVector Extent = GridBounds.GetExtent();
    FVector Center = GridBounds.GetCenter();
    float MinX = Center.X - Extent.X;
    float MaxX = Center.X + Extent.X;
    float MinY = Center.Y - Extent.Y;
    float MaxY = Center.Y + Extent.Y;

    int32 NumGridX = FGenericPlatformMath::CeilToInt((MaxX - MinX)/Properties->GridSize);
    int32 NumGridY = FGenericPlatformMath::CeilToInt((MaxY - MinY)/Properties->GridSize);

    UE_LOG(LogTemp, Log, TEXT("Grid size [%d, %d]"), NumGridX, NumGridY);

    UNavigationSystemV1* NavSystem = Cast< UNavigationSystemV1>(TargetWorld->GetNavigationSystem());
    if (bFoundNavMesh)
    {
        check(NavSystem);
    }
    FVector QueryPoint;
    TFunctionRef<float (const int &)> DistanceFunc = [&](const int &Idx) { 
        float Distance = FVector::Distance(GridPoints[Idx], QueryPoint);
        return Distance * Distance;
    };

    int Idx = 0;
    for (int X = 0; X < NumGridX; X++)
    { 
        for (int Y = 0; Y < NumGridY; Y++)
        {
            UE_LOG(LogTemp, Log, TEXT("X=%d Y=%d"), X, Y);

            FVector GridPoint(
                (MinX + (Properties->GridSize / 2) + (X * Properties->GridSize)),
                (MinY + (Properties->GridSize / 2) + (Y * Properties->GridSize)),
                ZLevel);

            if (bFoundNavMesh)
            {
                FNavLocation NavLocation;

                // use spatial index to determine grid overlaps
                if (NavSystem->ProjectPointToNavigation(GridPoint, NavLocation, Extent))
                {
                    UE_LOG(LogTemp, Log, TEXT("Found position on navmesh: [%g, %g, %g]"),
                        NavLocation.Location.X,
                        NavLocation.Location.Y,
                        NavLocation.Location.Z);

                    QueryPoint = FVector(NavLocation.Location.X, NavLocation.Location.Y, NavLocation.Location.Z);
                    TVector<float> Pos(QueryPoint.X, QueryPoint.Y, QueryPoint.Z);
                    TPair<int, float> Result = SpatialIndex->FindNearestInRadius(Pos, Properties->GridSize/4, DistanceFunc);
                    if (Result.Key == 0)
                    {
                        GridPoints.Add(NavLocation);
                        SpatialIndex->InsertPoint(Idx, Pos);
                        Idx++;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Inoring position on navmesh: [%g, %g, %g]"),
                            NavLocation.Location.X,
                            NavLocation.Location.Y, 
                            NavLocation.Location.Z);
                    }
                }
                continue;
            }

            // no nav mesh, fall back to actors
            if (GridPoint.X < MaxX && GridPoint.Y < MaxY)
            {
                GridPoints.Add(GridPoint);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Generated %d of grid points"), GridPoints.Num());

    // TODO: sort points topologically FDirectedGraphAlgo

}

void UGridCaptureTool::Generate()
{
    Remove();

    FScopeLock ScopeLock(&Mutex);
    UE_LOG(LogTemp, Log, TEXT("Generating grid"));
    GenerateGridPoints();

    FVector Scale(1, 1, 1);
    FRotator Rotator(0, 0, 0);
    int32 ID = 0;

    for (FVector Location : GridPoints)
    {
        AStaticMeshActor* GridActor = Cast<AStaticMeshActor>(GEditor->AddActor(
            TargetWorld->GetLevel(0),
            AStaticMeshActor::StaticClass(),
            FTransform(Rotator, Location, Scale),
            false,
            RF_Transient));
        FString Name = FString::Printf(TEXT("%s%d"), *ActorPrefix, ID);
        GridActor->Rename(*Name);
        GridActor->SetActorLabel(*Name);
        GEditor->EditorUpdateComponents();
        GridActor->GetStaticMeshComponent()->SetStaticMesh(Properties->Mesh);
        GridActor->GetStaticMeshComponent()->RegisterComponentWithWorld(TargetWorld);
        TargetWorld->UpdateWorldComponents(true, false);
        GridActor->RerunConstructionScripts();
        ID++;
    }
}

void UGridCaptureTool::Remove()
{
    FScopeLock ScopeLock(&Mutex);
    TArray<AActor*> ExistingActors;
    UGameplayStatics::GetAllActorsOfClass(TargetWorld, AStaticMeshActor::StaticClass(), ExistingActors);
    UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
    check(EditorActorSubsystem);
    for (AActor* ExistingActor : ExistingActors)
    {
        if (!ExistingActor->GetName().StartsWith(ActorPrefix))
        {
            continue;
        }
        FString Name = FString::Printf(TEXT("%s_DELETED.%f"), *(ExistingActor->GetName()), FPlatformTime::Seconds());
        ExistingActor->Rename(*Name);
        ExistingActor->ClearActorLabel();
        ExistingActor->Modify();
        TargetWorld->EditorDestroyActor(Cast<AActor>(ExistingActor), true);
    }
    GEditor->ForceGarbageCollection(true);
    GEditor->PerformGarbageCollectionAndCleanupActors();
    if (GridPoints.Num() > 0)
    {
        GridPoints.Empty();
    }

}

#undef LOCTEXT_NAMESPACE
