using UnrealBuildTool;

public class UISampleEditor : ModuleRules
{
    public UISampleEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UISample"
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
