// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;
class USpotLightComponent;
class USoundCue;
class UMaterial;
class USoundAttenuation;
class ASDMGameMode;


UCLASS()
class LIGHTSOUT_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	ASDMGameMode* GM;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float axis);

	void MoveRight(float axis);

	void InitailizeVariables();

	

	void BeginCrouch();

	void EndCrouch();

	void StartJump();

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterSpeed;

	float DefaultFOV;

	void BeginZoom();

	void EndZoom();

	UPROPERTY(Replicated)
	ASWeapon* CurrentWeapon;

	UPROPERTY(Replicated)
	bool bFlash = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	USpotLightComponent* FlashLight;

	void Fire();

	void StopFire();

	void StartFlash();
	UFUNCTION(BlueprintCallable)
	void StartGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundAttenuation* SoundSettings;

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_PlaySound(FVector Location, USoundCue* Sound, float Volume);

	UFUNCTION(Server, BlueprintCallable, Reliable)
		void Server_PlaySound(FVector Location, USoundCue* Sound, float Volume);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ToggleFlash();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_ToggleFlash(bool flash);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleFlash(bool flash);

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachedSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComp")
	USHealthComponent* HealthComp;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

	void Respawn();

	FTimerHandle RespawnTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArmComp;

	virtual FVector GetPawnViewLocation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamInfo")
		FString TeamName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamInfo")
	UMaterial* RedTeamMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamInfo")
	UMaterial* BlueTeamMaterial;

	void DestroyWeapon();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Lobby")
	bool PlayerReady;
	
};
