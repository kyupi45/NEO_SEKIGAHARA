// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_UMG.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayer_UMG::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HealthBar) {
		HealthBar->SetPercent(GetHPPercent());
	}
}
