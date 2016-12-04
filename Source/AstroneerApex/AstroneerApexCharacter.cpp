// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "AstroneerApex.h"
#include "AstroneerApexCharacter.h"
#include "AstroneerApexProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"

//Apex issues, can add iOS here  <3 Rama
#if PLATFORM_ANDROID || PLATFORM_HTML5_BROWSER || PLATFORM_IOS
#ifdef WITH_APEX
#undef WITH_APEX
#endif
#define WITH_APEX 0
#endif //APEX EXCLUSIONS

//~~~ PhysX ~~~
#include "PhysXIncludes.h"
#include "PhysXPublic.h"		//For the ptou conversions

//For Scene Locking using Epic's awesome helper macros like SCOPED_SCENE_READ_LOCK
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AAstroneerApexCharacter

AAstroneerApexCharacter::AAstroneerApexCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
}

void AAstroneerApexCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	
	Mesh1P->SetHiddenInGame(false, true);
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAstroneerApexCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AAstroneerApexCharacter::OnFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAstroneerApexCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAstroneerApexCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AAstroneerApexCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AAstroneerApexCharacter::LookUpAtRate);
}

void AAstroneerApexCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			// spawn the projectile at the muzzle
			World->SpawnActor<AAstroneerApexProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AAstroneerApexCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AAstroneerApexCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AAstroneerApexCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AAstroneerApexCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AAstroneerApexCharacter::DestroyDestructibleChunk(UDestructibleComponent* DestructibleComp, int32 HitItem)
{
#if WITH_APEX
	if (!DestructibleComp)
	{
		return false;
	}

	//Visibility
	DestructibleComp->SetChunkVisible(HitItem, false);

	//Collision
	physx::PxShape** PShapes;
	const physx::PxU32 PShapeCount = DestructibleComp->ApexDestructibleActor->getChunkPhysXShapes(PShapes, HitItem);
	if (PShapeCount > 0)
	{
		PxFilterData PQueryFilterData, PSimFilterData; //null data

		for (uint32 ShapeIndex = 0; ShapeIndex < PShapeCount; ++ShapeIndex)
		{
			PxShape* Shape = PShapes[ShapeIndex];
			if (!Shape) continue;

			{
				SCOPED_SCENE_WRITE_LOCK(Shape->getActor()->getScene());

				Shape->setQueryFilterData(PQueryFilterData); //null data
				Shape->setSimulationFilterData(PSimFilterData); //null data
				Shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
				Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				Shape->setFlag(PxShapeFlag::eVISUALIZATION, false);
			}
		}
	}
	return true;
#endif //WITH_APEX

	UE_LOG(LogTemp, Error, TEXT("AMyPawn::DestroyDestructibleChunk ~ Current Platform does not support APEX"));
	return false;
}