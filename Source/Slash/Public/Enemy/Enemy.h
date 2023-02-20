// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class UHealthBarComponent;
class UAttributeComponent;
class UAnimMontage;
class USoundBase;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	virtual void BeginPlay() override;

	/**
	 * Play montage functions
	 */
	UFUNCTION()
	void PlayHitReactMontage(const FName& SectionName) const;

	UFUNCTION()
	void PlayDeathMontage();

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	bool InTargetRange(const AActor* Target, const double Radius) const;

	void MoveToTarget(const AActor* Target) const;

	void ChoosePatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;
	
private:
	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	
	/**
	 * Animation Montages
	 */
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	UPROPERTY(VisibleAnywhere)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	/**
	 * Navigation
	 */
	UPROPERTY()
	class AAIController* EnemyController;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	FTimerHandle PatrolTimer;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	void PatrolTimerFinished() const;
};
