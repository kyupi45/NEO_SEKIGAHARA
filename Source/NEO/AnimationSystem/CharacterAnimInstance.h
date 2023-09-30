// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

UCLASS()
class NEO_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:

	// 開始時処理
	virtual void NativeBeginPlay() override;

	// 毎フレーム処理(Tick) 
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	// オーナーをキャラクタークラスで取得
	ACharacter* OwnerCharacter;

	// オーナーのMovementComponent保存用
	class UCharacterMovementComponent* CharacterMovementComp;

	// 
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Owner Status")
		FVector Velocity;

	//歩行速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner Status")
		float GroundSpeed;

	// 動いているか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner Status")
		bool ShouldMove;

	// 空中にいる状態かどうか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner Status")
		bool IsFall;
};
