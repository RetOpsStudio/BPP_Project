// Fill out your copyright notice in the Description page of Project Settings.


#include "BPP_Gun.h"
#include "BPP_ProjectProjectile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "BPP_PlayerCharacter_Grunt.h"
#include "TimerManager.h"

ABPP_Gun::ABPP_Gun()
{


}



void ABPP_Gun::Attack(FRotator AimRotation)
{
	Super::Attack(AimRotation);
	
	if (ProjectileClass)
	{
		//spawn bullet in  socket 
		FVector BulletSpawnLocation = Weapon_Mesh->GetSocketLocation("BulletSpawn");
		ABPP_ProjectProjectile* SpawnedBullet = GetWorld()->SpawnActor<ABPP_ProjectProjectile>(ProjectileClass, BulletSpawnLocation, AimRotation);//TODO Set instigator on owner
		SpawnedBullet->SetOwner(GunOwner);
		//DrawDebugLine(GetWorld(), BulletSpawnLocation, BulletSpawnLocation + AimRotation.Vector() * 1000.f, FColor(255, 0, 0), false, 2.f);
	}
}
