// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SWeapon.h"
#include "LightsOut.h"
#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SGameState.h"
#include "SDMGameMode.h"
#include "Components/SpotLightComponent.h"
#include "EngineGlobals.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundCue.h"
#include "Engine/Engine.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitailizeVariables();

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultFOV = CameraComp->FieldOfView;
	AActor* Owner = GetOwner();
	GM = GetWorld()->GetAuthGameMode<ASDMGameMode>();
	ASGameState* GS = GetWorld()->GetGameState<ASGameState>();

	if (GM)
	{
		
		//TeamName = GM->AssignTeamToPlayer(this);
		//GM->SpawnPlayer(this->GetOwner(), this);
		
	}
	
	

	if (Role == ROLE_Authority)
	{
		
		
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		
		

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachedSocketName);
		}

		//GM->SpawnPlayer(Owner, this);
	}
	

}

void ASCharacter::MoveForward(float axis)
{
	AddMovementInput(GetActorForwardVector()* axis);
}

void ASCharacter::MoveRight(float axis)
{
	AddMovementInput(GetActorRightVector()* axis);
}

//initalizing variable to help with start up 
void ASCharacter::InitailizeVariables()
{
	//when creating a default sub object, it will need a unique name
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	FlashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLightComp"));
	FlashLight->SetupAttachment(CameraComp);
	//FlashLight->SetHiddenInGame(true);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	ZoomFOV = 65.0f;
	ZoomInterSpeed = 20.0f;

	WeaponAttachedSocketName = "WeaponSocket";

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	//TeamName = FString(TEXT("Empty"));
	
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::StartJump()
{
	Jump();
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::Fire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::StartFlash()
{

	if (bFlash)
	{
		bFlash = false;
	}
	else
	{
		bFlash = true;
	}

	ClientRPC_ToggleFlash();
}

void ASCharacter::StartGame()
{
	if (GM)
	{

		TeamName = GM->AssignTeamToPlayer(this);
		GM->SpawnPlayer(this->GetOwner(), this);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SPAWN")));

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("NO SPAWN")));
	}
}

bool ASCharacter::Server_ToggleFlash_Validate(bool flash)
{
	if (FlashLight == nullptr)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ASCharacter::Server_ToggleFlash_Implementation(bool flash)
{
	
	if (flash)
	{
		flash = false;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Flash: %f"), bFlash));
	}
	else
	{
		flash = true;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Flash: %f"), bFlash));
	}
	Multicast_ToggleFlash(flash);

}

void ASCharacter::ClientRPC_ToggleFlash_Implementation()
{

	Server_ToggleFlash(bFlash);
}

void ASCharacter::Multicast_ToggleFlash_Implementation(bool flash)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Flash: x: %f"), bFlash));

	FlashLight->SetHiddenInGame(flash);

}



void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		//this needs rework
		if (ASCharacter* playerDealtDamage = Cast<ASCharacter>(DamageCauser->GetOwner()))
		{
			if (playerDealtDamage)
			{
				ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
				if (GS)
				{
					if (playerDealtDamage->TeamName != TeamName)
					{
						if (playerDealtDamage->TeamName == "Blue")
						{
							GS->BlueTeamScore += 10;

						}
						else
						{
							GS->RedTeamScore += 10;

						}
					}

				}

			}
			else
			{

			}
		}
		else
		{
			ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
			if (GS)
			{
				//GS->BlueTeamCount += 10;
				GS->BlueTeamScore += 10;
				ASDMGameMode* GM = GetWorld()->GetAuthGameMode<ASDMGameMode>();
				if (GM)
				{
					GM->ChangeTeam();
				}
			}
			
		}
		

		

		//Die!
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//DetachFromControllerPendingDestroy();
		//SetLifeSpan(10.0f);

		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASCharacter::Respawn, 2.0f, false);
	}
}

void ASCharacter::Respawn()
{
	bDied = false;
	HealthComp->Respawn();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ASDMGameMode* GM = GetWorld()->GetAuthGameMode<ASDMGameMode>();
	GM->SpawnDemon(this);
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomFOV : DefaultFOV;

	float newFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterSpeed);

	CameraComp->SetFieldOfView(newFOV);

	

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::StartJump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);

	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Flash", IE_Pressed, this, &ASCharacter::StartFlash);
	//PlayerInputComponent->BindAction("Flash", IE_Released, this, &ASCharacter::EndFlash);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::DestroyWeapon()
{
	CurrentWeapon->Destroy();
}

void ASCharacter::Multicast_PlaySound_Implementation(FVector Location, USoundCue * Sound, float Volume)
{
	if (Sound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Volume, 1.0f, 0.0f, SoundSettings, nullptr);
	}

}

void ASCharacter::Server_PlaySound_Implementation(FVector Location, USoundCue * Sound, float Volume)
{
	Multicast_PlaySound(Location, Sound, Volume);
}



void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDied); 
	DOREPLIFETIME(ASCharacter, PlayerReady);
}