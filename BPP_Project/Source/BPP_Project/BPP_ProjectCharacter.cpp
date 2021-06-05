// Copyright Epic Games, Inc. All Rights Reserved.

#include "BPP_ProjectCharacter.h"
#include "BPP_ProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"



DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ABPP_ProjectCharacter

ABPP_ProjectCharacter::ABPP_ProjectCharacter()
{

}


void ABPP_ProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}




