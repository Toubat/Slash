// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Containers/Deque.h"
#include "SlashCharacter.generated.h"

class USlashOverlay;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USlashOverlay;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	void SetHUDHealthPercent(const float Percent);

	void SetHUDStaminaPercent(const float Percent);

	void SetHUDGoldCount(const int32 Count);

	void SetHUDSoulCount(const int32 Count);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Getters
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	// Setters
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE void SetHitReactTimer(const float Time) { GetWorldTimerManager().SetTimer(HitReactTimer, this, &ASlashCharacter::OnHitReactMontageEnd, Time); }

	FORCEINLINE void ClearHitReactTimer() { GetWorldTimerManager().ClearTimer(HitReactTimer); }

protected:
	virtual void BeginPlay() override;

	/**
	 * Callbacks for input
	 */
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);

	void Equip(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);

	/**
	 * Play Montage functions
	 */
	virtual void PlayAttackMontage() const override;

	virtual void OnAttackMontageEnd() override;

	void PlayEquipMontage(const FName& SectionName) const;

	virtual void PlayHitReactMontage(const FName& SectionName) const override;

	virtual void OnHitReactMontageEnd() override;

	virtual void PlayDeathMontage() override;

	UFUNCTION(BlueprintCallable)
	void OnDeathMontageEnd();

	UFUNCTION(BlueprintCallable)
	void OnEquipMontageEnd();

	UFUNCTION(BlueprintCallable)
	void OnUnEquipMontageEnd();

	UFUNCTION(BlueprintCallable)
	void OnWeaponArm();

	UFUNCTION(BlueprintCallable)
	void OnWeaponDisarm();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;
	
private:
	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere)
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	FTimerHandle HitReactTimer;

	USlashOverlay* SlashOverlay;
};
