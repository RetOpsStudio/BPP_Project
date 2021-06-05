// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BPP_Project : ModuleRules
{
	public BPP_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
