// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuyiSDKDemoCharacter.h"
#include "GameFramework/Character.h"
#include "GameCharacter1.generated.h"

/**
 * 
 */
UCLASS()
class RUYISDKDEMO_API AGameCharacter1 : public ARuyiSDKDemoCharacter
{
	GENERATED_BODY()
	
public:
	AGameCharacter1();

	void MoveForward1(float Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
};
