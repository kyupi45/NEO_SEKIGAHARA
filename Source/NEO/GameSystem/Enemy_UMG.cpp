// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_UMG.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "NEO/Enemy/EnamyBase.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UEnemy_UMG::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEnemy_UMG::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	fEnemyHP = GetEnemyHP();

	if (HealthBar)	{
		HealthBar->SetPercent(GetHPPercent());
	}

	if(Text){
		Text->SetText(FText::FromName(EnemyName));
	}
}

float UEnemy_UMG::GetEnemyHP()
{
	//Cast Enemy
	AEnamyBase* Enemy = Cast<AEnamyBase>(GetOwningPlayerPawn());
	if (Enemy)	{
		return Enemy->GetHP();
	}
	else	{
		//Log
		UE_LOG(LogTemp, Warning, TEXT("Enemy_UMG::GetEnemyHP() : Enemy is nullptr"));
		return 0.0f;

	}
}

void UEnemy_UMG::SetFaceCamera()
{
//Cast Enemy
	AEnamyBase* Enemy = Cast<AEnamyBase>(GetOwningPlayerPawn());
	if (Enemy)	{
		FVector EnemyLocation = Enemy->GetActorLocation();
	}
	else	{
		//Log
		UE_LOG(LogTemp, Warning, TEXT("Enemy_UMG::SetFaceCamera() : Enemy is nullptr"));
	}
}
