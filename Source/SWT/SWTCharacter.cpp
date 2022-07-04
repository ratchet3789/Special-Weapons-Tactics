// Copyright Epic Games, Inc. All Rights Reserved.

#include "SWTCharacter.h"
#include "SWTProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include <Kismet/KismetMathLibrary.h>
#include "DynamicClimbActor.h"


//////////////////////////////////////////////////////////////////////////
// ASWTCharacter

ASWTCharacter::ASWTCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

}

void ASWTCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input

void ASWTCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ASWTCharacter::OnPrimaryAction);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASWTCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASWTCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ASWTCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ASWTCharacter::LookUpAtRate);
}

void ASWTCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void ASWTCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ASWTCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void ASWTCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASWTCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASWTCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ASWTCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool ASWTCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASWTCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ASWTCharacter::EndTouch);

		return true;
	}

	return false;
}

void ASWTCharacter::LedgeCheck(FVector StartPosition, FRotator StartRotation, float Height, bool bFirstTime)
{
	if(bFirstTime)
	{
		ledgeCheckCount=0;
		SplinePositions.Empty();
	}
	
	FVector FwdVec = UKismetMathLibrary::GetForwardVector(StartRotation);
	FVector StartVec = StartPosition;
	FVector EndVec = FwdVec * 100.0f + StartPosition;

	FHitResult HitResult;
	// Cast Forward
	GetWorld()->LineTraceSingleByChannel(HitResult, StartVec, EndVec, ECC_Visibility);
	DrawDebugLine(GetWorld(), StartVec, EndVec, FColor::Red, false, 10.0f, 0, 0.5f);
	// If we've hit something then we re-run this script
	if (HitResult.GetActor() != nullptr)
	{
		ledgeCheckCount++;
		LedgeCheck(StartPosition + FVector::UpVector * Height, StartRotation, Height, false);
		return;
	}

	// If we've checked more than once and we've hit walls constantly
	// We now use the length of the cast + 50uu to cast downwards 100 units
	// to see if we've hit a solid surface with the tag of Climbable
	if (ledgeCheckCount > 0)
	{
		// As were at the end we can set our start and end hit points
		SplinePositions.Add(UKismetMathLibrary::MakeTransform(GetActorLocation(), FRotator(0.0f, RoundToNearest90(StartRotation.Yaw), RoundToNearest90(StartRotation.Roll)))

		StartVec = FwdVec * 100.0f + StartPosition;
		EndVec = (StartVec + (FwdVec * 50.0f)) - FVector::UpVector * 100.0f;
		GetWorld()->LineTraceSingleByChannel(HitResult, StartVec, EndVec, ECC_Visibility);

		DrawDebugLine(GetWorld(), StartVec, EndVec, FColor::Blue, false, 10.0f, 0, 0.5f);

		if (HitResult.GetActor() != nullptr)
		{
			DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector::OneVector * 100.0f, FQuat::Identity, FColor::Black, false, 10.0f, 0, 1.0f);
			DrawDebugLine(GetWorld(), StartVec, EndVec, FColor::Blue, false, 10.0f, 1, 0.5f);
			if(HitResult.GetActor()->ActorHasTag("Climbable"))
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Hit Climbable, Draw Climb."));
				GetWorld()->SpawnActor(ADynamicClimbActor::StaticClass(), new FTransform(UKismetMathLibrary::MakeTransform(GetActorLocation(), FRotator(0.0f, RoundToNearest90(StartRotation.Yaw), RoundToNearest90(StartRotation.Roll)), FVector::OneVector * 0.25f)));
			}
		}
	}
}