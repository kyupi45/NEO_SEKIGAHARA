// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpModuleActor.h"


// Sets default values
AJumpModuleActor::AJumpModuleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsJumping = false;

	//重力加速度
	fGravityAcceleration = 9.8f;

	//ジャンプ時間
	fJumpTime = 0.0f;

	//ジャンプ高さ
	fJumpHeight = 100.0f;

	//ジャンプ開始時の位置
	vJumpStartLocation = GetActorLocation();

}

// Called when the game starts or when spawned
void AJumpModuleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJumpModuleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ジャンプ処理
	JumpingByGravity(DeltaTime);
}

/**
 * 重力ジャンプを開始する
 * @param JumpHeight ジャンプする高さ
 * @param GravityAcceleration 重力加速度
 */
void AJumpModuleActor::StartJumpByGravity(float JumpHeight, float GravityAcceleration)
{
	if (!bIsJumping)
	{
		//ジャンプを開始
		fJumpTime = 0.0f;
		fJumpHeight = JumpHeight;
		fGravityAcceleration = GravityAcceleration;

		bIsJumping = true;
	}
}

/**
 * 重力によるジャンプを実行する
 * @param DeltaTime 経過時間
 */
void AJumpModuleActor::JumpingByGravity(float DeltaTime)
{
	if (bIsJumping)
	{
		//ジャンプ中
		fJumpTime += DeltaTime;

		//ジャンプの移動距離を計算
		float fCurrentJumpHeight = fJumpHeight * FMath::Sin(fJumpTime) - 0.5f * fGravityAcceleration * FMath::Pow(fJumpTime, 2.0f);

		//キャラクターに移動距離を反映
		FVector vCurrentLocation = GetActorLocation();
		vCurrentLocation.Z = vJumpStartLocation.Z + fCurrentJumpHeight;
		SetActorLocation(vCurrentLocation);

		//ジャンプを終了するかどうかを判断
		if (fCurrentJumpHeight <= 0.0f)
		{
			EndJumpByGravity();
		}

	}
}

/**
 * 重力ジャンプを終了する
 */
void AJumpModuleActor::EndJumpByGravity()
{
	//ジャンプを終了
	bIsJumping = false;

	//ジャンプ時間をリセット
	fJumpTime = 0.0f;

	//ジャンプ高さをリセット
	fJumpHeight = 0.0f;

	//重力加速度をリセット
	fGravityAcceleration = 0.0f;
}
