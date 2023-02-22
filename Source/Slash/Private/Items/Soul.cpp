// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "NiagaraFunctionLibrary.h"
#include "Interfaces/PickupInterface.h"

void ASoul::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (const auto PickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		PickupInterface->AddSouls(this);
		if (PickupEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
		SpawnPickupSound();
		Destroy();
	}
}
