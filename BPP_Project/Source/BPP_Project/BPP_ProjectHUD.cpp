// Copyright Epic Games, Inc. All Rights Reserved.

#include "BPP_ProjectHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

ABPP_ProjectHUD::ABPP_ProjectHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void ABPP_ProjectHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X),
		(Center.Y));
	// draw the crosshair
	TileItem = new FCanvasTileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem->BlendMode = SE_BLEND_Translucent;
	CrosshairSize = TileItem->Size;

	Canvas->DrawItem(*TileItem);
	
	
	
}


