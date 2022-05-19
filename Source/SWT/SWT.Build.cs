// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SWT : ModuleRules
{
	public SWT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
