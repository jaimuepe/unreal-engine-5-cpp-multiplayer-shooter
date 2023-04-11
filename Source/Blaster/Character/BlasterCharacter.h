// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	virtual void OnRep_PlayerState() override;

protected:
	virtual void BeginPlay() override;
	virtual void Jump() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void Turn(float Value);
	void LookUp(float Value);

	void EquipButtonPressed();

	void CrouchButtonPressed();

	void AimButtonPressed();
	void AimButtonReleased();

	void AimOffset(float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	class UOverheadWidgetComponent* OverheadWidget;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon) const;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	/**
	 * @brief Use this property instead of bUseControllerRotationYaw because that one is not replicated.
	 */
	UPROPERTY(ReplicatedUsing = OnRep_UseControllerRotationYaw)
	bool UseControllerRotationYaw;

	UFUNCTION()
	void OnRep_UseControllerRotationYaw(bool ControllerRotationYaw);

	float AO_Yaw;
	float InterpAO_Yaw;
	
	float AO_Pitch;

	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

public:
	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped() const;

	bool IsAiming() const;

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon() const;
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; };
};
