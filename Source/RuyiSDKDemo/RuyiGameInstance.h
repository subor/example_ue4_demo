// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RuyiSDKDemo.h"

#include "CoreMinimal.h"
#include "HAL/ThreadingBase.h"

#include "Engine/GameInstance.h"
#include "RuyiGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RUYISDKDEMO_API URuyiGameInstance : public UGameInstance, public FRunnable
{
	GENERATED_BODY()
		
public:
	virtual void Shutdown() override;

	virtual uint32 Run() override;

};
