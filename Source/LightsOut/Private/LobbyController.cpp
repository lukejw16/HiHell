// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyController.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "SGameState.h"

// Sets default values
ALobbyController::ALobbyController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALobbyController::BeginPlay()
{
	Super::BeginPlay();
	
	if (wLobbyWidget) {

		auto name = FName(TEXT("CH"));
		LobbyWidget = CreateWidget<UUserWidget>(GetGameInstance(), wLobbyWidget, name);
		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();
		}
	}

	GS = GetWorld()->GetGameState<ASGameState>();

}

// Called every frame
void ALobbyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Server_RemoveWidget();

}

void ALobbyController::ClientRPC_RemoveWidget_Implementation(bool gamestart)
{
	if (gamestart)
	{
		//Multicast_RemoveWidget();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GAMEREADY")));
	}
}

void ALobbyController::Multicast_RemoveWidget_Implementation()
{
	
		LobbyWidget->RemoveFromParent();
	
}

void ALobbyController::Server_RemoveWidget_Implementation()
{
	ClientRPC_RemoveWidget(GS->bGameHasBegin);
	
	
	
}

void ALobbyController::ClientRPC_ImplimentReady_Implementation()
{
	Server_IncrementReadyUp(ReadyUp);
}

void ALobbyController::Server_IncrementReadyUp_Implementation(int iReadyUp)
{
	iReadyUp++;
	ReadyUp = iReadyUp;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ReadyUp: %f"), ReadyUp));
}


void ALobbyController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyController, ReadyUp);

}


