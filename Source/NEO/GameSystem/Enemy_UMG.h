// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enemy_UMG.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API UEnemy_UMG : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


public:
	UPROPERTY(Meta = (BindWidget))
		class UCanvasPanel* Canvas;

	//HPÉoÅ[
	UPROPERTY(Meta = (BindWidget))
		class UProgressBar* HealthBar;

	//âÊëú
	//UPROPERTY(Meta = (BindWidget))
	//	class UImage* EnemyImage;

	//Border
	UPROPERTY(Meta = (BindWidget))
		class UBorder* Border;
	
	//Text
	UPROPERTY(Meta = (BindWidget))
		class UTextBlock* Text;

//widgetValue
	float fEnemyHP = 0.f;
	float fEnemyMaxHP = 0.f;

	FName EnemyName = FName("None");

	void SetEnemyInfo(float& HP, float& MaxHP, FName& Name) {
		fEnemyHP = HP;
		fEnemyMaxHP = MaxHP;
		EnemyName = Name;
	}

	//GetHPÅì
	float GetHPPercent() {
		return fEnemyHP / fEnemyMaxHP;
	}

	float GetEnemyHP();

	//ÉJÉÅÉâÇ…å¸Ç´ÇÃê›íË
	void SetFaceCamera();
};
