// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NGDMP : ModuleRules
{
	public NGDMP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", 
			"Engine", "InputCore", 
			"EnhancedInput", "GameplayTags",
			"StateTreeModule", "GameplayStateTreeModule",
			"UMG", "Slate", "SlateCore", "Niagara"
		});
		
		if (Target.Type == TargetType.Editor)
		{
			PublicDependencyModuleNames.AddRange(new string[]
			{
				"StateTreeEditorModule",
				"AssetTools", // Only needed for the editor
				"StateTreeTestSuite",
				"UMGEditor",
			});
		}
		
		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
