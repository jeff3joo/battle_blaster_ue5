// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayHUD.h"

void UGamePlayHUD::SetHealthBarPercent(float NewPercent)
{
	if (NewPercent >= 0.0f && NewPercent <= 1.0f)
	{
		HealthBar->SetPercent(NewPercent);
	}
}