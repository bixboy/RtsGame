// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RtsGameTarget : TargetRules
{
	public RtsGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("RtsGame");
	}
}
