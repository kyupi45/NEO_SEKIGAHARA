// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerNotify.generated.h"

UCLASS()
class NEO_API ATriggerNotify : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerNotify();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game starts or when spawned
	virtual void Tick(float DeltaTime) override;

	// 接触開始時に行う処理
	UFUNCTION()
		virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:

	// 当たったことを知らせる
	UFUNCTION()
		bool IsEnter() const { return IsEnterBox; }

private:

	// 判定用のボックスコンポーネント
	class UBoxComponent* pTriggerBoxComp;

	// コリジョンに当たっているか格納
	bool IsEnterBox;
};
