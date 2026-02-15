// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"

#include "GamePlayHUD.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API UGamePlayHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidgetOptional))
	UProgressBar* HealthBar;

	void SetHealthBarPercent(float NewPercent);
};
