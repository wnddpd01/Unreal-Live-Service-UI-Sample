using UnrealBuildTool;

public class PrismUIEditor : ModuleRules
{
	public PrismUIEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"PrismUI"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AssetRegistry",
			"BlueprintGraph",
			"GraphEditor",
			"Json",
			"KismetCompiler",
			"Slate",
			"SlateCore",
			"UnrealEd"
		});
	}
}
