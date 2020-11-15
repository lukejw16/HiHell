// Fill out your copyright notice in the Description page of Project Settings.

#include "SSpawnPoint.h"


// Sets default values
ASSpawnPoint::ASSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

