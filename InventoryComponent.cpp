using UnrealBuildTool;

public class MedievalRPG : ModuleRules
{
	public MedievalRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"Niagara",
			"UMG",
			"Slate",
			"SlateCore",
			"AIModule",
			"NavigationSystem",
			"NetCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Json",
			"JsonUtilities"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
