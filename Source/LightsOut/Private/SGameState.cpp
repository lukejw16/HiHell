// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"
#include "SCharacter.h"
#include "SDMGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"


void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, RedTeamCount);
	DOREPLIFETIME(ASGameState, BlueTeamCount);
	DOREPLIFETIME(ASGameState, RedTeamScore);
	DOREPLIFETIME(ASGameState, BlueTeamScore);
	DOREPLIFETIME(ASGameState, TimerSeconds);
	DOREPLIFETIME(ASGameState, TimerMinutes);
	DOREPLIFETIME(ASGameState, ReadyUp);
	DOREPLIFETIME(ASGameState, readyUpAmount);
}

void ASGameState::IncrementReadyUp()
{

	ReadyUp += 1;
	FString string = FString::FromInt(ReadyUp);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ReadyUp: %f"), ReadyUp));
	
}

void ASGameState::CheckIfAllPlayersAreReady()
{
	
	TSubclassOf<ASCharacter> Character;
	Character = ASCharacter::StaticClass();
	TArray<AActor*> foundCharacter;
	if (GetWorld())
	{
		//Statement that uses GetWorld()
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), Character, foundCharacter);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Searching")));
	}
	
	
	if (foundCharacter.Num() != 0)
	{
		for (int i = 0; i < foundCharacter.Num(); i++) {
			AActor* Owner = foundCharacter[i];
			ASCharacter* CurrentChar = Cast<ASCharacter>(Owner);
			if (CurrentChar != nullptr)
			{
				if (CurrentChar->PlayerReady)
				{
					readyUpAmount += 1;
					Multicast_Increase(readyUpAmount);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("FoundPlayerWithPlayerReady!")));
					Server_CheckComplete(foundCharacter.Num(), CurrentChar);
					if (readyUpAmount == foundCharacter.Num())
					{
						bGameHasBegin = true;
					}
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Cast didnt work boss")));
			}
			
		}

		readyUpAmount = 0;
	}
	
}

void ASGameState::BeginPlay()
{
	Super::BeginPlay();

	bDoOnce = false;
	//bGameHasBegin = false;
	//GetWorldTimerManager().SetTimer(PlayerCheckReadyUp, this, &ASGameState::CheckIfAllPlayersAreReady, 5.0f, true);
	//ReadyUp = 2;

	//TimerSeconds = 60;
	//TimerMinutes = 4;
	


}

void ASGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (bGameHasBegin)
	{
		

		if (!bDoOnce)
		{
			bDoOnce = true;
			GetWorldTimerManager().SetTimer(RemainingPlayerCheck, this, &ASGameState::RemainingPlayers, 2.0f, true);
			GetWorldTimerManager().SetTimer(TimeHandle, this, &ASGameState::TakeTimeAway, 1.0f, true);
		}
	}
	
}


void ASGameState::RemainingPlayers()
{
	if (!EndGame)
	{
		if (RedTeamCount <= 0 && BlueTeamCount >= 2)
		{
			EndGame = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("EndGame")));
			WinningTeamName = FString(TEXT("Demons Wins!"));


			
		}
		
		//EndGame = false;

	}
	
	
}

void ASGameState::ClientRPC_CallMe_Implementation()
{
	bGameHasBegin = true;
}

void ASGameState::Server_CheckComplete_Implementation(int playeramount, ASCharacter* Char)
{
	if (readyUpAmount == playeramount)
	{
		bGameHasBegin = true;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("bGameHasBegin")));
		ClientRPC_CallMe();
		ASDMGameMode* GM = GetWorld()->GetAuthGameMode<ASDMGameMode>();
		//GM->SpawnPlayer(Char->GetOwner(), Char);
	}
}

void ASGameState::Multicast_Increase_Implementation(int amount)
{
	readyUpAmount = amount;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Another player has readied up")));
}

void ASGameState::AssignTeamToPlayer(ASCharacter* player)
{
	//ASGameState* GS = GetWorld()->GetGameState<ASGameState>();

	

	if (RedTeamCount == 0 && BlueTeamCount == 0)
	{
			//assign to either team, do random number jazz
			int random = FMath::RandRange(0, 1);
			if (random == 0)
			{
				//BlueTeamCount++;

				
			}
			else
			{
				//RedTeamCount++;

				
			}
	}
	
	if (RedTeamCount > BlueTeamCount)
	{
			//assign to blue team
			//BlueTeamCount++;

		

			//++ to blue team 
	}else{
			//assign to red team 
			//RedTeamCount++;
	
	

			//++ to red team 
	}




	
	

}



void ASGameState::Multicast_CreateWidget_Implementation(TSubclassOf<class UUserWidget> WBPReference)
{
	if (WBPReference != nullptr)
	{
		UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), WBPReference);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
		}
	}
}


void ASGameState::TakeTimeAway()
{
	if (!EndGame)
	{
		if (TimerSeconds <= 0 && TimerMinutes <= 0)
		{
			GetWorldTimerManager().ClearTimer(TimeHandle);
			EndGame = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("EndGame")));
			WinningTeamName = FString(TEXT("Survivors Win!"));


			
			//EndGame = false;
		}

		if (TimerSeconds > 0 && TimerMinutes >= 0)
		{
			TimerSeconds-= 1;
		}
		else if (TimerMinutes > 0)
		{
			TimerMinutes -= 1;
			TimerSeconds = 60;
		}
		

		
	}
	
}