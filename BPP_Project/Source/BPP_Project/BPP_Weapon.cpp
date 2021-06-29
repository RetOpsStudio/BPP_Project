// Fill out your copyright notice in the Description page of Project Settings.


#include "BPP_Weapon.h"
#include "BPP_PlayerCharacter_Grunt.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABPP_Weapon::ABPP_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootComponent"));

	// Create a gun mesh component
	Weapon_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon_Mesh"));
	Weapon_Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true));
	Weapon_Mesh->bCastDynamicShadow = false;
	Weapon_Mesh->CastShadow = false;


	//UE_LOG(LogTemp, Warning, TEXT("sie tworzy"));

}

void ABPP_Weapon::Attack(FRotator AimRotation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Fire")));
	LastAttackTime = GetWorld()->GetTimeSeconds();
	if(GunOwner && AttackAnimation)
	{
		if (AttackSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());//TODO Set Attenuation
		}
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

	//constructor executes too early for putting this inside
	GunOwner = Cast<ABPP_PlayerCharacter_Grunt>(GetParentActor());
}





