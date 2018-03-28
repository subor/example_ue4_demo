#include "RuyiSDKManager.h"
#include "MainWidget.h"
#include "RuyiSDKDemoCharacter.h"
#include "Kismet/GameplayStatics.h"
//#include "boost/container/detail/json.hpp"


#pragma comment(lib,"urlmon.lib") 

FRuyiSDKManager* FRuyiSDKManager::Instance() 
{
	static FRuyiSDKManager* m_Instance = new FRuyiSDKManager();
	return m_Instance;
}

Ruyi::RuyiSDK* FRuyiSDKManager::SDK() 
{
	return m_RuyiSDK;
}

FRuyiSDKManager::FRuyiSDKManager() 
{
	m_RuyiSDK = nullptr;
	m_ThreadEnd = false;
	m_ThreadBegin = false;
	m_Thread = nullptr;
	
	InitRuyiSDK();

	m_SaveCloudFileName = TEXT("unrealRuyiSDKDemo1.sav");
}

void FRuyiSDKManager::InitRuyiSDK() 
{
	UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::InitRuyiSDK  !!!!!!!!!!!!!!!!!!!!!!"));
	try
	{
		auto context = new Ruyi::RuyiSDKContext(Ruyi::RuyiSDKContext::Endpoint::PC, "localhost");
		m_RuyiSDK = Ruyi::RuyiSDK::CreateSDKInstance(*context);
		
		//remember, if you initialize ruyinet, the user should already logined in main client, or you won't 
		//able be get their logon profile. The RuyiSDK doesn't support login in game. The user should always
		//login through main client. 
		m_RuyiSDK->RuyiNet->Initialise(APPID, GAMEID);
		CurPlayerProfile = m_RuyiSDK->RuyiNet->GetPlayer(0);

		if (nullptr == CurPlayerProfile)
		{
			UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::InitRuyiSDK Not login !!!"));
		} else 
		{
			FString profileID = UTF8_TO_TCHAR(CurPlayerProfile->profileId.c_str());
			UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::InitRuyiSDK profileID:%s"), *profileID);
		}

		IsSDKReady = true;
		
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::InitRuyiSDK Success !!!"));
	}
	catch (exception e)
	{
		m_RuyiSDK = nullptr;
		IsSDKReady = false;
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::InitRuyiSDK Fail !!!"));
	}
}

#pragma region ruyi sdk request for public call
void FRuyiSDKManager::StartRuyiSDKRegister(FString& username, FString& password, RuyiSDKRequestType requestType)
{
	m_Username = username;
	m_Password = password;
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKLogin(FString& username, FString& password, RuyiSDKRequestType requestType)
{
	m_Username = username;
	m_Password = password;
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKLoginout(RuyiSDKRequestType requestType)
{
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKFriendList(RuyiSDKRequestType requestType) 
{
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKAddFriends(TArray<FString>& profileIds, RuyiSDKRequestType requestType)
{
	m_RuyiSDKRequestType = requestType;
	m_AddOrRemoveFriendIds.Empty();
	m_AddOrRemoveFriendIds.Append(profileIds);
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKRemoveFriends(TArray<FString>& profileIds, RuyiSDKRequestType requestType)
{
	m_RuyiSDKRequestType = requestType;
	m_AddOrRemoveFriendIds.Empty();
	m_AddOrRemoveFriendIds.Append(profileIds);
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKMatchMakingFindPlayers(int rangeDelta, int numMatches, RuyiSDKRequestType requestType)
{
	m_MatchesRangeDelta = rangeDelta;
	m_NumMatches = numMatches;
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKSave(FString id, int Score, RuyiSDKRequestType requestType)
{
	m_PlayerId = id;
	m_Score = Score;
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKLoad(FRuyiNetProfile* profile, RuyiSDKRequestType requestType)
{
	m_Profile = profile;
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKSettingSystem(FRuyiSystemSettingData* settingData, RuyiSDKRequestType requestType)
{
	m_SettingData = settingData;
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKUploadFileToStorageLayer(RuyiSDKRequestType requestType)
{
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKLeaderboard(RuyiSDKRequestType requestType)
{
	m_RuyiSDKRequestType = requestType;
	StartThread();
}

void FRuyiSDKManager::StartRuyiSDKGetProfile(RuyiSDKRequestType requestType)
{
	m_RuyiSDKRequestType = requestType;
	StartThread();
}
#pragma endregion

#pragma region ruyi sdk async request

void FRuyiSDKManager::Ruyi_AsyncSDKRegister(FString& username, FString& password)
{
	/*
	string userNameString(TCHAR_TO_UTF8(*username));
	string passwordString(TCHAR_TO_UTF8(*password));

	try
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKRegister username:%s password:%s !!!"), *username, *password);

		string ret;
		m_RuyiSDK->BCService->Authentication_AuthenticateEmailPassword(ret, userNameString, passwordString, true, 0);

		FString fRet = UTF8_TO_TCHAR(ret.c_str());

		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKRegister ret:%s !!!"), *fRet);

		TSharedPtr<FJsonObject> jsonParsed;
		TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(fRet);

		if (FJsonSerializer::Deserialize(jsonReader, jsonParsed))
		{
			int status = jsonParsed->GetIntegerField(TEXT("status"));

			if (JSON_RESPONSE_OK == status)
			{
				UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKRegister Login Success !!!"));

				m_mutex.Lock();

				MainWidget->IsLogin = true;

				TSharedPtr<FJsonObject> dataJsonObject = jsonParsed->GetObjectField("data");

				FString playerName("");
				FString emailAddress("");
				FString id("");

				if (dataJsonObject->TryGetStringField(TEXT("playerName"), playerName))
				{
					MainWidget->PlayerProfile.profileName = playerName;
				}
				if (dataJsonObject->TryGetStringField(TEXT("emailAddress"), emailAddress))
				{
					MainWidget->PlayerProfile.email = emailAddress;
				}
				if (dataJsonObject->TryGetStringField(TEXT("id"), id))
				{
					MainWidget->PlayerProfile.profileId = id;
				}

				FString pictureUrl("");
				if (dataJsonObject->TryGetStringField("pictureUrl", pictureUrl))
				{
					MainWidget->PlayerProfile.pictureUrl = pictureUrl;
				}
				else
				{
					MainWidget->PlayerProfile.pictureUrl = TEXT("");
				}

				m_mutex.Unlock();
			}
		}
	}
	catch (exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKRegister Authentication_AuthenticateEmailPassword exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();*/
}

void FRuyiSDKManager::Ruyi_AsyncSDKLogin(FString& username, FString& password)
{
	if (nullptr != CurPlayerProfile)
	{
		m_mutex.Lock();

		MainWidget->IsLogin = true;

		MainWidget->PlayerProfile.profileName = UTF8_TO_TCHAR(CurPlayerProfile->profileName.c_str());
		MainWidget->PlayerProfile.email = UTF8_TO_TCHAR(CurPlayerProfile->email.c_str());
		MainWidget->PlayerProfile.profileId = UTF8_TO_TCHAR(CurPlayerProfile->profileId.c_str());

		m_mutex.Unlock();
	}

	/*
	try
	{
		m_RuyiSDK->BCService->Authentication_ClearSavedProfileID(0);
	}
	catch (exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKLogin Authentication_ClearSavedProfileID exception !!!"));
	}

	string userNameString(TCHAR_TO_UTF8(*username));
	string passwordString(TCHAR_TO_UTF8(*password));

	try
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKLogin username:%s password:%s !!!"), *username, *password);

		string ret;
		m_RuyiSDK->BCService->Authentication_AuthenticateEmailPassword(ret, userNameString, passwordString, true, 0);
		
		FString fRet = UTF8_TO_TCHAR(ret.c_str());

		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKLogin ret:%s !!!"), *fRet);

		TSharedPtr<FJsonObject> jsonParsed;
		TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(fRet);

		if (FJsonSerializer::Deserialize(jsonReader, jsonParsed))
		{
			int status = jsonParsed->GetIntegerField(TEXT("status"));

			if (JSON_RESPONSE_OK == status)
			{
				UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKLoginr Login Success !!!"));
				
				m_mutex.Lock();
				
				MainWidget->IsLogin = true;

				TSharedPtr<FJsonObject> dataJsonObject = jsonParsed->GetObjectField("data");

				FString playerName("");
				FString emailAddress("");
				FString id("");

				if (dataJsonObject->TryGetStringField(TEXT("playerName"), playerName)) 
				{
					MainWidget->PlayerProfile.profileName = playerName;
				}
				if (dataJsonObject->TryGetStringField(TEXT("emailAddress"), emailAddress)) 
				{
					MainWidget->PlayerProfile.email = emailAddress;
				}
				if (dataJsonObject->TryGetStringField(TEXT("id"), id)) 
				{
					MainWidget->PlayerProfile.profileId = id;
				}
				
				FString pictureUrl("");
				if (dataJsonObject->TryGetStringField("pictureUrl", pictureUrl)) 
				{
					MainWidget->PlayerProfile.pictureUrl = pictureUrl;
				} else 
				{
					MainWidget->PlayerProfile.pictureUrl = TEXT("");
				}
				
				m_mutex.Unlock();
			}
		}
	}
	catch (exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncLoginTestUser Authentication_AuthenticateEmailPassword exception !!!"));
	}*/

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKLoginOut() 
{
	/*
	try
	{
		m_RuyiSDK->BCService->Authentication_ClearSavedProfileID(0);

		MainWidget->IsLogin = false;
		MainWidget->IsRequestFinish = true;

		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKLoginOut Success !!!"));
	}
	catch (exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKLoginOut Authentication_ClearSavedProfileID exception !!!"));
	}
	*/
	MainWidget->IsRequestFinish = true;

	EndThread();
}


void FRuyiSDKManager::Ruyi_AsyncSDKFriendList() 
{
	try
	{
		Ruyi::RuyiNetFriendListResponse response;
		m_RuyiSDK->RuyiNet->GetFriendService()->ListFriends(0, response);
		
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKFriendList reponse.status:%d !!!"), response.status);

		ParseFriendListData(response);
	}
	catch (exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKFriendList Request exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKAddFriends(TArray<FString>& friendIds)
{
	try 
	{
		for (FString& id : friendIds)
		{
			string idStr(TCHAR_TO_UTF8(*id));

			//UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKAddFriends add friend id:%s"), *id);
			Ruyi::RuyiNetAddRemoveFriendResponse response;
			m_RuyiSDK->RuyiNet->GetFriendService()->AddFriend(0, idStr, response);
		}
		
		Ruyi::RuyiNetFriendListResponse response;
		m_RuyiSDK->RuyiNet->GetFriendService()->ListFriends(0, response);

		ParseFriendListData(response);
	}catch(exception e) 
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKAddFriends Friend_AddFriends exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKRemoveFriends(TArray<FString>& friendIds)
{
	try
	{
		for (FString& id : friendIds)
		{
			string idStr(TCHAR_TO_UTF8(*id));

			UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKRemoveFriends add friend id:%s"), *id);
			
			Ruyi::RuyiNetAddRemoveFriendResponse response;
			m_RuyiSDK->RuyiNet->GetFriendService()->RemoveFriend(0, idStr, response);
		}

		
		Ruyi::RuyiNetFriendListResponse response;
		m_RuyiSDK->RuyiNet->GetFriendService()->ListFriends(0, response);

		ParseFriendListData(response);
	}
	catch (exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKRemoveFriends Friend_AddFriends exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKMatchMakingFindPlayers(int rangeDelta, int numMatches)
{
	try
	{
		Ruyi::RuyiNetFindPlayersResponse response;
		m_RuyiSDK->RuyiNet->GetMatchmakingService()->FindPlayers(0, rangeDelta, numMatches, response);
		
		if (200 == response.status) 
		{
			m_mutex.Lock();
			MainWidget->Matches.Empty();
			m_mutex.Unlock();

			std::list<Ruyi::RuyiNetFindPlayersResponse::Data::MatchesFound>::iterator it;
			for (it = response.data.matchesFound.begin(); it != response.data.matchesFound.end(); ++it)
			{
				FRuyiNetProfile playerProfile;
				
				playerProfile.profileId = FString(it->playerId.c_str());
				playerProfile.profileName = FString(it->playerName.c_str());
				playerProfile.pictureUrl = FString(it->pictureUrl.c_str());

				m_mutex.Lock();
				MainWidget->Matches.Add(playerProfile);
				m_mutex.Unlock();
			}
		}
		
	}
	catch (exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("ARuyiSDKSampleCharacter::MatchMaking_FindPlayers exception  !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKSave(FString playerId, int score) 
{
	FString jsonStr;
	TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<>::Create(&jsonStr);

	jsonWriter->WriteObjectStart();

	jsonWriter->WriteObjectStart("Saving");
	jsonWriter->WriteValue("playerId:", playerId);
	jsonWriter->WriteValue("Score:", score);
	jsonWriter->WriteObjectEnd();

	jsonWriter->WriteObjectEnd();

	jsonWriter->Close();

	FString path = FPaths::ConvertRelativePathToFull(FPaths::GameSavedDir());
	UE_LOG(CommonLog, Log, TEXT("UMainWidget::Ruyi_StartSave path:%s !!!"), *path);
	FString fileName = TEXT("Saving1.sav");
	path += fileName;

	//FArchive* saveFile = GFileManager->CreateFileWriter(*path);
	FArchive* saveFile = IFileManager::Get().CreateFileWriter(*path);
	if (!saveFile) return;

	*saveFile << jsonStr;
	saveFile->Close();
	delete saveFile;
	saveFile = nullptr;

	try 
	{
		string cloudPath = "test/files";
		string localPath = TCHAR_TO_UTF8(*path);
		string cloudFileName = TCHAR_TO_UTF8(*m_SaveCloudFileName);
		
		Ruyi::RuyiNetUploadFileResponse response;
		m_RuyiSDK->RuyiNet->GetUserFileService()->UploadFile(0, cloudPath, cloudFileName, true, true, localPath, response);
		
		if (200 == response.status)
		{
			m_mutex.Lock();
			MainWidget->IsSaveSucceed = true;
			m_mutex.Unlock();
		}
	}catch(exception e) 
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKSave exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKLoad(FRuyiNetProfile* profile)
{
	try
	{
		string cloudPath = "test/files";
		string ret;
		string cloudFileName = TCHAR_TO_UTF8(*m_SaveCloudFileName);

		Ruyi::RuyiNetResponse response;
		m_RuyiSDK->RuyiNet->GetUserFileService()->DownloadFile(0, cloudPath, cloudFileName, response);
	}catch(exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKLoad File_DownloadFile exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKSettingSys_SimulateLoginAndChangeSettings()
{
	try 
	{
		/*
		std::vector<Ruyi::SDK::SettingSystem::Api::SettingItem> vecSetting;
		m_RuyiSDK->SettingSys->GetSettingItems(vecSetting, "ActionMapping", true);

		for (std::vector<Ruyi::SDK::SettingSystem::Api::SettingItem>::iterator it = vecSetting.begin(); it != vecSetting.end(); ++it) 
		{
			
		}*/
	}catch(exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKSettingSys_SimulateLoginAndChangeSettings exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKSettingSystem(FRuyiSystemSettingData* settingData)
{
	std::map<string, string> kvs;
	kvs["AudioVolumn"] = to_string(settingData->AudioVolumn);
	kvs["SpeakerVolume"] = to_string(settingData->SpeakerVolumn);
	if (settingData->Mute) 
	{
		kvs["Mute"] = "true";
	} else 
	{
		kvs["Mute"] = "false";
	}

	try 
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKSettingSys_SetVolume AV:%d SV:%d"), settingData->AudioVolumn, settingData->SpeakerVolumn);

		int count = m_RuyiSDK->SettingSys->SetSettingItems(kvs);
		
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKSettingSys_SetVolume count:%d"), count);
	}catch (exception e) 
	{
		UE_LOG(CommonLog, Log, TEXT("exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKUploadFileToStorageLayer()
{
	try 
	{
		Ruyi::SDK::StorageLayer::GetLocalPathResult result;
		m_RuyiSDK->Storage->GetLocalPath(result, "/<HDD0>/godenTest.json");

		FString fPath = UTF8_TO_TCHAR(result.path.c_str());

		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::Ruyi_AsyncSDKUploadFileToStorageLayer result:%d path:%s"), result.result, *fPath);

		FString jsonStr;
		TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<>::Create(&jsonStr);

		jsonWriter->WriteObjectStart();

		jsonWriter->WriteObjectStart("Test");
		jsonWriter->WriteValue("AAA:", 0);
		jsonWriter->WriteObjectEnd();

		jsonWriter->WriteObjectEnd();

		jsonWriter->Close();

		FArchive* saveFile = IFileManager::Get().CreateFileWriter(*fPath);
		if (!saveFile) return;

		*saveFile << jsonStr;
		saveFile->Close();
		delete saveFile;
		saveFile = nullptr;

	} catch (exception e) 
	{
		UE_LOG(CommonLog, Log, TEXT("exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AyncSDKLeaderboard(int startIndex, int endIndex)
{
	try 
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::StartRuyiSDKLeaderboard !!!"));
		
		if (nullptr == CurPlayerProfile) return;

		Ruyi::RuyiNetLeaderboardResponse response;
		m_RuyiSDK->RuyiNet->GetLeaderboardService()->GetGlobalLeaderboardPage(0, CurPlayerProfile->profileId, Ruyi::SDK::BrainCloudApi::SortOrder::type::HIGH_TO_LOW, startIndex, endIndex, response);
		
	} catch (exception e) 
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::StartRuyiSDKLeaderboard excpetion !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

void FRuyiSDKManager::Ruyi_AsyncSDKGetProfile(std::string profileId)
{
	try 
	{
		Ruyi::RuyiNetGetProfileResponse response;
		m_RuyiSDK->RuyiNet->GetFriendService()->GetProfile(0, profileId, response);
		
	} catch(std::exception e)
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager Ruyi_AsyncSDKGetProfile exception !!!"));
	}

	MainWidget->IsRequestFinish = true;

	EndThread();
}

#pragma endregion

#pragma region data handle

void FRuyiSDKManager::ParseFriendListData(Ruyi::RuyiNetFriendListResponse& response)
{
	if (response.status == 200)
	{
		m_mutex.Lock();
		MainWidget->Friends.Empty();

		int friendNum = response.data.response.friends.size();
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::ParseFriendListData friendNum:%d !!!"), friendNum);

		std::vector<Ruyi::RuyiNetFriendListResponse::Data::Response::Friend>::iterator it;
		for (it = response.data.response.friends.begin(); it != response.data.response.friends.end(); ++it)
		{
			FString fplayerId = FString(it->playerId.c_str());

			//UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::ParseFriendListData playerID:%s !!!"), *fplayerId);

			FRuyiNetProfile friendProfile;
			friendProfile.profileId = FString(it->playerId.c_str());
			friendProfile.profileName = FString(it->name.c_str());
			friendProfile.pictureUrl = FString(it->pictureUrl.c_str());
			friendProfile.summaryFriendData.dob = FString(it->summaryFriendData.dob.c_str());
			friendProfile.summaryFriendData.gender = FString(it->summaryFriendData.gender.c_str());
			friendProfile.summaryFriendData.location = FString(it->summaryFriendData.location.c_str());

			MainWidget->Friends.Add(friendProfile);
		}

		m_mutex.Unlock();
	}
}

void FRuyiSDKManager::ReadSaveFile(FString& localPath) 
{
	FArchive* readFile = IFileManager::Get().CreateFileReader(*localPath);

	FString jsonStr;
	*readFile << jsonStr;
	readFile->Close();
	delete readFile;
	readFile = nullptr;

	UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::ReadSaveFile jsonStr:%s"), *jsonStr);

	TSharedPtr<FJsonObject> jsonParsed;
	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(jsonStr);

	if (FJsonSerializer::Deserialize(jsonReader, jsonParsed)) 
	{
		TSharedPtr<FJsonObject> savingObject = jsonParsed->GetObjectField(TEXT("Saving"));

		if (savingObject.IsValid()) 
		{
			FString playerId = savingObject->GetStringField(TEXT("playerId:"));
			uint32 score = savingObject->GetIntegerField(TEXT("Score:"));
			
			if (0 == MainWidget->PlayerProfile.profileId.Compare(playerId)) 
			{			
				m_mutex.Lock();
				MainWidget->Score1P = score;
				MainWidget->IsLoadCloudSucceed = true;
				m_mutex.Unlock();	
			}
		}
	}
}

#pragma endregion

#pragma region multi-thread
bool FRuyiSDKManager::Init()
{
	m_ThreadEnd = false;
	m_ThreadBegin = false;
	m_Thread = nullptr;

	return true;
}

uint32 FRuyiSDKManager::Run()
{
	while (!m_ThreadEnd)
	{
		if (!m_ThreadBegin)
		{
			m_ThreadBegin = true;

			switch(m_RuyiSDKRequestType)
			{
			case RuyiSDKRequestType::RuyiSDKRequestTypeRegister:
				//Ruyi_AsyncSDKRegister(m_Username, m_Password);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestTypeLogin:
				Ruyi_AsyncSDKLogin(m_Username, m_Password);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestTypeLoginOut:
				//Ruyi_AsyncSDKLoginOut();
				break;
			case RuyiSDKRequestType::RuyiSDKRequestTypeFriendList:
				Ruyi_AsyncSDKFriendList();
				break;
			case RuyiSDKRequestType::RuyiSDKRequestTypeAddFriends:
				Ruyi_AsyncSDKAddFriends(m_AddOrRemoveFriendIds);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestTypeRemoveFriends:
				Ruyi_AsyncSDKRemoveFriends(m_AddOrRemoveFriendIds);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestTypeMatchMaking:
				Ruyi_AsyncSDKMatchMakingFindPlayers(m_MatchesRangeDelta, m_NumMatches);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestGameSave:
				Ruyi_AsyncSDKSave(m_PlayerId, m_Score);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestGameLoad:
				Ruyi_AsyncSDKLoad(m_Profile);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestSettingSys_SetSetting:
				Ruyi_AsyncSDKSettingSystem(m_SettingData);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestUploadFileToStorageLayer:
				Ruyi_AsyncSDKUploadFileToStorageLayer();
				break;
			case RuyiSDKRequestType::RuyiSDKRequestGetLeaderboard:
				Ruyi_AyncSDKLeaderboard(0, 10);
				break;
			case RuyiSDKRequestType::RuyiSDKRequestGetProfile:
				Ruyi_AsyncSDKGetProfile(CurPlayerProfile->profileId);
				break;
			default:
				break;
			}
		}
	}

	return 0;
}

void FRuyiSDKManager::StartThread()
{
	m_ThreadBegin = false;
	m_ThreadEnd = false;
	m_Thread = FRunnableThread::Create(this, TEXT(""), 0, TPri_BelowNormal);
}

void FRuyiSDKManager::EndThread()
{
	m_ThreadEnd = true;

	if (nullptr != m_Thread)
	{
		m_Thread->Kill();
		delete m_Thread;
		m_Thread = nullptr;
	}
}

#pragma endregion
