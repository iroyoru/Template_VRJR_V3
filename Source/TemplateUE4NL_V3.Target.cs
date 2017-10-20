// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class TemplateUE4NL_V3Target : TargetRules
{
	public TemplateUE4NL_V3Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "TemplateUE4NL_V3" } );
	}
}
