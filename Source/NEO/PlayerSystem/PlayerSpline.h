// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSpline.generated.h"

UCLASS()
class NEO_API APlayerSpline : public AActor
{
	GENERATED_BODY()
	
private:

	// コンストラクタ
	APlayerSpline();

	// ゲーム開始時に呼び出される処理
	virtual void BeginPlay() override;

	// 毎フレーム呼び出される処理
	virtual void Tick(float DeltaTime) override;

public:

	// スプラインの角度参照
	FRotator GetSplineAngle(float _distance);

private:

	// スプラインコンポーネント
	class USplineComponent* SplineComp;

	// スプライン全体の長さ
	float AllLength;
};
