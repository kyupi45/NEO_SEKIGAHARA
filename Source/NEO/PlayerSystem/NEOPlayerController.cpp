// Fill out your copyright notice in the Description page of Project Settings.


#include "NEOPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "NEOGameMode.h"
#include "PlayerBase.h"
#include "NEO/WeaponSystem/WeaponBase.h"

ANEOPlayerController::ANEOPlayerController()
	: DefaultRemainingLives(2)
	, EnemiesCnt(3)
	, PlayerToEnemyDistance(200.f)
	, PickUpWeaponDistance(120.f)
	, RemainingLives(DefaultRemainingLives)
	, PlayerIsDead(false)
{

}

// ゲーム開始時の処理
void ANEOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ゲームモード取得
	pGameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// プレイヤー取得
	pPlayer = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
}


/*
 * 関数名　　　　：ResetPlayerStatus()
 * 処理内容　　　：プレイヤーの入力受付(移動処理)
 * 引数１　　　　：FInputActionValue& Value・・・入力量
 * 戻り値　　　　：なし
 */
void ANEOPlayerController::ResetPlayerStatus()
{
	// 残機設定
	RemainingLives = DefaultRemainingLives;
}


/*
 * 関数名　　　　：DestroyPlayer()
 * 処理内容　　　：プレイヤーの入力受付(移動処理)
 * 戻り値　　　　：なし
 */
void ANEOPlayerController::DestroyPlayer()
{
	if (pPlayer && pGameMode)
	{
		// プレイヤー削除
		pPlayer->Destroy();

		// プレイヤーをNULLに
		pPlayer = nullptr;

		// プレイヤーを死亡状態へ
		PlayerIsDead = true;

		// ゲームを次の状態へ
		pGameMode->SetNextGameState();
	}
}


/*
 * 関数名　　　　：RespawnPlayer()
 * 処理内容　　　：プレイヤーの入力受付(移動処理)
 * 引数１　　　　：FInputActionValue& Value・・・入力量
 * 戻り値　　　　：なし
 */
void ANEOPlayerController::RespawnPlayer()
{
	if (pPlayer && pGameMode)
	{
		// プレイヤーが死亡した位置取得
		FTransform RespownPos = pPlayer->GetActorTransform();

		// プレイヤーを削除
		pPlayer->Destroy();

		// プレイヤーをNULLに
		pPlayer = nullptr;

		// プレイヤーの残機を1減らす
		ReduceRemainingLives();

		// 新しいプレイヤーを生成
		pPlayer = Cast<APlayerBase>(GetWorld()->SpawnActor<APawn>(pGameMode->GetDefaultPawnClass(), RespownPos));

		// リスポーンのカメラ処理
		pGameMode->SetCameraOnPlayer();
	}
}


/*
 * 関数名　　　　：GetPlayerLocation()
 * 処理内容　　　：プレイヤーの座標を返す
 * 戻り値　　　　：プレイヤーの座標
 */
FVector ANEOPlayerController::GetPlayerLocation()const
{
	if (pPlayer)
	{
		return pPlayer->GetActorLocation();
	}

	return FVector::ZeroVector;
}

/*
 * 関数名　　　　：GetNowCameraRotation()
 * 処理内容　　　：カメラの回転
 * 戻り値　　　　：なし
 */
FRotator ANEOPlayerController::GetNowCameraRotation()const
{
	if (pGameMode)
	{
		// 現在のカメラを取得
		AActor* NowCamera = pGameMode->GetNowPlayerCamera();

		// 回転を取得して返す
		if (NowCamera)
		{
			// カメラのコンポーネント取得
			UCameraComponent* CameraComponent = NowCamera->FindComponentByClass<UCameraComponent>();
			if (CameraComponent)
			{
				return CameraComponent->GetComponentRotation();
			}
		}
	}

	return FRotator::ZeroRotator;
}


/*
 * 関数名　　　　：RemoveWeapons()
 * 処理内容　　　：武器削除
 * 戻り値　　　　：なし
 */
void ANEOPlayerController::RemoveWeapons(AWeaponBase* _weapon)
{
	if (_weapon)
	{
		// 武器削除
		for (int i = 0; i < CanPickUpWeapons.Num(); ++i)
		{
			if (CanPickUpWeapons[i] == _weapon)
			{
				CanPickUpWeapons.Remove(_weapon);
				break;
			}
		}
	}
}

/*
 * 関数名　　　　：GetIsDebugKeyPressed()
 * 処理内容　　　：デバッグキーに設定されているキーが押されているか
 * 戻り値　　　　：なし
 */
bool ANEOPlayerController::GetIsDebugKeyPressed()const
{
	for (int i = 0; i < sizeof(KeyNames) / sizeof(FName); ++i)
	{
		if (IsInputKeyDown(KeyNames[i]))
		{
			return false;
		}
	}

	return true;
}


/*
 * 関数名　　　　：GetClosestDistanceWeapons()
 * 処理内容　　　：登録された武器の中で距離が一番近いものを取得
 * 戻り値　　　　：なし
 */
AWeaponBase* ANEOPlayerController::GetClosestDistanceWeapons()const
{
	// プレイヤーがいなければnullを返す
	if (!pPlayer) { return nullptr; }

	// 一番近い武器は何番目か保管
	int WeaponIndex = 0;

	// 一番近い武器の距離保管
	float ClosestDistance = 0;

	for (int i = 0; i < CanPickUpWeapons.Num(); ++i)
	{
		// プレイヤーの位置取得
		const FVector PlayerPos = GetPlayerLocation();

		// 武器の位置取得
		const FVector WeaponPos = CanPickUpWeapons[i]->GetActorLocation();

		// プレイヤーと武器の距離計算
		const float Distance = FVector::Dist(PlayerPos, WeaponPos);

		// 1回目は一番近い距離に設定
		if (i == 0)
		{
			ClosestDistance = Distance;
		}
		//　2回目以降は距離を比べて一番近い距離を探す
		else
		{
			// 距離を比較
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				WeaponIndex = i;
			}
		}
	}

	return CanPickUpWeapons[WeaponIndex];
}


bool ANEOPlayerController::AnyEnemiesNearbyMoreSecond()const
{
	if (pGameMode)
	{
		// ゲームモードから現在出ている敵の情報を取得
		TArray<AActor*> Enemies = pGameMode->GetEnemies();

		// プレイヤーの位置取得
		FVector PlayerPos = pPlayer->GetActorLocation();

		// 近くに敵がいた時のカウント
		int NearByEnemiesCnt = 0;
		// プレイヤーと敵との距離を計測
		for (int i = 0; i < Enemies.Num(); ++i)
		{
			if (Enemies[i])
			{
				// 敵の位置取得
				FVector EnemyPos = Enemies[i]->GetActorLocation();
				// プレイヤーと距離計算
				float Distance = FVector::Dist(EnemyPos, PlayerPos);

				if (Distance <= PlayerToEnemyDistance)
				{
					++NearByEnemiesCnt;
				}
			}

			if (NearByEnemiesCnt >= EnemiesCnt)
			{
				return true;
			}
		}
	}

	return false;
}