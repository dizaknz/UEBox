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

void UGridCaptureTool::GenerateGridPoints()
{
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	check(EditorActorSubsystem);

	FBox GridBounds;
	GridBounds.Init();
	bool bFoundNavMesh = false;

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
	FVector Extents = GridBounds.GetExtent();
	FVector Center = GridBounds.GetCenter();
	float MinX = Center.X - Extents.X;
	float MaxX = Center.X + Extents.X;
	float MinY = Center.Y - Extents.Y;
	float MaxY = Center.Y + Extents.Y;

	int32 NumGridX = FGenericPlatformMath::CeilToInt(MaxX - MinX);
	int32 NumGridY = FGenericPlatformMath::CeilToInt(MaxY - MinY);

}


#undef LOCTEXT_NAMESPACE
