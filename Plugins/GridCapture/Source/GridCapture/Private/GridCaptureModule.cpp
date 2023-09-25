// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridCaptureModule.h"
#include "GridCaptureEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "GridCaptureModule"

void FGridCaptureModule::StartupModule()
{
	FGridCaptureEditorModeCommands::Register();
}

void FGridCaptureModule::ShutdownModule()
{
	FGridCaptureEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGridCaptureModule, GridCaptureEditorMode)