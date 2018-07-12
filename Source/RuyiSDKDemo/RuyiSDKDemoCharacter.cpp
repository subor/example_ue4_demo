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
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Subscribe("service/inputmgr_int");
		FRuyiSDKManager::Instance()->SDK()->Subscriber->AddMessageHandler(this, &ARuyiSDKDemoCharacter::InputStateChangeHandler);
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
	
}

//////////////////////////////////////////////////////////////////////////
void ARuyiSDKDemoCharacter::InputStateChangeHandler(std::string topic, apache::thrift::TBase* msg)
{
	//you can use UE4 engine input system, or you can use input system from sdk.
	//if you wanna call the input logic in blueprints but still get the input from Ruyi SDK
	//We recommand you listener all the input in c++ from Ruyi SDK, then call in blueprints

	auto idsc = dynamic_cast<Ruyi::SDK::InputManager::RuyiGamePadInput*>(msg);

	if (idsc == NULL)
	{
		return;
	}

	//note, joystick will call constantly while pushing it, so as the button event while pressing buttons and pushing joystick
	//please do the proper filter logic if needed

	float leftThumbX = idsc->LeftThumbX * 1.0f / powf(2.0f, 15);
	float leftThumbY = idsc->LeftThumbY * 1.0f / powf(2.0f, 15);

	if (0 == idsc->ButtonFlags) 
	{
		moveXAxis = 0;
		moveYAxis = 0;
	}

	//if (abs(leftThumbX) <= 0.1f)
	if(leftThumbX >= -0.1f && leftThumbX <= 0.1f)
	{
		moveXAxis = 0;
	}

	//if (abs(leftThumbY) <= 0.1f)
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

	if (Ruyi::SDK::CommonType::RuyiGamePadButtonFlags::GamePad_Y == idsc->ButtonFlags && !isJump)
	{
		//FRuyiSDKManager::Instance()->SDK()->
	}

	if (Ruyi::SDK::CommonType::RuyiGamePadButtonFlags::GamePad_B == idsc->ButtonFlags && !isJump)
	{
		uint8_t motor1Value = 255, motor1Time = 255, motor2Value = 127, motor2Time = 127;
		//FRuyiSDKManager::Instance()->SDK()->InputMgr->SetRuyiControllerStatus(4, false, false, false, true, true, false, 0, 0, 0, motor1Value, motor1Time, motor2Value, motor2Time);
	}

	if (0 == idsc->ButtonFlags && isJump)
	{
		isJump = false;
		StopJumping();
	}
}

void ARuyiSDKDemoCharacter::RuyiInputListener()
{
	MoveRight(moveXAxis);
	MoveForward(moveYAxis);
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
