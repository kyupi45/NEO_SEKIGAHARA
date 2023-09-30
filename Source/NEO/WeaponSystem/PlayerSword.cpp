// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSword.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NEO/PlayerSystem/PlayerCharacter.h"
#include "NEO/Enemy/EnamyBase.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APlayerSword::APlayerSword()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	// 武器のメッシュのパス
	TCHAR* WeaponAssetPath = TEXT("/Game/0122/Player/Weapon/Sword/Sword");

	// 武器のメッシュ設定
	SetupWeaponMesh(WeaponStaticMesh, WeaponAssetPath);

	// コリジョン設定
	SetupCollisionComponent(WeaponCollision);

	// 武器の種類を刀に設定
	WeaponType = EWeaponType::WeaponType_Sword;
}

// Called when the game starts or when spawned
void APlayerSword::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*
 * 関数名　　　　：SetCollision()
 * 処理内容　　　：プレイヤーの攻撃の当たり判定
 * 戻り値　　　　：なし
 */
void APlayerSword::SetCollision()
{
	// 持っていないときはスキップ
	if (!GetIsHeld()) { return; }

	switch (OwnerType)
	{
	case EOwnerType::OwnerType_Player:
		PlyerAttack();
		break;
	case EOwnerType::OwnerType_Enemy:
		EnemyAttack();
		break;
	case EOwnerType::OwnerType_Boss:
		BossAttack();
		break;
	}
}

// プレイヤーの当たり判定
void APlayerSword::PlyerAttack()
{
	// プレイヤーのベースクラスにキャスト
	APlayerBase* Player = Cast<APlayerCharacter>(pOwner);

	if (Player)
	{
		// 当たり判定を取る範囲
		FVector Start = WeaponCollision->GetComponentLocation();
		FVector End = Start - 50.f;

		// 当たったオブジェクト格納用
		TArray<FHitResult> HitResults;

		// 攻撃が当たっているか判定
		bool IsHit = GetHitResults(Start, End, HitResults);

		if (IsHit)
		{
			// 当たったオブジェクトの数だけ繰り返し
			for (const FHitResult HitResult : HitResults)
			{
				// 当たったキャラクターを格納
				AActor* tempActor = HitResult.GetActor();

				// プレイヤーから現在の攻撃力を取得
				float DamageAmount = Player->GetDamageAmount();

				// 先にオブジェクトに当たったら処理しない
				if (tempActor && tempActor->ActorHasTag("Object"))
				{
					// オブジェクトに攻撃
					AttackObject(tempActor, DamageAmount, ObjectHitSoundObj);
					break;
				}

				// ヒットしたアクターが"Enemy"タグを持っていたら
				if (tempActor && tempActor->ActorHasTag("Enemy"))
				{
					// 敵に攻撃
					AttackEnemy(tempActor,DamageAmount, EnemyHitSoundObj);

					if (HitEffect)
					{
						// ヒットエフェクト表示
						ActionAssistComp->SpawnEffect(HitEffect, HitResult.Location);
					}
					break;
				}
			}
		}
	}
}

// 敵の当たり判定
void APlayerSword::EnemyAttack()
{
	AEnamyBase* pEnemy = Cast<AEnamyBase>(pOwner);

	if (pEnemy)
	{
		// 当たり判定を取る範囲
		FVector Start = WeaponCollision->GetComponentLocation();
		FVector End = Start;

		// 当たったオブジェクト格納用
		TArray<FHitResult> HitResults;

		// 攻撃が当たっているか判定
		bool IsHit = GetHitResults(Start, End, HitResults);

		if (IsHit)
		{
			// ダメージ量
			float DamageAmount = 5.f;
			// 当たったオブジェクトの数だけ繰り返し
			for (const FHitResult HitResult : HitResults)
			{
				// 当たったキャラクターを格納
				AActor* tempActor = HitResult.GetActor();

				// ヒットしたアクターが"Enemy"タグを持っていたら
				if (tempActor && tempActor->ActorHasTag("Player"))
				{

					// Playerのダメージ処理
					APlayerBase* Player = Cast<APlayerBase>(HitResult.GetActor());

					if (Player)
					{
						Player->TakedDamage(DamageAmount);

						pEnemy->ActionAssistComp->HitStop(0.1f, HitStopTime);

						if (EnemyHitSoundObj)
						{
							// 斬撃SE再生
							ActionAssistComp->PlaySound(EnemyHitSoundObj);
						}
					}

					break;
				}
			}
		}
	}

}


/*
 * 関数名　　　　：SetHitStopTime()
 * 処理内容　　　：敵に攻撃した時のヒットストップの時間を設定
 * 戻り値　　　　：ヒットストップの時間を返す
 */
float APlayerSword::SetHitStopTime(int _comboNum)
{
	// 初期設定
	float hitStopTime = HitStopTime;

	// コンボ最終段につれ、ヒットストップを少し長く

	for (int i = 2; i < _comboNum; ++i)
	{
		if (_comboNum)
		{
			hitStopTime += HitStopTimeRise;
		}
	}

	return hitStopTime;
}
