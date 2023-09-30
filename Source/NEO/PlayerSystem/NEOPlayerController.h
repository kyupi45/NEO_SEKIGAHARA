// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NEOPlayerController.generated.h"

class APlayerBase;
class AWeaponBase;

UCLASS()
class NEO_API ANEOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// コンストラクタ
	ANEOPlayerController();

	// ゲーム開始時の処理
	void BeginPlay();

	// いずれかのキーを入力中かを取得
	UFUNCTION(BlueprintCallable, Category = "SetStatus")
		bool GetIsAnyKeyPressed()const { return IsInputKeyDown("AnyKey") && GetIsDebugKeyPressed(); }

	UFUNCTION(BlueprintCallable, Category = "SetStatus")
		bool AnyEnemiesNearbyMoreSecond()const;

	// プレイヤーの状態リセット
	void ResetPlayerStatus();

	// プレイヤーの残機取得
	int GetRemainingLives()const { return RemainingLives; }

	// プレイヤーの残機減らす
	void ReduceRemainingLives() { --RemainingLives; }

	// プレイヤーの削除
	void DestroyPlayer();

	// プレイヤーのリスポーン
	void RespawnPlayer();

	// プレイヤーの位置取得
	FVector GetPlayerLocation()const;

	// 現在のプレイヤーのカメラ取得
	FRotator GetNowCameraRotation()const;

	// プレイヤーが死んでいるかを伝える
	bool GetPlayerIsDead()const { return PlayerIsDead; }

	// プレイヤーが武器を持っているか伝える
	bool GetPlayerIsHeldWeapon()const { return PlayerIsHeldWeapon; }

	// プレイヤーが武器を拾える距離を返す
	float GetPickUpDistance()const { return PickUpWeaponDistance; }

	// プレイヤーに取得できる武器があるか伝える
	bool GetIsPickUpWeapon()const { return (CanPickUpWeapons.Num() > 0); }

	// 登録された武器の中で距離が一番近いものを探して渡す
	AWeaponBase* GetClosestDistanceWeapons()const;

	// 拾える武器を追加
	void AddWeapons(AWeaponBase* _weapon) { CanPickUpWeapons.Add(_weapon); }

	// 離れた武器を削除
	void RemoveWeapons(AWeaponBase* _weapon);

private:

	// デバッグキーが押されたか
	bool GetIsDebugKeyPressed()const;


private:

	// プレイヤーがリスポーンできる回数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int DefaultRemainingLives;

	// プレイヤーの近くに何体敵がいたら強調表示するか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int EnemiesCnt;

	// プレイヤーがリスポーンできる回数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float PlayerToEnemyDistance;

	// 武器が拾えるようになる距離
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float PickUpWeaponDistance;

	// PressAnyからFキーは除く
	FName KeyNames[10] = { "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10" };

	// プレイヤーの復活可能回数
	int RemainingLives;

	// プレイヤーが死んでいるか
	bool PlayerIsDead;

	// プレイヤーが武器を持っているか
	bool PlayerIsHeldWeapon;

	// プレイヤーの情報
	APlayerBase* pPlayer;

	// ゲームモード
	class ANEOGameMode* pGameMode;

	// 武器の情報
	TArray<AWeaponBase*> CanPickUpWeapons;
};

