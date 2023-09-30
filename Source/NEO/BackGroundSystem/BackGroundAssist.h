// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BackGroundAssist.generated.h"


UCLASS( ClassGroup=(BackGroundAssist), meta=(BlueprintSpawnableComponent) )
class NEO_API UBackGroundAssist : public UActorComponent
{
	GENERATED_BODY()

public:	

	// コンストラクタ
	UBackGroundAssist();

protected:

	// ゲーム開始時に呼び出される処理
	virtual void BeginPlay() override;

public:	

	// 毎フレーム呼び出される処理
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	// 常にカメラの方向を向く処理
	void ToFaceCamera();

protected:

	// 機能を使うかどうか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Assist")
		bool bUseBillboard;


		
private:
	FVector BeforeCameraPos;
};
