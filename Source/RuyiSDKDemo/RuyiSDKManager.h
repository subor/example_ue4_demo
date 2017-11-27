#pragma once

#include "MainWidget.h"
#include "RuyiSDKDemo.h"
#include "RuyiSDKDemoGameMode.h"
//#include "RuyiSDKDemoCharacter.h"

#include "AllowWindowsPlatformTypes.h"
#include "Windows/WindowsPlatformProcess.h"
#include "HideWindowsPlatformTypes.h"
#include "HAL/ThreadingBase.h"

#include "CoreMinimal.h"

#include "RuyiSDK.h"

class FRuyiSDKManager : public FRunnable
{
public:
	static FRuyiSDKManager* Instance();
	Ruyi::RuyiSDK* SDK();

	class UMainWidget* MainWidget;

public:
	void StartRuyiSDKLogin(FString& username, FString& password, RuyiSDKRequestType requestType);
	void StartRuyiSDKLoginout(RuyiSDKRequestType requestType);
	void StartRuyiSDKFriendList(RuyiSDKRequestType requestType);
	void StartRuyiSDKMatchMakingFindPlayers(int rangeDelta, int numMatches, RuyiSDKRequestType requestType);
	void StartRuyiSDKAddFriends(TArray<FString>& profileIds, RuyiSDKRequestType requestType);
	void StartRuyiSDKRemoveFriends(TArray<FString>& profileIds, RuyiSDKRequestType requestType);

	RuyiSDKRequestType m_RuyiSDKRequestType;

	//multi-thread
	virtual bool Init() override;
	virtual uint32 Run() override;
	

private:
	FRuyiSDKManager();
	void InitRuyiSDK();

	Ruyi::RuyiSDK* m_RuyiSDK;

	//ruyi sdk data related
	FString m_Username;
	FString m_Password;

	int m_MatchesRangeDelta;
	int m_NumMatches;

	TArray<FString> m_AddOrRemoveFriendIds;

	void ParseFriendListData(FString& jsonData);

	//async request
	void Ruyi_AsyncSDKLogin(FString& username, FString& password);
	void Ruyi_AsyncSDKLoginOut();
	void Ruyi_AsyncSDKFriendList();
	void Ruyi_AsyncSDKMatchMakingFindPlayers(int rangeDelta, int numMatches);
	void Ruyi_AsyncSDKAddFriends(TArray<FString>& friendIds);
	void Ruyi_AsyncSDKRemoveFriends(TArray<FString>& friendsIds);

	//multi-thread
	void StartThread();
	void EndThread();
	FRunnableThread* m_Thread;
	FCriticalSection m_mutex;
	bool m_ThreadEnd;
	bool m_ThreadBegin;
};

