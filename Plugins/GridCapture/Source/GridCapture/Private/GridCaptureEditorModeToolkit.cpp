// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridCaptureEditorModeToolkit.h"
#include "GridCaptureEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "GridCaptureEditorModeToolkit"

FGridCaptureEditorModeToolkit::FGridCaptureEditorModeToolkit()
{
}

void FGridCaptureEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FGridCaptureEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FGridCaptureEditorModeToolkit::GetToolkitFName() const
{
	return FName("GridCaptureEditorMode");
}

FText FGridCaptureEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "GridCaptureEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
