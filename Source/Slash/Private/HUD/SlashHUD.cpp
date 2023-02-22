// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() && GetWorld()->GetFirstPlayerController() && SlashOverlayClass)
	{
		SlashOverlay = CreateWidget<USlashOverlay>(GetWorld()->GetFirstPlayerController(), SlashOverlayClass);
		SlashOverlay->AddToViewport();
	}
}
