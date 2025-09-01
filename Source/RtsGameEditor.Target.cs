// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RtsGameEditorTarget : TargetRules
{
	public RtsGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("RtsGame");
	}
}
