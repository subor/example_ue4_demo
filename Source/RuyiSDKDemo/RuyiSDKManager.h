#pragma once

#include "MainWidget.h"
#include "RuyiSDKDemo.h"
#include "RuyiSDKDemoGameMode.h"

#include "AllowWindowsPlatformTypes.h"
#include "Windows/WindowsPlatformProcess.h"
#include "HideWindowsPlatformTypes.h"
#include "HAL/ThreadingBase.h"

#include "CoreMinimal.h"

#include "RuyiSDK.h"

#include "RuyiNet/Response/RuyiNetFriendResponse.h"
#include "RuyiNet/Response/RuyiNetProfile.h"
#include "RuyiNet/Response/RuyiNetLeaderboardResponse.h"
#include "RuyiNet/Response/RuyiNetGetProfileResponse.h"
#include "RuyiNet/Response/RuyiNetFindPlayersResponse.h"
#include "RuyiNet/Response/RuyiNetUploadFileResponse.h"
#include "RuyiNet/Response/RuyiNetTelemetrySessionResponse.h"

/*
#include "RuyiNet/Service/Friend/RuyiNetFriendService.h"
#include "RuyiNet/Service/Leaderboard/RuyiNetLeaderboardService.h"
//#include "RuyiNet/Service/RuyiNetMatchmakingService.h"
#include "RuyiNet/Service/UserFile/RuyiNetUserFileService.h"
#include "RuyiNet/Service/Telemetry/RuyiNetTelemetryService.h"
*/

#include "RuyiNet/Service/RuyiNetFriendService.h"
#include "RuyiNet/Service/RuyiNetLeaderboardService.h"
#include "RuyiNet/Service/RuyiNetMatchmakingService.h"
#include "RuyiNet/Service/RuyiNetUserFileService.h"
#include "RuyiNet/Service/RuyiNetTelemetryService.h"


//tempory
#define APPID "11499"
#define GAMEID "Shooter"

//using namespace Ruyi::SDK::Online;
using namespace Ruyi;

/// <summary>
/// RuyiSDk provide most function include console hardware-related function, user-related data transport.
/// 
/// </summary>

class FRuyiSDKManager : public FRunnable
{
public:
	static FRuyiSDKManager* Instance();
	Ruyi::RuyiSDK* SDK();
	
	void InitRuyiSDK();

	//destroy the SDK instance, call this when your game is over
	void ShutDown();
	class UMainWidget* MainWidget;

public:
	//users should always login/register in main client.
	//RuyiNet won't provide these function. You can still use them by RuyiSDK->BCService but we don't 
	//recommend that and we'll remove them in later version
	void StartRuyiSDKRegister(FString& username, FString& password, RuyiSDKRequestType requestType);
	void StartRuyiSDKLogin(FString& username, FString& password, RuyiSDKRequestType requestType);
	void StartRuyiSDKLoginout(RuyiSDKRequestType requestType);

	void StartRuyiSDKFriendList(RuyiSDKRequestType requestType);
	void StartRuyiSDKMatchMakingFindPlayers(int rangeDelta, int numMatches, RuyiSDKRequestType requestType);
	void StartRuyiSDKAddFriends(TArray<FString>& profileIds, RuyiSDKRequestType requestType);
	void StartRuyiSDKRemoveFriends(TArray<FString>& profileIds, RuyiSDKRequestType requestType);
	void StartRuyiSDKSave(FString id, int Score, RuyiSDKRequestType requestType);
	void StartRuyiSDKLoad(struct FRuyiNetProfile* profile, RuyiSDKRequestType requestType);
	void StartRuyiSDKSettingSystem(struct FRuyiSystemSettingData* settingData, RuyiSDKRequestType requestType);
	void StartRuyiSDKUploadFileToStorageLayer(RuyiSDKRequestType requestType);
	void StartRuyiSDKLeaderboard(RuyiSDKRequestType requestType);
	//when user logged in main client, you can get his profile 
	void StartRuyiSDKGetProfile(RuyiSDKRequestType requestType);

	RuyiSDKRequestType m_RuyiSDKRequestType;

	bool IsSDKReady;

	//multi-thread
	virtual bool Init() override;
	virtual uint32 Run() override;
	

private:
	FRuyiSDKManager();

	Ruyi::RuyiSDK* m_RuyiSDK;

	//ruyi sdk data related
	const RuyiNetProfile* CurPlayerProfile;

	FString m_Username;
	FString m_Password;

	int m_MatchesRangeDelta;
	int m_NumMatches;

	TArray<FString> m_AddOrRemoveFriendIds;

	FString m_PlayerId;
	int m_Score;
	FRuyiNetProfile* m_Profile;

	void ParseFriendListData(RuyiNetFriendListResponse& response);

	FString m_SaveCloudFileName;
	void ReadSaveFile(FString& localPath);

	FRuyiSystemSettingData* m_SettingData;

	string m_TelemetrySessionId;

	//async request
	void Ruyi_AsyncSDKRegister(FString& username, FString& password);
	void Ruyi_AsyncSDKLogin(FString& username, FString& password);
	void Ruyi_AsyncSDKLoginOut();
	
	void Ruyi_AsyncSDKFriendList();
	void Ruyi_AsyncSDKMatchMakingFindPlayers(int rangeDelta, int numMatches);
	void Ruyi_AsyncSDKAddFriends(TArray<FString>& friendIds);
	void Ruyi_AsyncSDKRemoveFriends(TArray<FString>& friendsIds);
	void Ruyi_AsyncSDKSave(FString playerId, int score);
	void Ruyi_AsyncSDKLoad(FRuyiNetProfile* profile);
	void Ruyi_AsyncSDKSettingSys_SimulateLoginAndChangeSettings();
	void Ruyi_AsyncSDKSettingSystem(FRuyiSystemSettingData* settingData);
	void Ruyi_AsyncSDKUploadFileToStorageLayer();

	void Ruyi_AyncSDKLeaderboard(int startIndex, int endIndex);
	void Ruyi_AsyncSDKGetProfile(std::string profileId);

	void Ruyi_AsyncSDKTelemetryStart();
	void Ruyi_AsyncSDKTelemetryEnd();

	void Ruyi_AsyncSDKDestroy();

	//multi-thread
	void StartThread();
	void EndThread();
	FRunnableThread* m_Thread;
	FCriticalSection m_mutex;
	bool m_ThreadEnd;
	bool m_ThreadBegin;
};

