using UnrealBuildTool;
using System.Collections.Generic;

public class MedievalRPGEditorTarget : TargetRules
{
	public MedievalRPGEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "MedievalRPG" });
	}
}
