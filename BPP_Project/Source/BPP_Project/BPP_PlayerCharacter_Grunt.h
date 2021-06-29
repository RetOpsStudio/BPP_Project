// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BPP_Gun_AR4.h"
#include "BPP_ProjectCharacter.h"
#include "BPP_PlayerCharacter_Grunt.generated.h"
class ABPP_Gun;
class UInputComponent;

UCLASS()
class BPP_PROJECT_API ABPP_PlayerCharacter_Grunt : public ABPP_ProjectCharacter
{
	GENERATED_BODY()
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

public:

	ABPP_PlayerCharacter_Grunt();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABPP_ProjectProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;


	/** Primary weapon class and child actor */
	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<ABPP_Gun> PrimaryWeaponClass = ABPP_Gun_AR4::StaticClass();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UChildActorComponent* PrimaryWeapon = nullptr;

	FTimerHandle AttackTimerHandle;

protected:

	virtual void BeginPlay();

	//for replicated variables
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/*Fire code extracted to function*/
	void Fire();

	void FireFromMulticast(FRotator AimRotation = FRotator(0, 0, 0));

	/** Fires a projectile. */
	void OnFire();

	/** Stops firing*/
	void StopFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(FRotator AimRotation = FRotator(0, 0, 0));

	UFUNCTION(Server, Reliable)
	void ServerUpdateLookUp(float Degrees);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastFire(FRotator AimRotation = FRotator(0, 0, 0));

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/*Used to rotate up and down mesh shown at server*/
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = Camera)
	float LookUpServerDeg;

	ABPP_Weapon* AcctualUsedWeapon = nullptr;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};
