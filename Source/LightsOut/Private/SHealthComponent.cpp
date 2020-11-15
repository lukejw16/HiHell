// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "SSpawnPoint.h"
#include "SDMGameMode.h"
#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	DefaultHealth = 100;

	
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	// only if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		MyOwner = GetOwner();

		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}

		
	}

	Health = DefaultHealth;

}

void USHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::Respawn()
{
	Health = DefaultHealth;
	//set actor position 

	GM = GetWorld()->GetAuthGameMode<ASDMGameMode>();

	ASCharacter* player = Cast<ASCharacter>(MyOwner);

	if (GM && MyOwner)
	{
		GM->SpawnPlayer(MyOwner, player);
	}
	
	
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);

}


