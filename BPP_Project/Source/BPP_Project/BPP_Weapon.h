// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BPP_Weapon.generated.h"


UCLASS()
class BPP_PROJECT_API ABPP_Weapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABPP_Weapon();

	/*Every weapon got kind of attack*/
	virtual void Attack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	/** Every kind of weapon got mesh*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Weapon_Mesh;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* AttackSound;


	/** AnimMontage to play each time we Attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* AttackAnimation;

	/** AnimMontage to play for other players each time we Attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* AttackAnimationTp;

	

};
