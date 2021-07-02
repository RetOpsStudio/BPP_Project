// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BPP_ProjectCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ABPP_ProjectCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	ABPP_ProjectCharacter();

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* CameraComponent;

protected:
	virtual void BeginPlay();

public:
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() { return CameraComponent; }
};

