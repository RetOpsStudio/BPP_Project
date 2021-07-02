// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BPP_Weapon.h"
#include "BPP_Gun.generated.h"

/**
 * 
 */
UCLASS()
class BPP_PROJECT_API ABPP_Gun : public ABPP_Weapon
{
	GENERATED_BODY()
public:

	ABPP_Gun();
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	FTransform GetSightTransform(); //return location where owner camera should be to aim down sights

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABPP_ProjectProjectile> ProjectileClass;

	/** Muzzle flash particle class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UParticleSystem* MuzzleFlash;

	virtual void Attack(FRotator AimRotation = FRotator(0, 0, 0)) override;

protected:
	

};
