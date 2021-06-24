// Fill out your copyright notice in the Description page of Project Settings.


#include "BPP_Gun.h"
#include "BPP_ProjectProjectile.h"
#include "Engine/World.h"
#include "BPP_PlayerCharacter_Grunt.h"

ABPP_Gun::ABPP_Gun()
{


}



void ABPP_Gun::Attack()
{
	Super::Attack();
	if (ProjectileClass)
	{
		//spawn bullet in bullet socket in gun
		FVector BulletSpawnLocation = Weapon_Mesh->GetSocketLocation("BulletSpawn");
		FRotator BulletSpawnRotation = Cast<ABPP_PlayerCharacter_Grunt>(GetParentActor())->GetControlRotation(); //TODO fix, this doesnt replicate from server to other players
		ABPP_ProjectProjectile* SpawnedBullet = GetWorld()->SpawnActor<ABPP_ProjectProjectile>(ProjectileClass, BulletSpawnLocation, BulletSpawnRotation);//TODO Set instigator on owner
	}
}
