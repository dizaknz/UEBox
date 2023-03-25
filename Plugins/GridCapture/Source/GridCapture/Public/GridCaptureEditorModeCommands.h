// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FGridCaptureEditorModeCommands : public TCommands<FGridCaptureEditorModeCommands>
{
public:
	FGridCaptureEditorModeCommands();

	virtual void RegisterCommands() override;
	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();

	TSharedPtr<FUICommandInfo> CaptureTool;

protected:
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};
