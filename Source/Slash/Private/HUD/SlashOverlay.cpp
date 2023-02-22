// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"

#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthPercent(float Percent)
{
	if (HealthProgressBar) HealthProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetStaminaPercent(float Percent)
{
	if (StaminaProgressBar) StaminaProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetGoldCount(int32 Count)
{
	if (GoldCountText) GoldCountText->SetText(FText::FromString(FString::FromInt(Count)));
}

void USlashOverlay::SetSoulCount(int32 Count)
{
	if (SoulCountText) SoulCountText->SetText(FText::FromString(FString::FromInt(Count)));
}

void USlashOverlay::SetDieBorderVisibility(bool bVisible)
{
	if (DieBorder) DieBorder->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
