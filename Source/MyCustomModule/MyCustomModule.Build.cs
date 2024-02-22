// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;
using System.IO;

public class MyCustomModule : ModuleRules
{
	public MyCustomModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Renderer", "RenderCore", "RHI" });

        string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
        PublicIncludePaths.Add(EnginePath + "Source/Runtime/Renderer/Private");
    }
}
