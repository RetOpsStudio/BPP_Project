// Fill out your copyright notice in the Description page of Project Settings.


#include "BPP_Weapon.h"
#include "BPP_PlayerCharacter_Grunt.h"


// Sets default values
ABPP_Weapon::ABPP_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

void ABPP_Weapon::Attack()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Fire")));
	ABPP_PlayerCharacter_Grunt* GunOwner = Cast<ABPP_PlayerCharacter_Grunt>(GetParentActor());
	if(GunOwner && AttackAnimation)
	{
		//if locally controlled, get mesh from owner and play attack animation on it
		if (GunOwner->IsLocallyControlled())
		{
			UAnimInstance* AnimInstance = GunOwner->GetMesh1P()->GetAnimInstance();
			AnimInstance->Montage_Play(AttackAnimation, 1.f);
		} 
		else if (AttackAnimationTp)
		{
			UAnimInstance* AnimInstance = GunOwner->GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(AttackAnimationTp, 1.f);
		}
	}
}

// Called when the game starts or when spawned
void ABPP_Weapon::BeginPlay()
{
	Super::BeginPlay();
}





