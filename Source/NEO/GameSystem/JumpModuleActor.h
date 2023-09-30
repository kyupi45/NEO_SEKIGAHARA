// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JumpModuleActor.generated.h"

UCLASS()
class NEO_API AJumpModuleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJumpModuleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//重力加速度的跳跃实现

	//重力加速度的跳跃实现所需要的变量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		float fJumpHeight;					//ジャンプ高さ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		float fGravityAcceleration;			//重力加速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		float fJumpTime;					//ジャンプ時間

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		bool bIsJumping;					//ジャンプ中かどうか

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FVector vJumpStartLocation;				//ジャンプ開始時の位置

	//ジャンプの開始
	UFUNCTION(BlueprintCallable, Category = "Jump")
		void StartJumpByGravity(float JumpHeight = 1500.f, float GravityAcceleration = 2000.f);

	//ジャンプ中
	UFUNCTION(BlueprintCallable, Category = "Jump")
		void JumpingByGravity(float DeltaTime);

	//ジャンプの終了
	UFUNCTION(BlueprintCallable, Category = "Jump")
		void EndJumpByGravity();

};
