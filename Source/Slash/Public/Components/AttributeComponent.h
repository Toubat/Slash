// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health; // Current Health

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth; // Max Health

	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina; // Current Health

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina; // Max Health

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;

public:
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }

	FORCEINLINE bool IsAlive() const { return Health > 0.f; }

	FORCEINLINE float GetStaminaPercent() const { return Stamina / MaxStamina; }

	FORCEINLINE void UseStamina(const float Amount) { Stamina = FMath::Clamp(Stamina - Amount, 0.f, MaxStamina); }

	FORCEINLINE bool HasEnoughStamina() const { return Stamina > DodgeCost; }

	FORCEINLINE int32 GetGold() const { return Gold; }

	FORCEINLINE int32 GetSouls() const { return Souls; }

	FORCEINLINE void AddGold(const int32 Amount) { Gold += Amount; }

	FORCEINLINE void AddSouls(const int32 Amount) { Souls += Amount; }

	void ReceiveDamage(float Damage);
	void RegenerateStamina(float DeltaTime);
};
