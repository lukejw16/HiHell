// Fill out your copyright notice in the Description page of Project Settings.


#include "DemonCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "SDMGameMode.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "LightsOut.h"
#include "SGameState.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/SceneComponent.h"
#include "SCharacter.h"




// Sets default values
ADemonCharacter::ADemonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	bReplicates = true;
	Attack = false;

	FName SocketName = "RightHand";

	RightHand =  CreateDefaultSubobject<USceneComponent>(TEXT("RightHandComp"));
	RightHand->SetupAttachment(GetMesh(), SocketName);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ADemonCharacter::OnHealthChanged);

	

	//start Invisible
	TextureOpacity = 0.01f;
	RefractionOpacity = 1.0f;

	turningVisible = 0.005f;

	turningInvisible = 0.1f;

}

// Called when the game starts or when spawned
void ADemonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADemonCharacter::MoveForward(float axis)
{
	if (!bDied)
	{
		AddMovementInput(GetActorForwardVector()* axis);
	}
	
}

void ADemonCharacter::MoveRight(float axis)
{
	if (!bDied)
	{
		AddMovementInput(GetActorRightVector()* axis);
	}
	
}

void ADemonCharacter::StartJump()
{
	if (!bDied)
	{
		Jump();
	}
	
}

void ADemonCharacter::StartAttack()
{
	if (!Attack && !bDied)
	{
		ClientRPC_Attack(true);
	}
	
}

void ADemonCharacter::AttackTrace()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		//MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);



		FVector ShotDirection = GetActorLocation();

		FVector TraceEnd = GetActorLocation() + (GetActorForwardVector() * 150);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, ShotDirection, TraceEnd, ECollisionChannel::ECC_Pawn, QueryParams))
		{
			// hit something process damage

			AActor* HitActor = Hit.GetActor();
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			//this will need to change

			ASCharacter* hitCharacter = Cast<ASCharacter>(HitActor);
			ADemonCharacter* MyCharacter = Cast<ADemonCharacter>(MyOwner);
			ASGameState* GS = GetWorld()->GetGameState<ASGameState>();

			float ActualDamage = 40.0f;

			if (SurfaceType == SURFACE_FLESHVUNERABLE)
			{
				ActualDamage *= 4.0f;
			}


			if (hitCharacter)
			{

				if (GS)
				{
					if (!GS->EndGame)
					{
						UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageTypes);
					}
				}




			}



			TracerEndPoint = Hit.ImpactPoint;


		}


		//DrawDebugLine(GetWorld(), ShotDirection, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);





		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceEnd = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}
	}
}



// Called every frame
void ADemonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetVelocity().Size() > 300)
	{
		//setting of values for turning visible
		TextureOpacity = FMath::Clamp(TextureOpacity += turningVisible, 0.01f, 1.0f);
		GetMesh()->SetScalarParameterValueOnMaterials(FName("TextureOpacity"), TextureOpacity); //new value


		RefractionOpacity = FMath::Clamp(RefractionOpacity -= turningVisible, 0.0f, 1.0f);
		GetMesh()->SetScalarParameterValueOnMaterials(FName("RefractionOpacity"), RefractionOpacity); //new value
		

		if (bDoOnce != true && TextureOpacity >= 1.0f && RefractionOpacity <= 0.0f)
		{
			if (InvisableMaterial != nullptr)
			{
				GetMesh()->SetMaterial(0, DemonMaterial);
			}

			UGameplayStatics::SpawnEmitterAttached(DemonAppear, RootComponent, NAME_None, GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);

			Server_PlaySound(GetActorLocation(), AppearSound);

			bDoOnce = true;
		}

		
	}
	else
	{

		if (bDoOnce)
		{
			Server_PlaySound(GetActorLocation(), DisappearSound);
		}
		//setting values for turning invisiable
		if (DemonMaterial != nullptr)
		{
			GetMesh()->SetMaterial(0, InvisableMaterial);
		}

		TextureOpacity = FMath::Clamp(TextureOpacity -= turningInvisible, 0.01f, 1.0f);
		GetMesh()->SetScalarParameterValueOnMaterials(FName("TextureOpacity"), TextureOpacity); //new value
		

		RefractionOpacity = FMath::Clamp(RefractionOpacity += turningInvisible, 0.0f, 1.0f);
		GetMesh()->SetScalarParameterValueOnMaterials(FName("RefractionOpacity"), RefractionOpacity);
		

		bDoOnce = false;
	}

}

// Called to bind functionality to input
void ADemonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!bDied)
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &ADemonCharacter::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &ADemonCharacter::MoveRight);
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADemonCharacter::StartJump);
		PlayerInputComponent->BindAction("Jump", IE_Released, this, &ADemonCharacter::StopJumping);

		PlayerInputComponent->BindAxis("LookUp", this, &ADemonCharacter::AddControllerPitchInput);
		PlayerInputComponent->BindAxis("Turn", this, &ADemonCharacter::AddControllerYawInput);

		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADemonCharacter::StartAttack);
	}


}

void ADemonCharacter::OnHealthChanged(USHealthComponent * OwningHealthComp, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health changed")));

	if (Health <= 0.0f && !bDied)
	{
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Respawn function

		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ADemonCharacter::Respawn, 2.0f, false);
	}
}

void ADemonCharacter::Respawn()
{
	bDied = false;
	HealthComp->Respawn();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
}

void ADemonCharacter::OnRep_HitTrace()
{

}

void ADemonCharacter::Multicast_PlaySound_Implementation(FVector Location, USoundCue * Sound)
{
	if (Sound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, 1.0f, 1.0f, 0.0f, SoundSettings, nullptr);
	}
	
}

void ADemonCharacter::Server_PlaySound_Implementation(FVector Location, USoundCue * Sound)
{
	Multicast_PlaySound(Location, Sound);
}

void ADemonCharacter::Server_Attack_Implementation(bool bAttack)
{
	
	Attack = bAttack;
	//remake this as function
	//Multicast_PlaySound(GetActorLocation(), AttackSound);
	Multicast_PlaySound(GetActorLocation(), GruntSound);
	
}

void ADemonCharacter::ClientRPC_Attack_Implementation(bool bAttack)
{
	//call the server
	Server_Attack(bAttack);
	//on server set variables
}

void ADemonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADemonCharacter, Attack);
	DOREPLIFETIME(ADemonCharacter, TextureOpacity);
	DOREPLIFETIME(ADemonCharacter, RefractionOpacity);
	DOREPLIFETIME(ADemonCharacter, turningVisible);
	DOREPLIFETIME(ADemonCharacter, turningInvisible);
	DOREPLIFETIME(ADemonCharacter, bDied);
	DOREPLIFETIME_CONDITION(ADemonCharacter, HitScanTrace, COND_SkipOwner);
}

