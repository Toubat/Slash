// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthPercent(float Percent);

	void SetStaminaPercent(float Percent);

	void SetGoldCount(int32 Count);

	void SetSoulCount(int32 Count);

	void SetDieBorderVisibility(bool bVisible);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulCountText;

	// border
	UPROPERTY(meta = (BindWidget))
	class UBorder* DieBorder;
};
