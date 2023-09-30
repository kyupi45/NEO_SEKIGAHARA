// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Gun.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API AGun : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// 当たり判定
	void SetCollision();

	// プレイヤーの当たり判定
	void PlyerAttack();

	// 敵の当たり判定
	void EnemyAttack();

	// 敵に攻撃した時の処理
	void AttackEnemy(AActor* _enemy, float _damageAmount, class USoundBase* _hitSoundObj);

	// 銃弾をスポーン
	void SpawnBullet();

private:

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class APlayerBullet> BulletClass;

	// 被ダメージ時のエフェクト
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
		class UNiagaraSystem* HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
		class USoundBase* ShootSoundObj;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
		class USoundBase* EnemyHitSoundObj;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
		class USoundBase* ObjectHitSoundObj;

	// 何秒間ヒットストップを起こすか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Assist", meta = (AllowPrivateAccess = "true"))
		float HitStopTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Assist",meta = (AllowPrivateAccess = "true"))
		float KnockBackDistance = -250.f;


};
