// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RuyiSDKDemoCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "RuyiSDKManager.h"
//#include "curl/curl.h"
//#include "ThirdParty/OpenSSL/1.0.2g/include/Win64/VS2015/openssl/ssl.h"

//////////////////////////////////////////////////////////////////////////
// ARuyiSDKDemoCharacter

ARuyiSDKDemoCharacter::ARuyiSDKDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	IsPaused = false;

	isJump = false;
}

std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value);

void ARuyiSDKDemoCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(CommonLog, Log, TEXT("ARuyiSDKDemoCharacter::BeginPlay()"));

	FRuyiSDKManager::Instance()->InitRuyiSDK();
	
	const string* pChar = &Ruyi::SDK::Constants::g_ConstantsSDKDataTypes_constants.layer0_publisher_out_uri;
	string* modifier = const_cast<string*>(pChar);
	replace_all(*modifier, "{addr}", "localhost");

	if (FRuyiSDKManager::Instance()->IsSDKReady)
	{
		//FRuyiSDKManager::Instance()->SDK()->Subscriber->Subscribe("service/user_service_external");
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Subscribe("service/inputmgr_int");
		//FRuyiSDKManager::Instance()->SDK()->Subscriber->AddMessageHandler(this, &ARuyiSDKDemoCharacter::InputStateChangeHandler);
		FRuyiSDKManager::Instance()->SDK()->Subscriber->AddMessageHandler(this, &ARuyiSDKDemoCharacter::InputStateChangeHandler2);
	}

	PrimaryActorTick.bCanEverTick = true;
}

void ARuyiSDKDemoCharacter::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);

	RuyiInputListener();
}

void ARuyiSDKDemoCharacter::Ruyi_StartTest()
{
	/*
	FRuyiSDKManager::Instance()->SDK()->RuyiNet->Initialise(ToRuyiString("11499"), ToRuyiString("hunter2"), []()
	{
		UE_LOG(CommonLog, Log, TEXT("FRuyiSDKManager::InitRuyiSDK RuyiNet OnInitialized !!!"));
	});*/

	//FRuyiSDKManager::Instance()->SDK()->RuyiNet->Initialise(ToRuyiString("11499"), ToRuyiString("hunter2"), NULL);
}

//////////////////////////////////////////////////////////////////////////
void ARuyiSDKDemoCharacter::InputStateChangeHandler2(std::string topic, apache::thrift::TBase* msg)
{
	auto idsc = dynamic_cast<Ruyi::SDK::InputManager::RuyiGamePadInput*>(msg);

	if (idsc == NULL)
	{
		return;
	}

	//note, joystick will call constantly while pushing it, so as the button event while pressing buttons and pushing joystick
	//please do the proper filter logic if needed

	float leftThumbX = idsc->LeftThumbX * 1.0f / powf(2.0f, 15);
	float leftThumbY = idsc->LeftThumbY * 1.0f / powf(2.0f, 15);

	//UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler2 leftThumbX:%f"), leftThumbX);

	//UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler2 ButtonFlags:%d"), idsc->ButtonFlags);

	if (0 == idsc->ButtonFlags) 
	{
		moveXAxis = 0;
		moveYAxis = 0;
	}

	//if (abs(leftThumbX) <= 0.1)
	if(leftThumbX >= -0.1f && leftThumbX <= 0.1f)
	{
		moveXAxis = 0;
	}

	//if (abs(leftThumbY) <= 0.1)
	if (leftThumbY >= -0.1f && leftThumbY <= 0.1f)
	{
		moveYAxis = 0;
	}

	if (leftThumbX >= 0.5f)
	{
		moveXAxis = 1;
	}
	if (leftThumbX <= -0.5f)
	{
		moveXAxis = -1;
	}

	if (leftThumbY >= 0.5f)
	{
		moveYAxis = 1;
	}
	if (leftThumbY <= -0.5f)
	{
		moveYAxis = -1;
	}

	if (Ruyi::SDK::CommonType::RuyiGamePadButtonFlags::GamePad_Up == idsc->ButtonFlags)
	{
		moveYAxis = 1;
	}

	if (Ruyi::SDK::CommonType::RuyiGamePadButtonFlags::GamePad_Down == idsc->ButtonFlags)
	{
		moveYAxis = -1;
	}

	if (Ruyi::SDK::CommonType::RuyiGamePadButtonFlags::GamePad_Left == idsc->ButtonFlags)
	{
		moveXAxis = -1;
	}

	if (Ruyi::SDK::CommonType::RuyiGamePadButtonFlags::GamePad_Right == idsc->ButtonFlags)
	{
		moveXAxis = 1;
	}

	if (Ruyi::SDK::CommonType::RuyiGamePadButtonFlags::GamePad_X == idsc->ButtonFlags && !isJump)
	{
		isJump = true;
		Jump();
	}

	if (0 == idsc->ButtonFlags && isJump)
	{
		isJump = false;
		StopJumping();
	}
}

void ARuyiSDKDemoCharacter::InputStateChangeHandler(std::string topic, apache::thrift::TBase* msg)
{
	//you can use UE4 engine input system, or you can use input system from sdk.
	//if you wanna call the input logic in blueprints but still get the input from Ruyi SDK
	//We recommand you listener all the input in c++ from Ruyi SDK, then call in blueprints

	FString fTopic = UTF8_TO_TCHAR(topic.c_str());
	//UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler DDDDDDDDDDDDDD topic:%s"), *fTopic);

	auto idsc = dynamic_cast<Ruyi::SDK::UserServiceExternal::InputActionEvent*>(msg);

	if (idsc == NULL)
	{
		return;
	}

	//userId is the id of the current logged account
	FString fUserId = UTF8_TO_TCHAR(idsc->userId.c_str());
	FString fAction = UTF8_TO_TCHAR(idsc->action.c_str());

	//std::vector<TriggerKeys>  Triggers is the numbers of current input from any device
	//TriggerKeys 
	//DeviceType: to identify your input device
	//Key: the key of your input device
	//NewValue/OldValue:  could be three value:0,1,2.  1 means press Down 2 means release 0 not define yet
	//NewValue is the current key state, if your press down, NewValue will be 1, when you release, NewValue will be 2, OldValue will be 1
	//action:the Key value of config file.

	//you can judge the input key by "action" value of Triggers structure. The value of "action" can be modified
	//in config file of the game package. Now I just hard-core in code. We'll try to optimise this part
	//in future release
	//all default system action value: (Layer0/RuyiLocalRoot/Resources/configs/UserSetting)
	//GamePad_LB
	//GamePad_LT
	//GamePad_L3
	//GamePad_RB
	//GamePad_RT
	//GamePad_R3
	//GamePad_UP
	//GamePad_Down
	//GamePad_Left
	//GamePad_Down
	//GamePad_Home
	//GamePad_Back
	//GamePad_Start
	//GamePad_X
	//GamePad_Y
	//GamePad_A
	//GamePad_B
	//GamePad_LJoyX
	//GamePad_LJoyY
	//GamePad_RJoyX
	//GamePad_RJoyY
	int triggerNumber = idsc->Triggers.size();

	//UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler userId:%s action:%s triggers Num:%d"), *fUserId, *fAction, triggerNumber);
	std::for_each(idsc->Triggers.begin(), idsc->Triggers.end(), [&](Ruyi::SDK::UserServiceExternal::TriggerKeys& key)
	{
		UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler deviceType:%d, key:%d, newValue:%d, oldValue:%d"), key.DeviceType, key.Key, key.NewValue, key.OldValue);
		
		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_Up")))
		{
			moveYAxis = 1;
		}
		if ((2 == key.NewValue) && (0 == idsc->action.compare("GamePad_Up")))
		{
			//Up Button Press
			moveYAxis = 0;
		}
		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_Down")))
		{
			moveYAxis = -1;
		}
		if ((2 == key.NewValue) && (0 == idsc->action.compare("GamePad_Down")))
		{
			moveYAxis = 0;
		}
		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_Left")))
		{
			moveXAxis = -1;
		}
		if ((2 == key.NewValue) && (0 == idsc->action.compare("GamePad_Left")))
		{
			moveXAxis = 0;
		}
		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_Right")))
		{
			moveXAxis = 1;
		}
		if ((2 == key.NewValue) && (0 == idsc->action.compare("GamePad_Right")))
		{
			moveXAxis = 0;
		}
		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_X")))
		{
			Jump();
		}
		if ((2 == key.NewValue) && (0 == idsc->action.compare("GamePad_X")))
		{
			StopJumping();
		}
		/*
		if (Ruyi::SDK::GlobalInputDefine::RuyiInputDeviceType::Keyboard == key.DeviceType)
		{
			if ((Ruyi::SDK::GlobalInputDefine::Key::W == key.Key || Ruyi::SDK::GlobalInputDefine::Key::Up == key.Key) && (1 == key.NewValue))
			{
				//if you wanna call the input logic in blueprints but still get the input from Ruyi SDK
				//We recommand you listener all the input in c++ from Ruyi SDK, then call in blueprints
			}
		} else if (Ruyi::SDK::GlobalInputDefine::RuyiInputDeviceType::XB360 == key.DeviceType)
		{
			if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadUp == key.Key) && (1 == key.NewValue))
			{
			}
		} else if (Ruyi::SDK::GlobalInputDefine::RuyiInputDeviceType::RuyiController == key.DeviceType)
		{
			if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonUp == key.Key && (1 == key.NewValue))
			{
			}
		} else {}
		*/
	});
}

void ARuyiSDKDemoCharacter::RuyiInputListener()
{
	MoveRight(moveXAxis);
	MoveForward(moveYAxis);
}

// Input
/*
int TimeCounter = 0;
float horizontalAxis = 0;
float verticalAxis = 0;
void InputStateChangeHandler(std::string topic, apache::thrift::TBase* msg)
{
	FString fTopic = FString(topic.c_str());

	if (nullptr == GEngine) return;
	GEngine->GetWorld();
	//TimeCounter++;

	//if (TimeCounter > 200)
	{
		//auto idsc = dynamic_cast<Ruyi::SDK::InputManager::InputDeviceStateChanged*>(msg);
		auto idsc = dynamic_cast<Ruyi::SDK::InputManager::InputActionTriggered*>(msg);
		//Ruyi::SDK::InputManager::InputActionTriggered* idsc = (Ruyi::SDK::InputManager::InputActionTriggered*)(msg);
		//Ruyi::SDK::InputManager::InputActionTriggered* idsc = Cast<Ruyi::SDK::InputManager::InputActionTriggered>(msg);

		//if (idsc == NULL) return;
		
		//UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler __isset header:%d, x360:%d, dgamepad:%d, djoystick:%d, dkeyboard:%d, dmouse:%d, ruyicontroller:%d !!!"), idsc->__isset.header, idsc->__isset.x360, idsc->__isset.dgamepad, idsc->__isset.djoystick, idsc->__isset.dkeyboard, idsc->__isset.dmouse, idsc->__isset.ruyicontroller);
		
		FString deviceId = UTF8_TO_TCHAR(idsc->deviceId.c_str());
		FString name = UTF8_TO_TCHAR(idsc->name.c_str());

		UE_LOG(CommonLog, Log, TEXT("InputActionTriggered deviceId:%s name:%s"), *deviceId, *name);

		//TimeCounter = 0;
	}
}
*/
std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value)
{
	while (true)
	{
		std::string::size_type pos(0);
		if ((pos = str.find(old_value)) != std::string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}
	return str;
}

void ARuyiSDKDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	return;
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward_1P", this, &ARuyiSDKDemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight_1P", this, &ARuyiSDKDemoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARuyiSDKDemoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARuyiSDKDemoCharacter::LookUpAtRate);

	// handle touch devices
	//PlayerInputComponent->BindTouch(IE_Pressed, this, &ARuyiSDKDemoCharacter::TouchStarted);
	//PlayerInputComponent->BindTouch(IE_Released, this, &ARuyiSDKDemoCharacter::TouchStopped);

	// VR headset functionalit
	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ARuyiSDKDemoCharacter::OnResetVR);
}

void ARuyiSDKDemoCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ARuyiSDKDemoCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (IsPaused) return;

	Jump();
}

void ARuyiSDKDemoCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (IsPaused) return;

	StopJumping();
}

void ARuyiSDKDemoCharacter::TurnAtRate(float Rate)
{
	if (IsPaused) return;

	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARuyiSDKDemoCharacter::LookUpAtRate(float Rate)
{
	if (IsPaused) return;

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARuyiSDKDemoCharacter::MoveForward(float Value)
{
	if (IsPaused) return;

	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARuyiSDKDemoCharacter::MoveRight(float Value)
{
	if (IsPaused) return;

	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
