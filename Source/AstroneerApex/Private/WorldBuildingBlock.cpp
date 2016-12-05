// Fill out your copyright notice in the Description page of Project Settings.

#include "AstroneerApex.h"
#include "WorldBuildingBlock.h"


// Sets default values
AWorldBuildingBlock::AWorldBuildingBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DestructibleComp = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComp"));
	RootComponent = DestructibleComp;
}

// Called when the game starts or when spawned
void AWorldBuildingBlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldBuildingBlock::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

