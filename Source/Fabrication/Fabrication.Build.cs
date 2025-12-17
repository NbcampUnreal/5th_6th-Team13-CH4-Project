// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Fabrication : ModuleRules
{
	public Fabrication(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "UMG",
            "Slate",
            "SlateCore",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "NetCore",
            "Json",
            "JsonUtilities",
            "HTTP"
        });

        PublicIncludePaths.AddRange(new string[]
        {
            "Fabrication",
		});
        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
