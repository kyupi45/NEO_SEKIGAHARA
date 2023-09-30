// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "OdaShockWave.generated.h"

UCLASS()
class NEO_API AOdaShockWave : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOdaShockWave();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//プレイヤーが右側にいるか
	bool IsRightPlayer;

	//エフェクトを動かす処理
	void MoveWave();

	//NiagaraSystemを作る為
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* Wave;

	UPROPERTY(EditAnywhere)
		class UNiagaraComponent* NiagaraComp;

	//エフェクトの大きさを決める
	UPROPERTY(EditAnywhere)
	FVector Scale;


};
