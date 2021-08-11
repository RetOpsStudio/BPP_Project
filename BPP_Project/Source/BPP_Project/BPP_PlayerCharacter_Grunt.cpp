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
#include "BPP_ProjectHUD.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

ABPP_PlayerCharacter_Grunt::ABPP_PlayerCharacter_Grunt()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	//UCameraComponent* CameraComp = this->GetCameraComponent();
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;
	// Create AimCameraComponent
	AimCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("AimCamera"));
	AimCameraComp->bUsePawnControlRotation = true;
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(CameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	//Mesh1P->SetupAttachment(RootComponent);
	AimCameraComp->SetupAttachment(Mesh1P);

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

	// Bind Aim event
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABPP_PlayerCharacter_Grunt::OnAim);

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
	float InFirstDelay = FMath::Clamp<float>(TimeThatWeaponCanAttackAgain - GetWorld()->GetTimeSeconds(), 0, InRate); //prevents overriding fire rate by fast clicking
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ABPP_PlayerCharacter_Grunt::Fire, InRate, bInLoop, InFirstDelay);
}

void ABPP_PlayerCharacter_Grunt::OnAim()
{
	if (!bIsADS)
	{
		if (Cast<ABPP_Gun>(AcctualUsedWeapon)) // if weapon is gun TODO check if it shouldnt be interface
		{
			AimCameraComp->SetWorldTransform(CameraComponent->GetComponentTransform());
			GetWorld()->GetTimerManager().ClearTimer(AimTimerHandle);
			bIsADS = true;
			AimCameraComp->SetActive(true);
			CameraComponent->SetActive(false);
			GetWorld()->GetTimerManager().SetTimer(AimTimerHandle, this, &ABPP_PlayerCharacter_Grunt::AimDownSights, GetWorld()->GetDeltaSeconds(), true);
			Cast<ABPP_ProjectHUD>(Cast<APlayerController>(this->GetOwner())->GetHUD())->ShowHUD();//temporary solution
		}
	}
	else
	{
		if (Cast<ABPP_Gun>(AcctualUsedWeapon)) // if weapon is gun TODO check if it shouldnt be interface
		{
			GetWorld()->GetTimerManager().ClearTimer(AimTimerHandle);
			bIsADS = false;
			GetWorld()->GetTimerManager().SetTimer(AimTimerHandle, this, &ABPP_PlayerCharacter_Grunt::StopAimDownSights, GetWorld()->GetDeltaSeconds(), true);
			Cast<ABPP_ProjectHUD>(Cast<APlayerController>(this->GetOwner())->GetHUD())->ShowHUD(); //temporary solution
		}
	}
}

void ABPP_PlayerCharacter_Grunt::AimDownSights()
{
	auto Gun = Cast<ABPP_Gun>(AcctualUsedWeapon);
	if (Gun)
	{
		CameraComponent->SetWorldRotation(GetControlRotation());
		AimCameraComp->FieldOfView = FMath::FInterpTo(AimCameraComp->FieldOfView, 60.f, GetWorld()->GetDeltaSeconds(),25);
		auto NewTransform = UKismetMathLibrary::TInterpTo(AimCameraComp->GetComponentTransform(), Gun->GetSightTransform(), GetWorld()->GetDeltaSeconds()/2,25);
		AimCameraComp->SetWorldTransform(NewTransform);
		
	}
}

void ABPP_PlayerCharacter_Grunt::StopAimDownSights()
{
	if (CameraComponent)
	{	
		CameraComponent->SetWorldRotation(GetControlRotation());
		Mesh1P->AttachToComponent(CameraComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		AimCameraComp->FieldOfView = FMath::FInterpTo(AimCameraComp->FieldOfView, 90.f, GetWorld()->GetDeltaSeconds(), 10);
		auto NewTransform = UKismetMathLibrary::TInterpTo(AimCameraComp->GetComponentTransform(), CameraComponent->GetComponentTransform(), GetWorld()->GetDeltaSeconds()/2, 10);
		AimCameraComp->SetWorldTransform(NewTransform);
		UE_LOG(LogTemp, Warning, TEXT("weeee  %s"), *Cast<ABPP_Gun>(AcctualUsedWeapon)->GetSightTransform().GetLocation().ToString());

		if (UKismetMathLibrary::NearlyEqual_TransformTransform(AimCameraComp->GetComponentTransform(), CameraComponent->GetComponentTransform(), 0.1, 0.1, 0.1))
		{
			GetWorld()->GetTimerManager().ClearTimer(AimTimerHandle);
			AimCameraComp->SetActive(false);
			CameraComponent->SetActive(true);
			
		}
	}
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
