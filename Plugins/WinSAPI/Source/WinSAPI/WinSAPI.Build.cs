// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class WinSAPI : ModuleRules
{
	public WinSAPI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{

			PublicIncludePaths.AddRange(
				new string[] {
				Path.Combine(PluginDirectory, "Source", "ThirdParty", "Microsoft Speech SDK 5.1", "Include"),
				});

			PublicAdditionalLibraries.AddRange(
			new string[]{
				Path.Combine(PluginDirectory, "Source", "ThirdParty", "Microsoft Speech SDK 5.1", "Lib", "i386", "sapi.lib"),
			});

			PrivateIncludePaths.AddRange(
				new string[] {
				}
				);


			PublicDependencyModuleNames.AddRange(
				new string[]
				{
				"Core",
				"Projects",
				}
				);
		}

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
