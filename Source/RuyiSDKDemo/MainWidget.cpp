// Fill out your copyright notice in the Description page of Project Settings.

#include "MainWidget.h"
#include "DesktopPlatformModule.h"
#include "FileHelper.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "RuyiSDKDemoCharacter.h"


void UMainWidget::InitSDK() 
{
	FRuyiSDKManager::Instance()->MainWidget = this;
	IsRequestFinish = false;
	IsLogin = false;
	SettingData.AudioVolumn = 10;
	SettingData.SpeakerVolumn = 10;
	SettingData.DevModeOn = false;
	SettingData.Language = TEXT("en-US");
	SettingData.Mute = false;
}

#pragma region friend
void UMainWidget::Ruyi_StartLogin(FString username, FString password) 
{
	UE_LOG(CommonLog, Log, TEXT("UMainWidget::Ruyi_StartLogin username:%s password:%s !!!"), *username, *password);

	IsRequestFinish = false;
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestTypeLogin;
	FRuyiSDKManager::Instance()->StartRuyiSDKLogin(username, password, SDKRequestType);
}

void UMainWidget::Ruyi_StartLoginOut() 
{
	IsRequestFinish = false;
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestTypeLoginOut;
	FRuyiSDKManager::Instance()->StartRuyiSDKLoginout(SDKRequestType);
}

void UMainWidget::Ruyi_StartFriendList() 
{
	IsRequestFinish = false;
	Friends.Empty();
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestTypeFriendList;
	FRuyiSDKManager::Instance()->StartRuyiSDKFriendList(SDKRequestType);
}

void UMainWidget::Ruyi_StartAddFriends(TArray<FString> profileIds)
{
	IsRequestFinish = false;
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestTypeAddFriends;
	FRuyiSDKManager::Instance()->StartRuyiSDKAddFriends(profileIds, SDKRequestType);
}

void UMainWidget::Ruyi_StartRemoveFriends(TArray<FString> profileIds) 
{
	IsRequestFinish = false;
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestTypeRemoveFriends;
	FRuyiSDKManager::Instance()->StartRuyiSDKRemoveFriends(profileIds, SDKRequestType);
}

void UMainWidget::Ruyi_StartMatchMakingFindPlayers(int rangeDelta, int numMatches)
{
	IsRequestFinish = false;
	Matches.Empty();
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestTypeMatchMaking;
	FRuyiSDKManager::Instance()->StartRuyiSDKMatchMakingFindPlayers(rangeDelta, numMatches, SDKRequestType);
}

void UMainWidget::Ruyi_StartSave(int score) 
{
	IsRequestFinish = false;
	IsSaveSucceed = false;
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestGameSave;
	FRuyiSDKManager::Instance()->StartRuyiSDKSave(PlayerProfile.profileId, score, SDKRequestType);
}

void UMainWidget::Ruyi_StartLoad() 
{
	IsRequestFinish = false;
	IsLoadCloudSucceed = false;
	IsLoadLocalSucceed = false;
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestGameLoad;
	FRuyiSDKManager::Instance()->StartRuyiSDKLoad(&PlayerProfile, SDKRequestType);
}

void UMainWidget::Ruyi_StartSystemSetting1(FRuyiSystemSettingData settingData)
{
	IsRequestFinish = false;
	SettingData = settingData;
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestSettingSys_SetSetting;
	FRuyiSDKManager::Instance()->StartRuyiSDKSettingSystem(&SettingData, SDKRequestType);
}

void UMainWidget::Ruyi_StartUploadFileToStorageLayer() 
{
	IsRequestFinish = false;
	SDKRequestType = RuyiSDKRequestType::RuyiSDKRequestUploadFileToStorageLayer;
	FRuyiSDKManager::Instance()->StartRuyiSDKUploadFileToStorageLayer(SDKRequestType);
}

#pragma endregion

void UMainWidget::OpenFileDialog(FString& filePath)
{
	FString dialogTitle = TEXT("xxx");
	FString defaultPath = TEXT("");
	FString defaultFile = TEXT("");
	FString fileTypes = TEXT("sss|*.png|aaa|*.jpg|bbb|*.gif|ccc|*.bmp");
	uint32 flags = 0;
	TArray<FString> outFileNames;
	bool result = FDesktopPlatformModule::Get()->OpenFileDialog(nullptr, dialogTitle, defaultPath, defaultFile, fileTypes, flags, outFileNames);

	UE_LOG(CommonLog, Log, TEXT("UMainWidget::OpenFileDialog result:%d"), result);

	for(FString& filename : outFileNames) 
	{
		UE_LOG(CommonLog, Log, TEXT("UMainWidget::OpenFileDialog filename:%s"), *filename);
	}

	if (result) 
	{
		if (outFileNames.Num() > 0) 
		{
			filePath = outFileNames[0];
		}
	} else
	{
		filePath = TEXT("");
	}
}

UTexture2D* UMainWidget::LoadTexture2D_FromFile(const FString& filePath, bool& isValid, int& width, int& height)
{
	UTexture2D* ret = nullptr;

	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	IImageWrapperPtr imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	
	UE_LOG(CommonLog, Log, TEXT("UMainWidget::LoadTexture2D_FromFile branch 000 %s"), *filePath);

	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *filePath)) return ret;

	if (imageWrapper.IsValid() && imageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num())) 
	{
		//UE_LOG(CommonLog, Log, TEXT("UMainWidget::LoadTexture2D_FromFile branch 111"));

		const TArray<uint8>* uncompressedBGRA = nullptr;
		if (imageWrapper->GetRaw(ERGBFormat::BGRA, 8, uncompressedBGRA))
		{
			ret = UTexture2D::CreateTransient(imageWrapper->GetWidth(), imageWrapper->GetHeight(), PF_B8G8R8A8);

			//UE_LOG(CommonLog, Log, TEXT("UMainWidget::LoadTexture2D_FromFile branch 222"));
			if (ret == nullptr) return ret;

			width = imageWrapper->GetWidth();
			height = imageWrapper->GetHeight();

			void* textureData = ret->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(textureData, uncompressedBGRA->GetData(), uncompressedBGRA->Num());
			ret->PlatformData->Mips[0].BulkData.Unlock();

			//UE_LOG(CommonLog, Log, TEXT("UMainWidget::LoadTexture2D_FromFile branch 333"));
			ret->UpdateResource();
		}
	}
	//UE_LOG(CommonLog, Log, TEXT("UMainWidget::LoadTexture2D_FromFile branch 444"));
	isValid = true;

	return ret;
}

void UMainWidget::ReadSaveFile(FString localPath) 
{
	FString replaceFrom("\\"), replaceTo("/");
	FString path = localPath.Replace(*replaceFrom, *replaceTo);
	UE_LOG(CommonLog, Log, TEXT("UMainWidget::ReadSaveFile localPath:%s path:%s"), *localPath, *path);

	FArchive* readFile = IFileManager::Get().CreateFileReader(*path);
	FString jsonStr;
	
	if (nullptr != readFile) 
	{
		*readFile << jsonStr;
		readFile->Close();
		delete readFile;
		readFile = nullptr;

		UE_LOG(CommonLog, Log, TEXT("UMainWidget::ReadSaveFile jsonStr:%s"), *jsonStr);

		TSharedPtr<FJsonObject> jsonParsed;
		TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(jsonStr);

		if (FJsonSerializer::Deserialize(jsonReader, jsonParsed))
		{
			TSharedPtr<FJsonObject> savingObject = jsonParsed->GetObjectField(TEXT("Saving"));

			if (savingObject.IsValid())
			{
				FString playerId = savingObject->GetStringField(TEXT("playerId:"));
				uint32 score = savingObject->GetIntegerField(TEXT("Score:"));

				if (0 == PlayerProfile.profileId.Compare(playerId))
				{
					Score1P = score;
					IsLoadCloudSucceed = true;
				}
			}
		}
	} else
	{
		FString path = FPaths::ConvertRelativePathToFull(FPaths::GameSavedDir());
		UE_LOG(CommonLog, Log, TEXT("UMainWidget::Ruyi_StartSave path:%s !!!"), *path);
		FString fileName = TEXT("Saving1.sav");
		path += fileName;

		FArchive* readFile = IFileManager::Get().CreateFileReader(*path);
		FString jsonStr;

		if (readFile != nullptr) 
		{
			*readFile << jsonStr;
			readFile->Close();
			delete readFile;
			readFile = nullptr;

			UE_LOG(CommonLog, Log, TEXT("UMainWidget::ReadSaveFile jsonStr:%s"), *jsonStr);

			TSharedPtr<FJsonObject> jsonParsed;
			TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(jsonStr);

			if (FJsonSerializer::Deserialize(jsonReader, jsonParsed))
			{
				TSharedPtr<FJsonObject> savingObject = jsonParsed->GetObjectField(TEXT("Saving"));

				if (savingObject.IsValid())
				{
					FString playerId = savingObject->GetStringField(TEXT("playerId:"));
					uint32 score = savingObject->GetIntegerField(TEXT("Score:"));

					if (0 == PlayerProfile.profileId.Compare(playerId))
					{
						Score1P = score;
						IsLoadLocalSucceed = true;
					}
				}
			}
		}
	}
	
}


