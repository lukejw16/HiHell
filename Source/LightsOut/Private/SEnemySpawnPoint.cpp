// Fill out your copyright notice in the Description page of Project Settings.


#include "SEnemySpawnPoint.h"

// Sets default values
ASEnemySpawnPoint::ASEnemySpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASEnemySpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

