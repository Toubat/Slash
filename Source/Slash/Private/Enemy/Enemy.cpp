// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Characters/SlashCharacter.h"
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Skeletal mesh component
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Capsule component
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Attribute component
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute Component"));

	// Widget component
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(AttributeComponent->GetHealthPercent());
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::PlayDeathMontage()
{
	TArray<FName> Sections = { TEXT("Death1"), TEXT("Death2"), TEXT("Death3"), TEXT("Death4"), TEXT("Death5")};
	TArray<EDeathPose> DeathPoses = {EDeathPose::EDP_Death_1, EDeathPose::EDP_Death_2, EDeathPose::EDP_Death_3, EDeathPose::EDP_Death_4, EDeathPose::EDP_Death_5};
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		const int32 Idx = FMath::RandRange(0, Sections.Num() - 1);
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(Sections[Idx], DeathMontage);
		DeathPose = DeathPoses[Idx];
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (AActor* CombatTarget : CombatTargets) {
		if (CombatTarget == nullptr) continue;
		if (!Cast<ASlashCharacter>(CombatTarget)) continue;
		
		const double DistToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
		if (DistToTarget > CombatRadius) {
			CombatTarget = nullptr;
			if (HealthBarWidget) HealthBarWidget->SetVisibility(false);
		}
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	// DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);
	if (HealthBarWidget) HealthBarWidget->SetVisibility(true);

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

	if (AttributeComponent && AttributeComponent->IsAlive()) {
		PlayHitReactMontage(SectionName);
	} else {
		PlayDeathMontage();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(5.f);
		if (HealthBarWidget) HealthBarWidget->SetVisibility(false);
	}
	
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	if (HitParticles) UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint);
	
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 60.f, 5.f, FColor::Red, 5.f, 1.f);
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f, 1.f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (AttributeComponent && HealthBarWidget) {
		AttributeComponent->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(AttributeComponent->GetHealthPercent());
	}
	
	CombatTargets.AddUnique(EventInstigator->GetPawn());
	return DamageAmount;
}

