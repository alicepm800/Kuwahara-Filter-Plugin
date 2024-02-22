// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class KuwaharaFilterPluginEditorTarget : TargetRules
{
	public KuwaharaFilterPluginEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("KuwaharaFilterPlugin");
        ExtraModuleNames.Add("MyCustomModule");
	}
}
