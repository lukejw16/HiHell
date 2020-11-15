// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DemonCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USHealthComponent;
class UDamageType;
class USceneComponent;
class UParticleSystemComponent;
class UMaterial;
class USoundCue;
class USoundAttenuation;


USTRUCT()
struct FHitTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
		FVector_NetQuantize TraceEnd;
};

UCLASS()
class LIGHTSOUT_API ADemonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemonCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float axis);

	void MoveRight(float axis);

	void StartJump();

	void StartAttack();

	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComp")
		USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* RightHand;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void AttackTrace();

	UFUNCTION()
		void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		bool Attack;

	UFUNCTION(Client, Reliable)
		void ClientRPC_Attack(bool bAttack);

	UFUNCTION(Server, Reliable)
		void Server_Attack(bool bAttack);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Death")
	bool bDied;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Material")
	float TextureOpacity;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Material")
	float RefractionOpacity;

	FTimerHandle RespawnTimerHandle;

	void Respawn();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Material")
	float turningVisible;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Material")
	float turningInvisible;

	UPROPERTY(ReplicatedUsing = OnRep_HitTrace)
		FHitTrace HitScanTrace;

	UFUNCTION()
		void OnRep_HitTrace();

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* GruntSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* AppearSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* DisappearSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundAttenuation* SoundSettings;

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_PlaySound(FVector Location, USoundCue* Sound);

	UFUNCTION(Server, BlueprintCallable, Reliable)
		void Server_PlaySound(FVector Location, USoundCue* Sound);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* DemonAppear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* DemonMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* InvisableMaterial;

	bool bDoOnce;
	
};
