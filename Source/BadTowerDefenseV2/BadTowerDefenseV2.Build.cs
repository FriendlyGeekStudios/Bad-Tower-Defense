// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BadTowerDefenseV2 : ModuleRules
{
	public BadTowerDefenseV2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
    }
}
