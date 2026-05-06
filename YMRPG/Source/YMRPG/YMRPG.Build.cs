// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class YMRPG : ModuleRules
{
	public YMRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		OptimizeCode = CodeOptimization.Never;

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
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"SlateCore",
			"HTTP",
			"JsonUtilities",
			"Json",
			"ModularGameplay"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"YMRPG",

			// Variant_Combat
			"YMRPG/Variant_Combat",
			"YMRPG/Variant_Combat/AI",
			"YMRPG/Variant_Combat/Animation",
			"YMRPG/Variant_Combat/Gameplay",
			"YMRPG/Variant_Combat/Interfaces",
			"YMRPG/Variant_Combat/UI",

			// Variant_Platforming
			"YMRPG/Variant_Platforming",
			"YMRPG/Variant_Platforming/Animation",

			// Variant_SideScrolling
			"YMRPG/Variant_SideScrolling",
			"YMRPG/Variant_SideScrolling/AI",
			"YMRPG/Variant_SideScrolling/Gameplay",
			"YMRPG/Variant_SideScrolling/Interfaces",
			"YMRPG/Variant_SideScrolling/UI",

			// GAS / AbilitySystem
			"YMRPG/AbilitySystem",
			"YMRPG/AbilitySystem/Abilities",
			"YMRPG/AbilitySystem/Attributes",
			"YMRPG/AbilitySystem/Executions",

			// Gameplay Systems
			"YMRPG/Character",
			"YMRPG/Component",
			"YMRPG/DamagedActor",
			"YMRPG/FeedBack",
			"YMRPG/FeedBack/NumberPopComponent",
			"YMRPG/GameMode",
			"YMRPG/Item",
			"YMRPG/Notify",
			"YMRPG/NotifyState",
			"YMRPG/Player",
			"YMRPG/System",

			// UI
			"YMRPG/UI",
			"YMRPG/UI/CharacterInfo",
			"YMRPG/UI/Common",
			"YMRPG/UI/Core",
			"YMRPG/UI/Damage",
			"YMRPG/UI/Equipment",
			"YMRPG/UI/Inventory"
		});
	}
}
