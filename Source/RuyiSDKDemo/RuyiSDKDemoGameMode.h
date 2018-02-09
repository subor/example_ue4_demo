// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RuyiSDKDemoGameMode.generated.h"

UENUM(BlueprintType)
enum class RuyiSDKRequestType : uint8
{
	RuyiSDKRequestTypeNone = 0,
	RuyiSDKRequestTypeRegister,
	RuyiSDKRequestTypeLogin,
	RuyiSDKRequestTypeLoginOut,
	RuyiSDKRequestTypeFriendList,
	RuyiSDKRequestTypeAddFriends,
	RuyiSDKRequestTypeRemoveFriends,
	RuyiSDKRequestTypeMatchMaking,
	RuyiSDKRequestGameSave,
	RuyiSDKRequestGameLoad,
	RuyiSDKRequestSettingSys_SimulateLoginAndChangeSettings,
	RuyiSDKRequestSettingSys_SetSetting,
	RuyiSDKRequestUploadFileToStorageLayer,
	RuyiSDKRequestGetLeaderboard,
	RuyiSDKRequestGetProfile,
};


UCLASS(minimalapi)
class ARuyiSDKDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARuyiSDKDemoGameMode();

};



