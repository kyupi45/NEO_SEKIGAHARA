// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player_UMG.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API UPlayer_UMG : public UUserWidget
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

	//Skill âÊëú
	UPROPERTY(Meta = (BindWidget))
		class UImage* SkillImage;

//-------------------------------
	//widgetValue
	float fPlayerHP = 0.f;
	float fPlayerHPMaxHP = 0.f;

	FName PlayerHPName = FName("None");

	void SetPlayerInfo(float& HP, float& MaxHP) {
		fPlayerHP = HP;
		fPlayerHPMaxHP = MaxHP;
	}

	//GetHPÅì
	float GetHPPercent() {
		return fPlayerHP / fPlayerHPMaxHP;
	}
};
