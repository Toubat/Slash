// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
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

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunningTime += DeltaTime; 
	
	// float DeltaZ = Amplitude * FMath::Sin(Frequency * RunningTime);
	// AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	DRAW_SPHERE_SINGLE_FRAME(GetActorLocation());
	DRAW_VECTOR_SINGLE_FRAME(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);
}