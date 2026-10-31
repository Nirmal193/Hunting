// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Hunting : ModuleRules
{
	public Hunting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore",
			"NavigationSystem"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Hunting",
			"Hunting/Variant_Platforming",
			"Hunting/Variant_Platforming/Animation",
			"Hunting/Variant_Combat",
			"Hunting/Variant_Combat/AI",
			"Hunting/Variant_Combat/Animation",
			"Hunting/Variant_Combat/Gameplay",
			"Hunting/Variant_Combat/Interfaces",
			"Hunting/Variant_Combat/UI",
			"Hunting/Variant_SideScrolling",
			"Hunting/Variant_SideScrolling/AI",
			"Hunting/Variant_SideScrolling/Gameplay",
			"Hunting/Variant_SideScrolling/Interfaces",
			"Hunting/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
