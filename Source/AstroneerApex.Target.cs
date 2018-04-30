// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AstroneerApexTarget : TargetRules
{
	public AstroneerApexTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Game;

        ExtraModuleNames.AddRange(new string[] { "AstroneerApex" });
    }
}
