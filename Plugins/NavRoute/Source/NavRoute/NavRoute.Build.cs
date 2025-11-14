// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NavRoute : ModuleRules
{
	public NavRoute(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] {
			});
				
		PrivateIncludePaths.AddRange( new string[] {
			});
			
		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"InputCore"
			});
			
		PrivateDependencyModuleNames.AddRange(new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"NavigationSystem",
				"AIModule"
			});
		
		DynamicallyLoadedModuleNames.AddRange(new string[]
			{
			});
	}
}
