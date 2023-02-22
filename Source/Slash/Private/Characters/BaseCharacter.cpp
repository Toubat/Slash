// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Slash/DebugMacros.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Attribute component
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute Component"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FName ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint) const
{
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = FVector(ImpactLowered - GetActorLocation()).GetSafeNormal();

	const FVector Front = FVector(GetActorForwardVector()).GetSafeNormal();
	const FVector Right = FVector(GetActorRightVector()).GetSafeNormal();
	const FVector Left = -Right;
	const FVector Back = -Front;

	// radian to degree
	const float FrontAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Front, ToHit)));
	const float RightAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Right, ToHit)));
	const float LeftAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Left, ToHit)));
	const float BackAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Back, ToHit)));

	FName SectionName;
	if (FrontAngle >= -45.f && FrontAngle <= 45.f) {
		SectionName = FName("FromFront");
	} else if (RightAngle >= -45.f && RightAngle <= 45.f) {
		SectionName = FName("FromRight");
	} else if (LeftAngle >= -45.f && LeftAngle <= 45.f) {
		SectionName = FName("FromLeft");
	} else if (BackAngle >= -45.f && BackAngle <= 45.f) {
		SectionName = FName("FromBack");
	}

	return SectionName;
}

void ABaseCharacter::SetWeaponCollisionEnabled(const ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoredActors.Empty();
	}
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (AttributeComponent) AttributeComponent->ReceiveDamage(DamageAmount);
	return DamageAmount;
}

void ABaseCharacter::PlayAttackMontage() const
{
}

void ABaseCharacter::OnAttackMontageEnd()
{
}

void ABaseCharacter::PlayDeathMontage()
{
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage) {
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::OnHitReactMontageEnd()
{
}

FVector ABaseCharacter::GetTranslationWrapTarget()
{
	if (CombatTarget == nullptr) return FVector::ZeroVector;

	const FVector TargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();
	FVector FromTarget = FVector(Location - TargetLocation).GetSafeNormal();
	FromTarget *= WarpTargetDistance;

	return TargetLocation + FromTarget;
}

FVector ABaseCharacter::GetRotationWrapTarget()
{
	if (CombatTarget == nullptr) return FVector::ZeroVector;
	return CombatTarget->GetActorLocation();
}
