// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
class UAttributeComponent;
class USoundBase;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

	FName DirectionalHitReact(const FVector& ImpactPoint) const;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(const ECollisionEnabled::Type CollisionEnabled);

	/**
	 * Play Montage functions
	 */
	virtual void PlayAttackMontage() const;

	UFUNCTION(BlueprintCallable)
	virtual void OnAttackMontageEnd();

	virtual void PlayDeathMontage();

	virtual void PlayHitReactMontage(const FName& SectionName) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

	/**
	 * Animation montages
	 */
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage_OneHand;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage_TwoHand;
};
