// Fill out your copyright notice in the Description page of Project Settings.


#include "BPP_Gun_AR4.h"
#include "BPP_PlayerCharacter_Grunt.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Pawn.h"


ABPP_Gun_AR4::ABPP_Gun_AR4()
{
	
	bIsAutomatic = true;
	AttackRate = 600; //per minute

	AttackInterval = 60 / AttackRate; //in seconds

}

