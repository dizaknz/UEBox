// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UEBox : ModuleRules
{
	public UEBox(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore" ,
			"AIModule",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
