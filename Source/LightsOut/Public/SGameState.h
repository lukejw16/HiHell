// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Blueprint/UserWidget.h"
#include "SGameState.generated.h"

/**
 * 
 */
class ASCharacter;
class UUserWidget;

UCLASS()
class LIGHTSOUT_API ASGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	bool bDoOnce;

	bool bOnlyOnce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "EndGame")
	bool bGameHasBegin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "TeamInfo")
	int ReadyUp;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "EndGame")
	TSubclassOf<class UUserWidget> EndGameWBPReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "EndGame")
	bool EndGame = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "EndGame")
		FString WinningTeamName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "TeamInfo")
		int RedTeamCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "TeamInfo")
		int BlueTeamCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "TeamInfo")
		int RedTeamScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "TeamInfo")
		int BlueTeamScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "GameInfo")
		int EndGamePointsTally = 100;

	void RemainingPlayers();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "EndGame")
	int readyUpAmount;

	UFUNCTION(Client, Reliable)
	void ClientRPC_CallMe();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Increase(int amount);

	UFUNCTION(Server, Reliable)
		void Server_CheckComplete(int playeramount, ASCharacter* Char);

	FTimerHandle RemainingPlayerCheck;
	FTimerHandle PlayerCheckReadyUp;

	float RemainingPlayerCheckTime;

	TArray<AActor*> foundCharacters;

	TSubclassOf<ASCharacter> Characters;

	void AssignTeamToPlayer(ASCharacter* player);

	//make multicast function for widget 
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_CreateWidget(TSubclassOf<class UUserWidget> WBPReference);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		int TimerMinutes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		int TimerSeconds = 60;

	void TakeTimeAway();

	FTimerHandle TimeHandle;

	UFUNCTION(BlueprintCallable)
		void IncrementReadyUp();

	void CheckIfAllPlayersAreReady();

protected:

	virtual void BeginPlay() override;


};
