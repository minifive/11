// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ImportTool : ModuleRules
{
	public ImportTool(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateIncludePaths.AddRange(
            new string[]
            {
               
                "Developer/DerivedDataCache/Public",
                "Developer/TargetPlatform/Public",
            }
        );
        PrivateIncludePathModuleNames.AddRange(
             new string[]{
                 "ProjectTargetPlatformEditor",
            }
          );
        PublicDependencyModuleNames.AddRange(
              new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "UnrealEd",
                "RawMesh"
              }
          );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "InputCore",
                "SlateCore",
                "Engine",
                "PropertyEditor",
                "LevelEditor",
                "TargetPlatform",
                "TargetDeviceServices",
            }
        );
    }
}
