// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridCaptureEditorMode.h"
#include "GridCaptureEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "GridCaptureEditorModeCommands.h"


#include "Tools/GridTool.h"

#define LOCTEXT_NAMESPACE "GridCaptureEditorMode"

const FEditorModeID UGridCaptureEditorMode::EM_GridCaptureEditorModeId = TEXT("EM_GridCaptureEditorMode");

FString UGridCaptureEditorMode::CaptureToolName = TEXT("GridCapture_ActorInfoTool");

UGridCaptureEditorMode::UGridCaptureEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	Info = FEditorModeInfo(UGridCaptureEditorMode::EM_GridCaptureEditorModeId,
		LOCTEXT("ModeName", "GridTool"),
		FSlateIcon(),
		true);
}


UGridCaptureEditorMode::~UGridCaptureEditorMode()
{
}


void UGridCaptureEditorMode::ActorSelectionChangeNotify()
{
}

void UGridCaptureEditorMode::Enter()
{
	UEdMode::Enter();

	const FGridCaptureEditorModeCommands& ToolCommands = FGridCaptureEditorModeCommands::Get();

	RegisterTool(ToolCommands.GridTool, CaptureToolName, NewObject<UGridToolBuilder>(this));

	GetToolManager()->SelectActiveToolType(EToolSide::Left, CaptureToolName);
}

void UGridCaptureEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FGridCaptureEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UGridCaptureEditorMode::GetModeCommands() const
{
	return FGridCaptureEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
