// Fill out your copyright notice in the Description page of Project Settings.

#include "SDMGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SEnemySpawnPoint.h"
#include "SCharacter.h"
#include "SGameState.h"
#include "SSpawnPoint.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "DemonCharacter.h"
#include "..\Public\SDMGameMode.h"

void ASDMGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ASDMGameMode::ASDMGameMode()
{
	bStartPlayersAsSpectators = false;
	
	TimerSeconds = 0;
	TimerMinutes = 1;

	//

}
void ASDMGameMode::BeginPlay()
{
	Super::BeginPlay();

	//GetWorldTimerManager().SetTimer(TimeHandle, this, &ASDMGameMode::TakeTimeAway, 1.0f, true);

	TSubclassOf<ASEnemySpawnPoint> SpawnPoint;
	SpawnPoint = ASEnemySpawnPoint::StaticClass();
	TArray<AActor*> foundSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPoint, foundSpawnPoints);
	int foundNumber = foundSpawnPoints.Num() - 1;
	int randomnumber = (int)FMath::RandRange(0, foundNumber);

	EnemyLocation = foundSpawnPoints[randomnumber]->GetActorLocation();
	
	GS = GetWorld()->GetGameState<ASGameState>();
}

void ASDMGameMode::SpawnPlayer(AActor* Owner, ASCharacter* Player)
{
	GS->bGameHasBegin = true;

	TSubclassOf<ASSpawnPoint> SpawnPoint;
	SpawnPoint = ASSpawnPoint::StaticClass();
	TArray<AActor*> foundSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPoint, foundSpawnPoints);
	
	int foundNumber = foundSpawnPoints.Num()-1;
	int randomnumber = FMath::RandRange(0, foundNumber);

	if (randomnumber > foundNumber)
	{
		int randomnumber = FMath::RandRange(0, foundNumber);
		FVector EnemyLoc = foundSpawnPoints[randomnumber]->GetActorLocation();
		Owner->SetActorLocation(EnemyLoc);
	}
	else
	{
		FVector EnemyLoc = foundSpawnPoints[randomnumber]->GetActorLocation();
		Owner->SetActorLocation(EnemyLoc);
	}

	
	
	
}

void ASDMGameMode::RespawnDemon(AActor * Owner)
{
	TSubclassOf<ASEnemySpawnPoint> SpawnPoint;
	SpawnPoint = ASEnemySpawnPoint::StaticClass();
	TArray<AActor*> foundSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPoint, foundSpawnPoints);

	int foundNumber = foundSpawnPoints.Num() - 1;
	int randomnumber = FMath::RandRange(0, foundNumber);

	if (randomnumber > foundNumber)
	{
		int randomnumber = FMath::RandRange(0, foundNumber);
		Owner->SetActorLocation(foundSpawnPoints[randomnumber]->GetActorLocation());
	}
	else
	{
		Owner->SetActorLocation(foundSpawnPoints[randomnumber]->GetActorLocation());
	}

	
}



FString ASDMGameMode::AssignTeamToPlayer(ASCharacter* player)
{
	ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
	if (GS)
	{
		if (GS->RedTeamCount == GS->BlueTeamCount && GS->BlueTeamCount < 1)
		{
			//assign to either team, do random number jazz
			int random = FMath::RandRange(0, 1);
			if (random == 0)
			{
				GS->BlueTeamCount++;
				FTimerDelegate TimerDel;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SPAWN DEMON")));
				TimerDel.BindUFunction(this, FName("SpawnDemon"), player);
				GetWorldTimerManager().SetTimer(UnusedHandle, TimerDel, 0.1f, false);
				//SpawnDemon(player);
				return FString(TEXT("Demon"));
				
			}
			else
			{
				GS->RedTeamCount++;
				//SpawnPlayer(player->GetOwner(), player);
				TSubclassOf<ASSpawnPoint> SpawnPoint;
				SpawnPoint = ASSpawnPoint::StaticClass();
				TArray<AActor*> foundSpawnPoints;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPoint, foundSpawnPoints);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SPAWN PLAYER")));
				int foundNumber = foundSpawnPoints.Num() - 1;
				int randomnumber = FMath::RandRange(0, foundNumber);
				player->SetActorLocation(foundSpawnPoints[randomnumber]->GetActorLocation());
				return FString(TEXT("Red"));
			}
		}
		if (GS->RedTeamCount > GS->BlueTeamCount && GS->BlueTeamCount < 1)
		{
			//assign to blue team
			GS->BlueTeamCount++;
			FTimerDelegate TimerDel;

			TimerDel.BindUFunction(this, FName("SpawnDemon"), player);
			GetWorldTimerManager().SetTimer(UnusedHandle, TimerDel, 0.1f, false);
			//SpawnDemon(player);
			return FString(TEXT("Demon"));
			

			//++ to blue team 
		}
		else {
			//assign to red team 
			GS->RedTeamCount++;
			TSubclassOf<ASSpawnPoint> SpawnPoint;
			SpawnPoint = ASSpawnPoint::StaticClass();
			TArray<AActor*> foundSpawnPoints;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPoint, foundSpawnPoints);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SPAWN PLAYER")));
			int foundNumber = foundSpawnPoints.Num() - 1;
			int randomnumber = FMath::RandRange(0, foundNumber);
			player->SetActorLocation(foundSpawnPoints[randomnumber]->GetActorLocation());
			return FString(TEXT("Red"));

			//++ to red team 
		}

		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CantfindGameState")));
		return FString(TEXT("FreeForAll"));
	}

	
	
}

void ASDMGameMode::SpawnDemon(ASCharacter * Player)
{
	AController* Controller = Player->GetController();
	FVector Location = Player->GetActorLocation();
	
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ADemonCharacter* DemonPlayer = GetWorld()->SpawnActor<ADemonCharacter>(EnemyPlayer, EnemyLocation, FRotator::ZeroRotator, SpawnParams);
	
	GetWorldTimerManager().ClearTimer(UnusedHandle);

	
	Controller->UnPossess();
	Controller->Possess(Cast<ADemonCharacter>(DemonPlayer));
	

	Player->DestroyWeapon();
	Player->Destroy();

	
}

void ASDMGameMode::ChangeTeam()
{
	ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
	GS->RedTeamCount--;
	GS->BlueTeamCount++;
}

void ASDMGameMode::TakeTimeAway()
{
	if (TimerSeconds > 0)
	{
		TimerSeconds--;
	}
	else
	{
		TimerSeconds = 60;
		TimerMinutes--;
	}
}

void ASDMGameMode::ImplimentReady()
{
	if (GS != nullptr)
	{
		GS->IncrementReadyUp();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ReadyUp: %f"), GS->ReadyUp));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("NO GAMESTATE")));
	}
	
}



void ASDMGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDMGameMode, TimerMinutes);
	DOREPLIFETIME(ASDMGameMode, TimerSeconds);
}



