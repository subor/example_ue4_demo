// Fill out your copyright notice in the Description page of Project Settings.

#include "RuyiGameInstance.h"
#include "RuyiSDKManager.h"

void URuyiGameInstance::Shutdown() 
{
	UE_LOG(CommonLog, Log, TEXT("URuyiGameInstance::Shutdown  !!!!!!!!!!!!!!!!!!!!!!"));

	if (FRuyiSDKManager::Instance()->IsSDKReady) 
	{
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Unsubscribe("service/inputmanager");

		FRuyiSDKManager::Instance()->ShutDown();
	}

	Super::Shutdown();
}

