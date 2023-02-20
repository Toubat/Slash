// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "AIController.h"
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
#include "Perception/PawnSensingComponent.h"
#include "Slash/DebugMacros.h"

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

	// Widget component
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// Pawn Sensing component
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SetPeripheralVisionAngle(75.f);
	PawnSensing->SightRadius = 500.f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 125.f;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget) {
		if (InTargetRange(CombatTarget, AttackRadius)) {
			// Inside attack radius, attack player
			if (EnemyState == EEnemyState::EES_Attacking) return;

			EnemyState = EEnemyState::EES_Attacking;
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			// TODO: Attack...
		} else if (InTargetRange(CombatTarget, CombatRadius)) {
			// Inside combat radius, but outside attack radius, chase player
			if (EnemyState == EEnemyState::EES_Chasing) return;

			EnemyState = EEnemyState::EES_Chasing;
			GetCharacterMovement()->MaxWalkSpeed = 300.f;
			MoveToTarget(CombatTarget);
		} else {
			// Outside combat radius, lose interest
			if (EnemyState == EEnemyState::EES_Patrolling) return;

			EnemyState = EEnemyState::EES_Patrolling;
			CombatTarget = nullptr;
			if (HealthBarWidget) HealthBarWidget->SetVisibility(false);
			GetCharacterMovement()->MaxWalkSpeed = 125.f;
			ChoosePatrolTarget();
			MoveToTarget(PatrolTarget);
		}
	} else if (!PatrolTarget || InTargetRange(PatrolTarget, PatrolRadius)) {
		// choose next patrol target if inside acceptable radius of current target
		ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, 3.f);
	}
}

void AEnemy::MoveToTarget(const AActor* Target) const
{
	if (EnemyController && Target) {
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(15.f);
		EnemyController->MoveTo(MoveRequest);
	}
}

void AEnemy::ChoosePatrolTarget()
{
	if (PatrolTargets.Num() > 0) {
		const int32 RandomIndex = FMath::RandRange(0, PatrolTargets.Num() - 1);
		PatrolTarget = PatrolTargets[RandomIndex];
	}
}

void AEnemy::PatrolTimerFinished() const
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidget) {
		HealthBarWidget->SetHealthPercent(AttributeComponent->GetHealthPercent());
		HealthBarWidget->SetVisibility(false);
	}

	EnemyController = Cast<AAIController>(GetController());
	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);
}

void AEnemy::PlayDeathMontage()
{
	TArray<FName> Sections = { TEXT("Death1"), TEXT("Death2"), TEXT("Death3"), TEXT("Death4"), TEXT("Death5")};
	TArray<EDeathPose> DeathPoses = {EDeathPose::EDP_Death_1, EDeathPose::EDP_Death_2, EDeathPose::EDP_Death_3, EDeathPose::EDP_Death_4, EDeathPose::EDP_Death_5};

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && DeathMontage) {
		const int32 Idx = FMath::RandRange(0, Sections.Num() - 1);
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(Sections[Idx], DeathMontage);
		DeathPose = DeathPoses[Idx];
	}
}

void AEnemy::OnPawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Chasing) return;
	if (EnemyState == EEnemyState::EES_Attacking) return;
	
	if (SeenPawn->ActorHasTag(FName("SlashCharacter"))) {
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		CombatTarget = SeenPawn;
	}
}

bool AEnemy::InTargetRange(const AActor* Target, const double Radius) const
{
	if (Target == nullptr) return false;

	const double DistToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	DRAW_SPHERE_SINGLE_FRAME(Target->GetActorLocation());
	DRAW_SPHERE_SINGLE_FRAME(GetActorLocation());
	return DistToTarget <= Radius;
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	// DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);
	if (HealthBarWidget) HealthBarWidget->SetVisibility(true);

	const FName SectionName = DirectionalHitReact(ImpactPoint);

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

	if (EventInstigator->GetPawn()->ActorHasTag("SlashCharacter")) {
		CombatTarget = EventInstigator->GetPawn();
	}
	
	return DamageAmount;
}

