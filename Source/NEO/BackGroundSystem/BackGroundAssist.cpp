// Fill out your copyright notice in the Description page of Project Settings.


#include "BackGroundAssist.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "NEO/PlayerSystem/CharacterCamera.h"
#include "NEO/GameSystem/TGS_GameMode.h"

#define DIRECTION (90.0)

// Sets default values for this component's properties
UBackGroundAssist::UBackGroundAssist()
	: bUseBillboard(true)
{
	// Tickを毎フレーム呼び出すかどうか
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UBackGroundAssist::BeginPlay()
{
	Super::BeginPlay();


}


// 毎フレーム呼び出す処理
void UBackGroundAssist::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 常にカメラを向く
	ToFaceCamera();
}


/*
 * 関数名　　　　：ToFaceCamera()
 * 処理内容　　　：常にカメラに向ける処理
 * 戻り値　　　　：なし
 */
void UBackGroundAssist::ToFaceCamera()
{
	// 機能のオン・オフ
	if (!bUseBillboard)
	{
		// ビルボード処理を行わない場合Tick()を呼ばないようにする
		PrimaryComponentTick.bCanEverTick = false;

		return; 
	}

	// ゲームモード作成
	ATGS_GameMode* GameMode = Cast<ATGS_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) { return; }


	// カメラの現在位置と角度を取得
	const FVector CameraLocation = GameMode->GetCameraLocation();


	// カメラの位置が変わっていれば新しい方向を計算
	if (BeforeCameraPos != CameraLocation)
	{
		// カメラに向ける計算
		FVector ToCamera = GetOwner()->GetActorLocation() - GameMode->GetCameraLocation();
		ToCamera.Y = 0;
		ToCamera.Z = 0;

		FRotator LookAtRotation = ToCamera.Rotation();
		GetOwner()->SetActorRotation(FRotator(LookAtRotation.Pitch, LookAtRotation.Yaw, LookAtRotation.Roll));
	}
}