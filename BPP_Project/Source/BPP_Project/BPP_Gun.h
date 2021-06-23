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
		/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABPP_ProjectProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	virtual void Attack() override;

protected:

};
