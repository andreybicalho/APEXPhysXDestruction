// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WorldBuildingBlock.generated.h"

UCLASS()
class ASTRONEERAPEX_API AWorldBuildingBlock : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Destructible, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	class UDestructibleComponent* DestructibleComp;
	
public:	
	// Sets default values for this actor's properties
	AWorldBuildingBlock();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
