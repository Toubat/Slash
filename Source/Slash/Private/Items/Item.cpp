// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh component
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

}

float AItem::TransformSin()
{
	return Amplitude * FMath::Sin(Frequency * RunningTime);
}

float AItem::TransformCos()
{
	return Amplitude * FMath::Cos(Frequency * RunningTime);
}

template<typename T>
inline T AItem::Avg(T first, T second)
{
	return (first + second) / 2;
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