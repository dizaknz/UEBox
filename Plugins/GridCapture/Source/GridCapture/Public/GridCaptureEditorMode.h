// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "GridCaptureEditorMode.generated.h"

UCLASS()
class UGridCaptureEditorMode : public UEdMode
{
	GENERATED_BODY()

public:
	const static FEditorModeID EM_GridCaptureEditorModeId;

	static FString CaptureToolName;

	UGridCaptureEditorMode();
	virtual ~UGridCaptureEditorMode();

	/** UEdMode interface */
	virtual void Enter() override;
	virtual void ActorSelectionChangeNotify() override;
	virtual void CreateToolkit() override;
	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;
};
