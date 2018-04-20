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
}

std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value);

void ARuyiSDKDemoCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(CommonLog, Log, TEXT("ARuyiSDKDemoCharacter::BeginPlay()"));

	FRuyiSDKManager::Instance();
	
	const string* pChar = &Ruyi::SDK::Constants::g_ConstantsSDKDataTypes_constants.layer0_publisher_out_uri;
	string* modifier = const_cast<string*>(pChar);
	replace_all(*modifier, "{addr}", "localhost");

	
	if (FRuyiSDKManager::Instance()->IsSDKReady)
	{
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Subscribe("service/inputmanager_internal");
		FRuyiSDKManager::Instance()->SDK()->Subscriber->AddMessageHandler(this, &ARuyiSDKDemoCharacter::InputStateChangeHandler);
	}

	PrimaryActorTick.bCanEverTick = true;
}

void ARuyiSDKDemoCharacter::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
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
void ARuyiSDKDemoCharacter::InputStateChangeHandler(std::string topic, apache::thrift::TBase* msg)
{
	//you can use UE4 engine input system, or you can use input system from sdk.
	//if if you wanna call the input logic in blueprints but still get the input from Ruyi SDK
	//We recommand you listener all the input in c++ from Ruyi SDK, then call in blueprints

	//UE_LOG(CommonLog, Log, TEXT("ARuyiSDKDemoCharacter::InputStateChangeHandler !!!"));

	auto idsc = dynamic_cast<Ruyi::SDK::InputManager::InputDeviceStateChanged*>(msg);

	if (idsc == NULL)
	{
		return;
	}

	if (idsc->__isset.header == 1)
	{
		//UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler header:%d"), idsc->__isset.header);
	}
	if (idsc->__isset.x360 == 1)
	{
		int64_t PacketNumber = idsc->x360.PacketNumber;
		//FString fDeviceId = UTF8_TO_TCHAR(idsc->x360.DeviceId.c_str());
		int16_t LeftThumbDeadZone = idsc->x360.LeftThumbDeadZone;
		int16_t RightThumbDeadZone = idsc->x360.RightThumbDeadZone;
		int32_t Buttons = idsc->x360.Buttons;
		int8_t LeftTrigger = idsc->x360.LeftTrigger;
		int8_t RightTrigger = idsc->x360.RightTrigger;
		int16_t LeftThumbX = idsc->x360.LeftThumbX;
		int16_t LeftThumbY = idsc->x360.LeftThumbY;
		int16_t RightThumbX = idsc->x360.RightThumbX;
		int16_t RightThumbY = idsc->x360.RightThumbY;
		//bool isPressed = true;
		//bool isReleased = false;
		if (Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadUp == Buttons)
		{
			UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler x360 DPadUp"));
		}		
	}
	if (idsc->__isset.dgamepad == 1)
	{
		UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler dgamepad:%d"), idsc->__isset.dgamepad);
	}
	if (idsc->__isset.djoystick == 1)
	{
		UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler djoystick:%d"), idsc->__isset.djoystick);
	}
	if (idsc->__isset.dkeyboard == 1)
	{
		char key = idsc->dkeyboard.Key;
		bool isPressed = idsc->dkeyboard.IsPressed;
		bool isReleased = idsc->dkeyboard.IsReleased;

		if ((Ruyi::SDK::GlobalInputDefine::Key::W == key || Ruyi::SDK::GlobalInputDefine::Key::Up == key) && isPressed)
		{
			UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler keybaord W"));
		}
	}
	if (idsc->__isset.dmouse == 1)
	{
		//UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler dmouse:%d"), idsc->__isset.dmouse);
	}
	if (idsc->__isset.ruyicontroller == 1)
	{
		int64_t PacketId = idsc->ruyicontroller.PacketId;
		int32_t ChannelId = idsc->ruyicontroller.ChannelId;
		//FString fDeviceId = UTF8_TO_TCHAR(idsc->ruyicontroller.DeviceId.c_str());
		int32_t KeyPress = idsc->ruyicontroller.KeyPress;
		int8_t AnalogL2 = idsc->ruyicontroller.AnalogL2;
		int8_t AnalogR2 = idsc->ruyicontroller.AnalogR2;
		int8_t AnalogLeftJoyX = idsc->ruyicontroller.AnalogLeftJoyX;
		int8_t AnalogLeftJoyY = idsc->ruyicontroller.AnalogLeftJoyY;
		int8_t AnalogRightJoyX = idsc->ruyicontroller.AnalogRightJoyX;
		int8_t AnalogRightJoyY = idsc->ruyicontroller.AnalogRightJoyY;
		//tempoary, ruyi controller not support isPressd/isReleased
		bool isPressed = true;
		bool isReleased = false;
		if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonUp == KeyPress)
		{
			UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler ruyicontroller eButtonUp"));
		}
		
		UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler ruyicontroller PacketId:%d, ChannelId:%d"), PacketId, ChannelId);
		UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler ruyicontroller KeyPress:%d, AnalogL2:%d, AnalogR2:%d"), KeyPress, AnalogL2, AnalogR2);
		UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler ruyicontroller AnalogLeftJoyX:%d, AnalogLeftJoyY:%d, AnalogRightJoyX:%d, AnalogRightJoyY:%d"), AnalogLeftJoyX, AnalogLeftJoyY, AnalogRightJoyX, AnalogRightJoyY);
	}
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
