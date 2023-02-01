// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"
#include "Components/SphereComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh component
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto ActiorName = OtherActor->GetName();

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, ActiorName);
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const auto ActiorName = FString("Ending Overlap with: ") + OtherActor->GetName();

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, ActiorName);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime; 
	
	// rotate every frame
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += 100.f * DeltaTime;
	SetActorRotation(NewRotation);
}

template<typename T>
inline T AItem::Avg(T first, T second)
{
	return (first + second) / 2;
}

float AItem::TransformSin()
{
	return Amplitude * FMath::Sin(Frequency * RunningTime);
}

float AItem::TransformCos()
{
	return Amplitude * FMath::Cos(Frequency * RunningTime);
}