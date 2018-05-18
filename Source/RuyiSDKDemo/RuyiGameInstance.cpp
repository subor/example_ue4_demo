// Fill out your copyright notice in the Description page of Project Settings.

#include "RuyiGameInstance.h"
#include "RuyiSDKManager.h"

void URuyiGameInstance::Shutdown() 
{
	UE_LOG(CommonLog, Log, TEXT("URuyiGameInstance::Shutdown  !!!!!!!!!!!!!!!!!!!!!!"));

	if (FRuyiSDKManager::Instance()->IsSDKReady) 
	{
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Unsubscribe("service/inputmanager");

		FRuyiSDKManager::Instance()->SDK()->RuyiNet->Exit();

		//FRuyiSDKManager::Instance()->ShutDown();

		//FRunnableThread* m_Thread = FRunnableThread::Create(this, TEXT(""), 0, TPri_BelowNormal);;
	}

	Super::Shutdown();
}

uint32 URuyiGameInstance::Run() 
{
	FRuyiSDKManager::Instance()->SDK()->Subscriber->Unsubscribe("service/inputmanager");

	FRuyiSDKManager::Instance()->ShutDown();

	return 0;
}


