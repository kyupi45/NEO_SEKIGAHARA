// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase_UMG.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEnemyBase_UMG::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HealthBar)
	{
		HealthBar->SetPercent(GetHPPercent());
	}
}

void UEnemyBase_UMG::SetPlayerInfo(float HP, float MaxHP)
{
	fHP = HP;
	fMaxHP = MaxHP;
}
