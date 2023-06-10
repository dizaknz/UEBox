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

#define LOCTEXT_NAMESPACE "GridCaptureTool"

UInteractiveTool* UGridCaptureToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
    UGridCaptureTool* NewTool = NewObject<UGridCaptureTool>(SceneState.ToolManager);
    NewTool->SetWorld(SceneState.World);
    return NewTool;
}


UGridCaptureToolProperties::UGridCaptureToolProperties()
{
    // cm
    GridSize = 150;

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

    Properties->CaptureStartEvent.AddDynamic(this, &UGridCaptureTool::Capture);
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
    UE_LOG(LogTemp, Log, TEXT("Generating grid points"));

    UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
    check(EditorActorSubsystem);

    FBox GridBounds;
    GridBounds.Init();
    bool bFoundNavMesh = false;

    for (AActor* LevelActor : EditorActorSubsystem->GetAllLevelActors())
    {
        FVector Origin;
        FVector Extent;
        // TODO: filter actors that are not visible
        UE_LOG(LogTemp, Log, TEXT("Level actor: %s"), *(LevelActor->GetName()));
        if (LevelActor->IsA(ANavMeshBoundsVolume::StaticClass()))
        {
            UE_LOG(LogTemp, Log, TEXT("Found nav mesh: %s"), *(LevelActor->GetName()));
            bFoundNavMesh = true;
            GridBounds = LevelActor->GetComponentsBoundingBox(true, false);
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
    for (int X = 0; X < NumGridX; X++)
    { 
        for (int Y = 0; Y < NumGridY; Y++)
        {
            UE_LOG(LogTemp, Log, TEXT("X=%d Y=%d"), X, Y);

            FVector GridPoint(
                (MinX + (Properties->GridSize / 2) + (X * Properties->GridSize)),
                (MinY + (Properties->GridSize / 2) + (Y * Properties->GridSize)),
                0);

            if (bFoundNavMesh)
            {
                FNavLocation NavLocation;

                // FIXME
                if (NavSystem->ProjectPointToNavigation(GridPoint, NavLocation, Extent))
                {
                    UE_LOG(LogTemp, Log, TEXT("Found position on navmesh: [%g, %g, %g]"), NavLocation.Location.X, NavLocation.Location.Y, NavLocation.Location.Z);
                    GridPoints.Add(NavLocation);
                }
                continue;
            }

            // no nav mesh fall back to actors
            if (GridPoint.X < MaxX && GridPoint.Y < MaxY)
            {
                GridPoints.Add(GridPoint);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Generated %d of grid points"), GridPoints.Num());

    // TODO: sort points topologically FDirectedGraphAlgo

    /* iterate through rows
        - filter points in same cell, closed to center wins
        - connect neighbours

        4 5         6
        |           |   
        1 <-> 2 <-> 3
    
    */
    
}

void UGridCaptureTool::Capture()
{
    UE_LOG(LogTemp, Log, TEXT("Capturing on grid"));
    GenerateGridPoints();

    FVector Scale(1, 1, 1);
    FRotator Rotator(0, 0, 0);
    int32 ID = 0;

    for (FVector Location : GridPoints)
    {
        AStaticMeshActor* GridActor = Cast<AStaticMeshActor>(GEditor->AddActor(
            TargetWorld->GetLevel(0),
            AStaticMeshActor::StaticClass(),
            FTransform(Rotator, Location, Scale)));
        FString Name = FString::Printf(TEXT("Grid_%d"), ID);
        GridActor->Rename(*Name);
        GridActor->SetActorLabel(*Name);
        GEditor->EditorUpdateComponents();
        /* TODO:
            GridActor->GetStaticMeshComponent()->RegisterComponentWithWorld(currentWorld);
            TargetWorld->UpdateWorldComponents(true, false);
            GridActor->RerunConstructionScripts();
        */
        ID++;
    }
}


#undef LOCTEXT_NAMESPACE
