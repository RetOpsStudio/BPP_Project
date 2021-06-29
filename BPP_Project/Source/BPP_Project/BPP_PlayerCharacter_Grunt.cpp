// Fill out your copyright notice in the Description page of Project Settings.


#include "BPP_PlayerCharacter_Grunt.h"
#include "BPP_ProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/InputSettings.h"
#include "BPP_Gun.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"

ABPP_PlayerCharacter_Grunt::ABPP_PlayerCharacter_Grunt()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	UCameraComponent* CameraComp = this->GetCameraComponent();
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	CameraComp->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(CameraComp);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//// Create a gun mesh component
	//FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	//FP_Gun->bCastDynamicShadow = false;
	//FP_Gun->CastShadow = false;

	//FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	//FP_MuzzleLocation->SetupAttachment(FP_Gun);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	//mesh wisible to others
	this->GetMesh()->SetOwnerNoSee(true);

	//Create Primary Weapon Actor
	PrimaryWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("PrimaryGun"));
	PrimaryWeapon->SetChildActorClass(ABPP_Gun_AR4::StaticClass());
	
	PrimaryWeapon->CreateChildActor(); //TODO Set Instigator on it
	
}

void ABPP_PlayerCharacter_Grunt::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	if (IsLocallyControlled())
	{
		PrimaryWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}
	else
	{
		PrimaryWeapon->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}

	AcctualUsedWeapon = Cast<ABPP_Weapon>(PrimaryWeapon->GetChildActor());
}

void ABPP_PlayerCharacter_Grunt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABPP_PlayerCharacter_Grunt, LookUpServerDeg, COND_SkipOwner);
}

void ABPP_PlayerCharacter_Grunt::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABPP_PlayerCharacter_Grunt::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABPP_PlayerCharacter_Grunt::StopFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ABPP_PlayerCharacter_Grunt::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABPP_PlayerCharacter_Grunt::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABPP_PlayerCharacter_Grunt::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABPP_PlayerCharacter_Grunt::LookUpAtRate);
}


void ABPP_PlayerCharacter_Grunt::OnFire()
{
	bool bInLoop = AcctualUsedWeapon->bIsAutomatic;
	float InRate = AcctualUsedWeapon->AttackInterval;
	float TimeThatWeaponCanAttackAgain = AcctualUsedWeapon->LastAttackTime + AcctualUsedWeapon->AttackInterval;
	float InFirstDelay = FMath::Clamp<float>(TimeThatWeaponCanAttackAgain - GetWorld()->GetTimeSeconds(), 0, InRate); //prevent overriding fire rate by fast clicking
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ABPP_PlayerCharacter_Grunt::Fire, InRate, bInLoop, InFirstDelay);
}

void ABPP_PlayerCharacter_Grunt::Fire()
{
	if(!AcctualUsedWeapon)
	{
		return;
	}
	FRotator AimRotation = GetControlRotation();
	if (!HasAuthority())
	{
		AcctualUsedWeapon->Attack(AimRotation);
	}
	ServerFire(AimRotation);
	// try and fire a projectile
	//if (ProjectileClass)
	//{
	//	UWorld* const World = GetWorld();
	//	if (World)
	//	{
	//		const FRotator SpawnRotation = GetControlRotation();
	//		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	//		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

	//		//Set Spawn Collision Handling Override
	//		FActorSpawnParameters ActorSpawnParams;
	//		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//		// spawn the projectile at the muzzle
	//		World->SpawnActor<ABPP_ProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

	//		//if (FireAnimation)
	//		//{
	//		//	if (IsLocallyControlled())
	//		//	{
	//		//		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	//		//		if (AnimInstance)
	//		//		{
	//		//			AnimInstance->Montage_Play(FireAnimation, 1.f);
	//		//		}
	//		//	}
	//		//	else
	//		//	{
	//		//		/*  TODO Play fire anim on mesh
	//		//		UAnimInstance* AnimInstance = this->GetMesh()->GetAnimInstance();
	//		//		if (AnimInstance)
	//		//		{
	//		//			AnimInstance->Montage_Play(FireAnimation, 1.f);
	//		//		}
	//		//		*/
	//		//	}
	//		//}

	//		// try and play the sound if specified
	//		if (FireSound != NULL)
	//		{
	//			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//		}
	//	}
	//}
}

void ABPP_PlayerCharacter_Grunt::FireFromMulticast(FRotator AimRotation)
{
	if (!AcctualUsedWeapon)
	{
		return;
	}
	//testing before fully moving weapon to child actor
	if (HasAuthority() || !this->IsLocallyControlled())
	{
		AcctualUsedWeapon->Attack(AimRotation);
	}
}

void ABPP_PlayerCharacter_Grunt::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle); //stops auto attack
}

void ABPP_PlayerCharacter_Grunt::ServerFire_Implementation(FRotator AimRotation)
{
	//Fire();
	NetMulticastFire(AimRotation);
}

bool ABPP_PlayerCharacter_Grunt::ServerFire_Validate(FRotator AimRotation)
{
	return true;
}

void ABPP_PlayerCharacter_Grunt::ServerUpdateLookUp_Implementation(float Degrees)
{
	LookUpServerDeg = Degrees;
}

void ABPP_PlayerCharacter_Grunt::NetMulticastFire_Implementation(FRotator AimRotation)
{
	FireFromMulticast(AimRotation);
}

void ABPP_PlayerCharacter_Grunt::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABPP_PlayerCharacter_Grunt::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ABPP_PlayerCharacter_Grunt::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABPP_PlayerCharacter_Grunt::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	if (HasAuthority())
	{	// if server, save actual rot multiplied by -1 to variable that replicates
		LookUpServerDeg = (GetControlRotation().Pitch * -1);
	}
	else
	{   // or call server to do so
		ServerUpdateLookUp((GetControlRotation().Pitch * -1));
	}
}
