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

void InputStateChangeHandler(std::string topic, apache::thrift::TBase* msg);
std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value);
void ARuyiSDKDemoCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(CommonLog, Log, TEXT("ARuyiSDKDemoCharacter::BeginPlay()"));

	FRuyiSDKManager::Instance();
	
	const string* pChar = &Ruyi::SDK::Constants::g_ConstantsSDKDataTypes_constants.layer0_publisher_out_uri;
	string* modifier = const_cast<string*>(pChar);
	replace_all(*modifier, "{addr}", "localhost");

	//void(*aaa)(string, TBase*);
	//MessageHandler handler;
	//handler = (MessageHandler)(&ARuyiSDKDemoCharacter::InputStateChangeHandler);
	//handler =  reinterpret_cast<MessageHandler>( &ARuyiSDKDemoCharacter::InputStateChangeHandler);
	if (FRuyiSDKManager::Instance()->IsSDKReady)
	{
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Subscribe("service/inputmanager");
		FRuyiSDKManager::Instance()->SDK()->Subscriber->AddMessageHandler(InputStateChangeHandler);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input
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
		//auto idsc = dynamic_cast<Ruyi::SDK::InputManager::InputActionTriggered*>(msg);
		Ruyi::SDK::InputManager::InputActionTriggered* idsc = (Ruyi::SDK::InputManager::InputActionTriggered*)(msg);
		//Ruyi::SDK::InputManager::InputActionTriggered* idsc = Cast<Ruyi::SDK::InputManager::InputActionTriggered>(msg);

		//if (idsc == NULL) return;
		
		//UE_LOG(CommonLog, Log, TEXT("InputStateChangeHandler __isset header:%d, x360:%d, dgamepad:%d, djoystick:%d, dkeyboard:%d, dmouse:%d, ruyicontroller:%d !!!"), idsc->__isset.header, idsc->__isset.x360, idsc->__isset.dgamepad, idsc->__isset.djoystick, idsc->__isset.dkeyboard, idsc->__isset.dmouse, idsc->__isset.ruyicontroller);
		
		FString deviceId = UTF8_TO_TCHAR(idsc->deviceId.c_str());
		FString name = UTF8_TO_TCHAR(idsc->name.c_str());

		UE_LOG(CommonLog, Log, TEXT("InputActionTriggered deviceId:%s name:%s"), *deviceId, *name);

		//TimeCounter = 0;
	}
}

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
