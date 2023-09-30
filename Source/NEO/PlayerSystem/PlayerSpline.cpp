// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSpline.h"
#include "Components/SplineComponent.h"

// Sets default values
APlayerSpline::APlayerSpline()
{
 	// Tick()を毎フレーム呼ばないようにする
	PrimaryActorTick.bCanEverTick = false;

	// "PlayerLoad"タグ追加
	Tags.Add("PlayerLoad");

	// スプラインコンポーネント作成
	SplineComp = CreateDefaultSubobject<USplineComponent>("PlayerLoad");
}

// Called when the game starts or when spawned
void APlayerSpline::BeginPlay()
{
	Super::BeginPlay();
	
	// 全体の長さ取得
	AllLength = SplineComp->GetSplineLength();
}

// Called every frame
void APlayerSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*
 * 関数名　　　　：GetSplineAngle()
 * 処理内容　　　：スプラインの角度参照
 * 引数１　　　　：float _distance・・・距離
 * 戻り値　　　　：その地点での角度
 */
FRotator APlayerSpline::GetSplineAngle(float _distance)
{
	if (SplineComp)
	{
		// プレイヤーの現在の移動距離を%で取得
		float m_localLength = (float)((int)_distance % (int)AllLength);/*FMath::Fmod(_distance,AllLength);*/

		//現在のスプラインの位置に合わせた回転情報の値を参照で返す
		FRotator SplineAngle = SplineComp->GetRotationAtDistanceAlongSpline(m_localLength, ESplineCoordinateSpace::World);

		return SplineAngle;
	}

	return FRotator(0.0,0.0,0.0);
}


