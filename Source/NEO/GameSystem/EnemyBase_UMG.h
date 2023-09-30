// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyBase_UMG.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API UEnemyBase_UMG : public UUserWidget
{
	GENERATED_BODY()
public:
	//virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(Meta = (BindWidget))
		class UCanvasPanel* Canvas;

	//HPÉoÅ[
	UPROPERTY(Meta = (BindWidget))
		class UProgressBar* HealthBar;

protected:
	//-------------------------------
	//widgetValue
	float fHP = 0.f;
	float fMaxHP = 0.f;

	FName Name = FName("None");

public:

	void SetPlayerInfo(float HP, float MaxHP);

	//GetHPÅì
	inline float GetHPPercent() {
		return fHP / fMaxHP;
	}

};
