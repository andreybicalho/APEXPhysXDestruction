// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "AstroneerApexHUD.generated.h"

UCLASS()
class AAstroneerApexHUD : public AHUD
{
	GENERATED_BODY()

public:
	AAstroneerApexHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

