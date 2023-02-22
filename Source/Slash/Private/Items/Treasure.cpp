// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"

#include "Interfaces/PickupInterface.h"


void ATreasure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const FRotator NewRotation(0.0, DeltaTime * 45.0, 0.0);	
	AddActorWorldRotation(NewRotation);
}

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const auto PickupInterface = Cast<IPickupInterface>(OtherActor)) {
		PickupInterface->AddGold(this);
		SpawnPickupSound();
		Destroy();
	}
}
