// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyController.generated.h"

class UUserWidget;
class ASGameState;

UCLASS()
class LIGHTSOUT_API ALobbyController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyController();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Lobby")
	int ReadyUp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	int MinPartySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wLobbyWidget;

	UUserWidget* LobbyWidget;

	ASGameState* GS;

	UFUNCTION(Server, BlueprintCallable, Reliable)
		void Server_IncrementReadyUp(int iReadyUp);
	
	UFUNCTION(Client, BlueprintCallable, Reliable)
	void ClientRPC_ImplimentReady();

	UFUNCTION(Client, BlueprintCallable, Reliable)
		void ClientRPC_RemoveWidget(bool gamestart);

	UFUNCTION(Server, BlueprintCallable, Reliable)
		void Server_RemoveWidget();

	UFUNCTION(NetMulticast, BlueprintCallable, Reliable)
		void Multicast_RemoveWidget();
};
