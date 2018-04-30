// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "AstroneerApexCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AAstroneerApexCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;



	// hoover
	UPROPERTY(Category = HooverSettings, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ChunkDetructorCollisonComp;

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	bool DestroyDestructibleChunk(UDestructibleComponent* DestructibleComp, int32 HitItem);

	FTimerHandle TimerHandle_Hoover;

	UPROPERTY(Category = HooverSettings, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float HooverSpeed;

	UPROPERTY(Category = HooverSettings, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int32 MaxNumberOfHooverLazers;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartHoover();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopHoover();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Fire();

	void StartSimulateHoover();

	void StopSimulateHoover();

	void SimulateHoover();

	void SimulateFire();

	// fire
public:
	AAstroneerApexCharacter();

	virtual void BeginPlay();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AAstroneerApexProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:
	
	void StartFire();

	void StartHoover();

	void StopHoover();

	/** Fires a projectile. */
	void OnFire();

	/** Start Hoover. */
	void OnHoover();

	void HooverOff();

	void DoHoover();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

