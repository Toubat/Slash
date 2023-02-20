// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"


ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

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

FName ASlashCharacter::GetHandSocketName(const ECharacterState State) const
{
	switch (State) {
	case ECharacterState::ECS_EquippedOneHandedWeapon:
		return FName("RightHandSocket");
	case ECharacterState::ECS_EquippedTwoHandedWeapon:
		return FName("TwoHandedSocket");
	default:
		return FName("RightHandSocket");
	}
}

FName ASlashCharacter::GetSpineSocketName(const ECharacterState State) const
{
	switch (State) {
	case ECharacterState::ECS_EquippedOneHandedWeapon:
		return FName("SpineSocket");
	case ECharacterState::ECS_EquippedTwoHandedWeapon:
		return FName("SpineSocketAxe");
	default:
		return FName("SpineSocket");
	}
}

void ASlashCharacter::SetWeaponCollisionEnabled(const ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoredActors.Empty();
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
		OverlappingWeapon->Equip(GetMesh(), GetHandSocketName(CharacterState), this, this);
	} 
}

void ASlashCharacter::PlayAttackMontage() const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName;

		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		default:
			break;
		}

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
	if (EquippedWeapon) EquippedWeapon->AttachToSocket(GetMesh(), GetHandSocketName(EquippedWeapon->GetEquipState()));
}

void ASlashCharacter::OnWeaponDisarm()
{
	if (EquippedWeapon) EquippedWeapon->AttachToSocket(GetMesh(), GetSpineSocketName(EquippedWeapon->GetEquipState()));
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

