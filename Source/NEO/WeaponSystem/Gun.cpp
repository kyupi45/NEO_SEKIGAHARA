// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NEO/PlayerSystem/PlayerCharacter.h"
#include "NEO/Enemy/EnamyBase.h"
#include "NEO/OdaBase.h"
#include "NEO/BackGroundSystem/ObjectBase.h"
#include "Components/CapsuleComponent.h"
#include "PlayerBullet.h"


// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	// 武器のメッシュのパス
	TCHAR* WeaponAssetPath = TEXT("/Game/0122/Player/Weapon/Gun/Gun");

	// 武器のメッシュ設定
	SetupWeaponMesh(WeaponStaticMesh, WeaponAssetPath);

	// コリジョン設定
	SetupCollisionComponent(WeaponCollision);

	// 武器の種類を刀に設定
	WeaponType = EWeaponType::WeaponType_Gun;
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*
 * 関数名　　　　：SetCollision()
 * 処理内容　　　：プレイヤーの攻撃の当たり判定
 * 戻り値　　　　：なし
 */
void AGun::SetCollision()
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
void AGun::PlyerAttack()
{
	// プレイヤーのベースクラスにキャスト
	APlayerBase* Player = Cast<APlayerCharacter>(pOwner);

	if (Player)
	{
		// 蹴り攻撃かどうか
		const bool Kicking = Player->GetKicking();

		if (!Kicking)
		{
			// 蹴り攻撃じゃない時銃を発射
			SpawnBullet();
		}
		else
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
					float DamageAmount = 10.f;

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
						AttackEnemy(tempActor, DamageAmount, EnemyHitSoundObj);

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
}

// 敵の当たり判定
void AGun::EnemyAttack()
{
	// 敵は発射のみ
	SpawnBullet();
}

// 敵に攻撃した時の処理
void AGun::AttackEnemy(AActor* _enemy, float _damageAmount, class USoundBase* _hitSoundObj)
{
	// プレイヤー取得
	APlayerBase* Player = Cast<APlayerBase>(pOwner);

	// エネミーのdamage処理
	AEnamyBase* Enemy = Cast<AEnamyBase>(_enemy);
	AOdaBase* Boss = Cast<AOdaBase>(_enemy);

	// ヒットストップ
	Player->HitStop(0.1f, HitStopTime);

	if (Enemy)
	{
		// 敵のダメージ処理
		Enemy->ApplyDamage(_damageAmount);

		// 敵のノックバック処理
		if (!Enemy->ActionAssistComp->WallCheck(KnockBackDistance))
		{
			Enemy->AddActorLocalOffset(FVector(KnockBackDistance, 0.f, 0.f));
		}

		// 敵が常に動いているので
		// コンボが当たりやすいように敵にも止まってもらう
		Enemy->ActionAssistComp->HitStop(0.1f, HitStopTime * 2.f);
	}
	else if (Boss)
	{
		// ボスのダメージ処理
		Boss->ApplyDamage(_damageAmount);

		// 敵のノックバック処理
		if (!Boss->ActionAssistComp->WallCheck(KnockBackDistance))
		{
			Boss->AddActorLocalOffset(FVector(KnockBackDistance, 0.f, 0.f));
		}

		// 敵が常に動いているので
		// コンボが当たりやすいように敵にも止まってもらう
		Boss->ActionAssistComp->HitStop(0.1f, HitStopTime * 2.f);
	}

	if (_hitSoundObj)
	{
		// 蹴りSE再生
		ActionAssistComp->PlaySound(_hitSoundObj);
	}
}



// 銃弾をスポーン
void AGun::SpawnBullet()
{
	// 銃弾をスポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// スポーン時の場所と回転を指定
	FVector SpawnBulletLocation = GetActorLocation();
	FRotator SpawnBulletRotation = pOwner->GetActorRotation();

	GetWorld()->SpawnActor<AActor>(BulletClass, SpawnBulletLocation, SpawnBulletRotation, SpawnParams);

	if (ShootSoundObj)
	{
		// 発射音再生
		ActionAssistComp->PlaySound(ShootSoundObj);
	}
}
