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

public:
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }

	FORCEINLINE bool IsAlive() const { return Health > 0.f; }

	void ReceiveDamage(float Damage);
};
