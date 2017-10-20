// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class TemplateUE4NL_V3EditorTarget : TargetRules
{
	public TemplateUE4NL_V3EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "TemplateUE4NL_V3" } );
	}
}
