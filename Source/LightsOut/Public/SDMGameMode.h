// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SDMGameMode.generated.h"

/**
 * 
 */
class ASSpawnPoint;
class ASCharacter;
class ADemonCharacter;
class ASGameState;

UCLASS()
class LIGHTSOUT_API ASDMGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:

	ASDMGameMode();

	ASGameState* GS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
		int MinPartySize;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TSubclassOf<ADemonCharacter> EnemyPlayer;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		ADemonCharacter* Controller;

	FVector EnemyLocation;

	void SpawnPlayer(AActor* Owner, ASCharacter* Player);

	FString AssignTeamToPlayer(ASCharacter* player);

	UFUNCTION()
	void SpawnDemon(ASCharacter* Player);

	void RespawnDemon(AActor * Owner);

	FTimerHandle UnusedHandle;

	FTimerHandle TimeHandle;

	void ChangeTeam();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	int TimerMinutes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	int TimerSeconds;

	void TakeTimeAway();

	UFUNCTION(BlueprintCallable)
		void ImplimentReady();

	
};
