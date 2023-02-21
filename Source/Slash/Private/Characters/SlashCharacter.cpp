// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"


ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("SlashCharacter"));
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (!GetController()) return;
		
	// const FVector Forward = GetActorForwardVector();
	// const FVector Right = GetActorRightVector();

	// AddMovementInput(Forward, MovementVector.X);
	// AddMovementInput(Right, MovementVector.Y);

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void ASlashCharacter::Jump(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	Super::Jump();
}

void ASlashCharacter::Attack(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (CharacterState == ECharacterState::ECS_Unequipped) return;
	
	ActionState = EActionState::EAS_Attacking;
	PlayAttackMontage();
}

void ASlashCharacter::Equip(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	const bool CanDisarm = EquippedWeapon && EquipMontage && CharacterState != ECharacterState::ECS_Unequipped;
	const bool CanArm = EquippedWeapon && EquipMontage && CharacterState == ECharacterState::ECS_Unequipped;

	if (CanDisarm) {
		CharacterState = ECharacterState::ECS_Unequipped;
		ActionState = EActionState::EAS_Equipping;
		PlayEquipMontage(FName("Unequip"));
	} else if (CanArm) {
		PlayEquipMontage(FName("Equip"));
		CharacterState = EquippedWeapon->GetEquipState();
		ActionState = EActionState::EAS_Equipping;
	} else if (AWeapon* OverlappingWeapon = Cast<AWeapon>(GetOverlappingItem())) {
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
		CharacterState = EquippedWeapon->GetEquipState();
		OverlappingWeapon->Equip(GetMesh(), "RightHandSocket", this, this);
	} 
}

void ASlashCharacter::PlayAttackMontage() const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UAnimMontage* AttackMontage = EquippedWeapon->GetEquipState() == ECharacterState::ECS_EquippedOneHandedWeapon ? AttackMontage_OneHand : AttackMontage_TwoHand;
	TArray<FName> Sections = { TEXT("Attack1"), TEXT("Attack2") };
	
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const FName SectionName = Sections[FMath::RandRange(0, Sections.Num() - 1)];
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::OnAttackMontageEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::OnEquipMontageEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::OnUnEquipMontageEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::OnWeaponArm()
{
	if (EquippedWeapon) EquippedWeapon->AttachToSocket(GetMesh(), "RightHandSocket");
}

void ASlashCharacter::OnWeaponDisarm()
{
	if (EquippedWeapon) EquippedWeapon->AttachToSocket(GetMesh(), "SpineSocket");
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
	}
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);

	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	if (HitParticles) UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint);
}

