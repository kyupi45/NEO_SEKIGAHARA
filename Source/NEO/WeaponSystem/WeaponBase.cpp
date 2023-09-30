// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NEO/PlayerSystem/ActionAssistComponent.h"
#include "NEO/PlayerSystem/PlayerBase.h"
#include "NEO/PlayerSystem/NEOPlayerController.h"
#include "NEO/BackGroundSystem/ObjectBase.h"
#include "NEO/Enemy/EnamyBase.h"
#include "NEO/OdaBase.h"

// Sets default values
AWeaponBase::AWeaponBase()
	: CalculateInterval_nearPlayer(1.f)
	, CalculateInterval_farPlayer(10.f)
	, DistanceToOmitCalc(2000.f)
	, JumpHeight(150.f)
	, IsHeld(false)
	, IsFalling(false)
	, Frames(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	// タグ設定
	Tags.Add("Weapon");

	// アタックアシストコンポーネント作成
	ActionAssistComp = CreateDefaultSubobject<UActionAssistComponent>(TEXT("AttackAssist"));
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	// コントローラー取得
	PlayerController = Cast<ANEOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFalling)
	{
		BlowsAway();
	}
}

/*
 * 関数名　　　　：SetupWeaponMesh()
 * 処理内容　　　：プレイヤーのメッシュをセットアップ
 * 引数１　　　　：UStaticMeshComponent*& MeshComp・・・メッシュコンポーネント
 * 引数２　　　　：TCHAR* WeaponAssetPath ・・・・・・・武器のアセットのパス
 * 引数３　　　　：FName PublicName ・・・・・・・・・・エディタでの公開名
 * 戻り値　　　　：なし
 */
void AWeaponBase::SetupWeaponMesh(UStaticMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName /*= "WeaponMesh"*/)
{
	// 武器のコンポーネントを作成
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(PublicName);

	if (WeaponAssetPath)
	{
		// 武器のアセット設定
		ConstructorHelpers::FObjectFinder< UStaticMesh > weaponMesh(WeaponAssetPath);

		if (weaponMesh.Succeeded())
		{
			MeshComp->SetStaticMesh(weaponMesh.Object);
		}
	}
}


/*
 * 関数名　　　　：AttachToHand()
 * 処理内容　　　：キャラクターにくっつける
 * 引数１　　　　：ACharacter* _owner ・・・・親になるキャラクター
 * 引数２　　　　：FName _socketName・・・・・ソケットの名前
 * 引数３		 ：EOwnerType _ownerType・・・オーナーの種類
 * 戻り値　　　　：なし
 */
void AWeaponBase::AttachToHand(ACharacter* _owner, FName _socketName,EOwnerType _ownerType)
{
	// 飛んでいるときは取れない
	if (IsFalling || IsHeld) { return; }

	// 持たれている状態にする
	IsHeld = true;

	// オーナーの種類を設定
	OwnerType = _ownerType;

	// オーナーのクラス取得
	pOwner = _owner;

	// コントローラーから削除
	PlayerController->RemoveWeapons(this);

	
	// キャラクターにアタッチ
	if (pOwner)
	{
		AttachToComponent(pOwner->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false), _socketName);
	}
}


/*
 * 関数名　　　　：DetachToHand()
 * 処理内容　　　：キャラクターにくっつける
 * 戻り値　　　　：なし
 */
void AWeaponBase::DetachToHand()
{
	// 持たれていない状態にする
	IsHeld = false;

	// キャラクターから外す
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld,false));

	// オーナーがいなくなる
	pOwner = nullptr;

	// 吹き飛ばすフラグを建てる
	IsFalling = true;

	// 飛ぶ前の場所設定
	FlyBeforePos = GetActorLocation();
}


/*
 * 関数名　　　　：GetHitResults()
 * 処理内容　　　：攻撃が当たったかどうか判定
 * 引数１　　　　：FVector _start ・・・・・・・・ ・・・・当たり判定をとる範囲(開始地点)
 * 引数２　　　　：FVector _end・・・・・・・・・・・・・・当たり判定をとる範囲(終了地点)
 * 引数３		 ：TArray<FHitResult>& _outHitResults・・・当たったオブジェクト
 * 戻り値　　　　：当たったかどうか
 */
bool AWeaponBase::GetHitResults(FVector _start, FVector _end, TArray<FHitResult>& _outHitResults)
{
	// 武器本体とオーナーには当たらないようにする
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(pOwner);

	// 当たった部分を取得
	FQuat Rot = WeaponCollision->GetComponentQuat();
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(WeaponCollision->GetScaledCapsuleRadius(), WeaponCollision->GetScaledCapsuleHalfHeight());

	// あたっているか確認
	bool IsHit = GetWorld()->SweepMultiByChannel(_outHitResults, _start, _end, Rot, ECollisionChannel::ECC_GameTraceChannel1, CollisionShape, CollisionParams);

	return IsHit;
}


/*
 * 関数名　　　　：AttackObject()
 * 処理内容　　　：破壊可能オブジェクトに攻撃した時の処理
 * 引数１　　　　：AActor* _object ・・・・・・・当たったオブジェクト
 * 引数２　　　　：float _damageAmount ・・・・・ダメージ量
 * 引数３		 ：USoundBase* _hitSoundObj・・・ヒット時になるサウンド
 * 戻り値　　　　：なし
 */
void AWeaponBase::AttackObject(AActor* _object, float _damageAmount, USoundBase* _hitSoundObj)
{
	AObjectBase* Object = Cast<AObjectBase>(_object);

	if (Object)
	{
		// ダメージを与える
		Object->ReceiveDamage(_damageAmount);

		// オブジェクト破壊用のサウンド再生
		ActionAssistComp->PlaySound(_hitSoundObj);
	}
}


/*
 * 関数名　　　　：AttackEnemy()
 * 処理内容　　　：敵に攻撃した時の処理
 * 引数１　　　　：AActor* _enemy・・・・・・・・当たった敵
 * 引数２　　　　：float _damageAmount ・・・・・ダメージ量
 * 引数３		 ：USoundBase* _hitSoundObj・・・ヒット時になるサウンド
 * 戻り値　　　　：なし
 */
void AWeaponBase::AttackEnemy(AActor* _enemy, float _damageAmount, USoundBase* _hitSoundObj)
{
	// プレイヤー取得
	APlayerBase* Player = Cast<APlayerBase>(pOwner);

	// エネミーのdamage処理
	AEnamyBase* Enemy = Cast<AEnamyBase>(_enemy);
	AOdaBase* Boss = Cast<AOdaBase>(_enemy);

	// コンボ取得
	int ComboNum = Player->GetComboIndex();

	// ヒットストップの時間を取得
	const float HitStopTime = SetHitStopTime(ComboNum);

	// ヒットストップ
	Player->HitStop(0.1f, HitStopTime);


	if (Enemy)
	{
		// 敵のダメージ処理
		Enemy->ApplyDamage(_damageAmount);

		// 敵のノックバック処理
		EnemyKnockBack(ComboNum,Enemy);

		// 敵が常に動いているので
		// コンボが当たりやすいように敵にも止まってもらう
		Enemy->ActionAssistComp->HitStop(0.1f, HitStopTime);
	}
	else if (Boss)
	{
		// ボスのダメージ処理
		Boss->ApplyDamage(_damageAmount);

		// コンボが最終段の時ノックバック
		if (Player->GetComboIndex() == 3)
		{
			Boss->BossKnockback();
		}
	}

	// コンボのフィニッシュのみカメラを揺らす
	if (ComboNum == 3)
	{
		Player->CameraShake();
	}

	if (_hitSoundObj)
	{
		// 斬撃SE再生
		ActionAssistComp->PlaySound(_hitSoundObj);
	}
}


/*
 * 関数名　　　　：EnemyKnockBack()
 * 処理内容　　　：敵のノックバック処理
 * 戻り値　　　　：なし
 */
void AWeaponBase::EnemyKnockBack(int _comboNum, AEnamyBase* _enemy)
{
	if (!_enemy) { return; }

	// ノックバックの距離格納用
	float KnockBackDistance = 0.f;

	// コンボによってノックバック
	if (_comboNum == 1)
	{
		KnockBackDistance = -100.f;
	}
	else if (_comboNum == 2)
	{
		KnockBackDistance = -150.f;
	}
	else if (_comboNum == 3)
	{
		KnockBackDistance = -250.f;
	}

	// ノックバック
	if (!_enemy->ActionAssistComp->WallCheck(KnockBackDistance))
	{
		_enemy->AddActorLocalOffset(FVector(KnockBackDistance, 0.f, 0.f));
	}
}


/*
 * 関数名　　　　：DistanceCalculationToPlayer()
 * 処理内容　　　：プレイヤーとの距離計算
 * 戻り値　　　　：なし
 */
void AWeaponBase::DistanceCalculationToPlayer()
{
	if (IsHeld || IsFalling) { return; }

	if (!PlayerController) { return; }

	// プレイヤーの位置取得
	FVector PlayerPos = PlayerController->GetPlayerLocation();

	// プレイヤーと距離計算
	DistanceToPlayer = FVector::Dist(PlayerPos, GetActorLocation());

	// 拾える距離だったら
	if (DistanceToPlayer <= PlayerController->GetPickUpDistance())
	{
		if (!IsPickUpDistanceEnteredPlayer)
		{
			// コントローラーに追加
			PlayerController->AddWeapons(this);

			// 一定距離内にプレイヤーが入った
			IsPickUpDistanceEnteredPlayer = true;
		}
	}
	else
	{
		if (IsPickUpDistanceEnteredPlayer)
		{
			// コントローラーから削除
			PlayerController->RemoveWeapons(this);

			// 一定距離外にプレイヤーがでた
			IsPickUpDistanceEnteredPlayer = false;
		}
	}

	// プレイヤーとの距離によって計算の間隔指定
	const float CalculateInterval = (DistanceToPlayer > DistanceToOmitCalc) ? (CalculateInterval_farPlayer) : (CalculateInterval_nearPlayer);

	// 一定時間後に再度距離を計算
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(TimerHandle, this, &AWeaponBase::DistanceCalculationToPlayer, CalculateInterval, false);
}


/*
 * 関数名　　　　：BlowsAway()
 * 処理内容　　　：武器が落ちて地面に刺さる
 * 戻り値　　　　：なし
 */
void AWeaponBase::BlowsAway()
{
	// 現在位置
	FVector NowPos = GetActorLocation();

	// Sinで高さ更新
	float SinValue = JumpHeight * FMath::Sin(RadPerFrame * Frames);

	// ジャンプ前の高さから位置更新
	const FVector nextPos(FVector(NowPos.X, NowPos.Y, SinValue + FlyBeforePos.Z));
	SetActorLocation(nextPos);

	// フレーム更新
	Frames += 1.f;

	// 現在の回転取得
	FRotator NowRotation = GetActorRotation();

	// 常に回転
	SetActorRotation(FRotator(NowRotation.Pitch, NowRotation.Yaw, NowRotation.Roll + 10.0));

	// 元の位置より低くなったら終了
	if (NowPos.Z < FlyBeforePos.Z)
	{
		// 飛んでいる状態のフラグを下ろす
		IsFalling = false;

		// フレームリセット
		Frames = 0.f;

		// 落下音再生
		if (DropWeaponSoundObj)
		{
			// 斬撃SE再生
			ActionAssistComp->PlaySound(DropWeaponSoundObj);
		}

		// 地面に刺さるように位置と角度を補正
		SetActorLocation(FlyBeforePos + DropLocation);
		SetActorRotation(DropAngle);

		// プレイヤーコントローラーに拾えるようになったことを伝える
		PlayerController->AddWeapons(this);

		//プレイヤーとの計算処理を開始
		DistanceCalculationToPlayer();
	}
}


