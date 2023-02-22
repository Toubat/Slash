// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class UHealthBarComponent;
class UAnimMontage;
class USoundBase;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	void HideHealthBar();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void ShowHealthBar();

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Destroyed() override;
	
protected:
	virtual void BeginPlay() override;

	void Attack();

	/**
	 * Play montage functions
	 */
	virtual void PlayDeathMontage() override;

	virtual void PlayAttackMontage() const override;

	virtual void OnAttackMontageEnd() override;

	virtual void OnHitReactMontageEnd() override;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	bool InTargetRange(const AActor* Target, const double Radius) const;

	void MoveToTarget(const AActor* Target) const;

	void ChoosePatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
private:
	void PatrolTimerFinished() const;

	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1500.f;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 250.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	/**
	 * Navigation
	 */
	UPROPERTY()
	class AAIController* EnemyController;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;
};
