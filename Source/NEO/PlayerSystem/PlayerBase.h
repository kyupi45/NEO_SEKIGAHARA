// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include <type_traits>
#include "ActionAssistComponent.h"
#include "NEO/WeaponSystem/WeaponBase.h"
#include <Runtime/Engine/Classes/Components/CapsuleComponent.h>

#include "PlayerBase.generated.h"

// 前方宣言
class UBoxComponent;
class USphereComponent;
class UCapsuleComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USplineComponent;


//-----------------inputAction------------------------------------------------------------
USTRUCT(BlueprintType)
struct FMainAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* RunAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* JumpAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* ComboAction1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* ComboAction2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* PickAction = nullptr;
};
//----------------------------------------------------------------------------------------

//-----------------プレイヤーのステータス-------------------------------------------------
USTRUCT(BlueprintType)
struct FPlayerStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float HP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float MaxHP = HP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int WeaponDropLimit = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int DefaultWeaponDropLimit = WeaponDropLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int RemainingLife = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float DamageAmount = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float InvincibilityTime_Short = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float InvincibilityTime_Long = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float ComboDamageFactor = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float JumpHeight = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float RunSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float NowMoveSpeed = 600.f;
};
//----------------------------------------------------------------------------------------

//-----------------アニメーション保管用---------------------------------------------------
USTRUCT(BlueprintType)
struct FPlayerAnimation
{
	GENERATED_BODY()

	// コンボ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		TArray<UAnimMontage*> ComboAttack = { nullptr,nullptr };

	// 銃での攻撃
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* GunAttack = nullptr;

	// 銃での攻撃２
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* GunAttack2 = nullptr;

	// 空中にいるときの攻撃
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* AirAttack = nullptr;

	// 被ダメージ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* TakeDamage = nullptr;

	// 被ダメージ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* KnockBack = nullptr;

	// 死亡時
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Death = nullptr;
};
//----------------------------------------------------------------------------------------

UCLASS()
class NEO_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

public:

	// コンストラクタ
	APlayerBase();

protected:

	//----------------入力で呼び出される関数-----------------------------
	// 移動
	virtual void Move(const FInputActionValue& _value);

	// デモ中の移動
	void AIMove();

	// デモ中の攻撃
	void AIAttack();

	// デモ中の武器拾う
	void AIPickUpWeapon();

	// ダッシュ切り替え
	void Run();

	// ジャンプ
	virtual void JumpStart();
	virtual void Jump();

	// コンボ攻撃
	virtual void ComboAttack(int _attackNum = 0);

	// 一つ目のコンボ
	virtual void Attack1();

	// 二つ目のコンボ
	virtual void Attack2();

	// 剣の攻撃
	void SwordAttack(int _attackNum);

	// 槍の攻撃
	void LanceAttack(int _attackNum);

	// 銃の攻撃
	void GunAttack(int _attackNum);

	// 武器拾う
	void PickUpWeapon();
	//-------------------------------------------------------------------


public:

	//-----------------他クラスで呼び出し可--------------------------------------------------------------------------------------------
	// AI解除
	void SetRelease(bool _isAIPlayer) { IsAIPlayer = _isAIPlayer; }

	// コンボ継続
	void ContinuationCombo();

	// コンボリセット
	void ResetCombo();

	void StopMontage();

	int GetComboIndex()const { return ComboIndex; }

	bool GetKicking()const { return IsKicking; }

	// 無敵時間設定
	void SetInvincibility(bool _invincibility) { IsInvincibility = _invincibility; }

	// ダメージを与える処理
	virtual void SetCollision();

	// アクションアシストコンポーネントを取得
	UActionAssistComponent* GetActionAssistComponent()const { return ActionAssistComp; }

	// ヒットストップ
	void HitStop(float _speedDuringHitStop, float _stopTime) { ActionAssistComp->HitStop(_speedDuringHitStop,_stopTime); }

	// カメラシェイク
	void CameraShake() { ActionAssistComp->CameraShake(ShakePattern); }

	// エフェクトスポーン
	void SpawnEffect() { ActionAssistComp->SpawnEffect(HitEffect,GetActorLocation()); }
	
	// 操作可・不可を切り替える処理
	UFUNCTION(BlueprintCallable, Category = "Action")
		void SetControl(bool _isControl) { IsControl = _isControl; }

	// 死亡時のアニメーションの再生を遅くする
	void SlowDownDeathAnimationRate();

	// 疑似RootMotion開始
	void SetEnableRootMotion(bool _enableRootMotion, float _distance = 0);
	
	// RootMotion
	void RootMotion(float _distance);

	// ダメージ量を返す関数
	UFUNCTION(BlueprintCallable, Category = "Action")
		float GetDamageAmount()const { return PlayerStatus.DamageAmount * (((float)ComboIndex + 1.f) * PlayerStatus.ComboDamageFactor); }

	// 現在のHPを返す
	UFUNCTION(BlueprintCallable, Category = "GetStatus")
		float GetHP()const { return PlayerStatus.HP; }

	// ダメージを受ける処理
	UFUNCTION(BlueprintCallable, Category = "Action")
		void TakedDamage(float _damage, bool _isLastAttack = false);

	// 武器の種類取得
	UFUNCTION(BlueprintCallable, Category = "WeaponType")
		EWeaponType GetWeaponType()const { return WeaponType; }

	// ダメージを受ける処理
	UFUNCTION(BlueprintCallable, Category = "Invincible")
		void SetAbsolutelyInvincible(bool _absolutelyInvincible) { AbsolutelyInvincible = _absolutelyInvincible; }

	// ダメージを受ける処理
	UFUNCTION(BlueprintCallable, Category = "SetStatus")
		void SetDeath();

	// プレイヤーが地面にいるか
	bool IsPlayerGrounded()const;

	// 接触開始時に行う処理
	UFUNCTION()
		virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 接触開始時に行う処理
	//UFUNCTION()
	//	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//---------------------------------------------------------------------------------------------------------------------------------------------------


private:

	// キャラクターの回転
	void RotateCharacter(float _nowInput_X);

	// キャラクターの移動量取得
	void AmountOfMovement(FVector _nowPos);

	// 死亡処理呼び出し
	void CallGameModeFunc_DestroyPlayer();

	// 攻撃に関するフラグをすべてリセット
	void ResetAllAttackFlags();

	// アニメーション再生
	void PlayAnimation(UAnimMontage* _toPlayAnimMontage, FName _startSectionName = "None", float _playRate = 1.f);

protected:

	// ゲーム開始時に呼び出される処理
	virtual void BeginPlay() override;

	// 毎フレーム呼び出される処理
	virtual void Tick(float DeltaTime) override;

	// 入力のセットアップ
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//------------------プレイヤーのセットアップ---------------------------------------------------------------------------------------------------------
	// プレイヤーのデータを初期化
	virtual void SetupPlayerData();

	// プレイヤーのステータス初期化
	void SetupPlayerStatus(float _hp = 100.f, int _remainingLife = 3.f, float _damageAmount = 10.f,
							float _invincibilityTime_Short = 0.3f, float _invincibilityTime_Long = 0.5f,
							float _jumpHeight = 150.f,float _comboDamageFactor = 1.f, float _walkSpeed = 10.f, float _runSpeed = 30.f);

	// ボタンの設定
	void SetupMainActionMapping();

	// アニメーションアセットの設定
	void SetupAnimationAssets();

	// 引数によってスタティックメッシュかスケルタルメッシュのセットアップ
	void SetupWeaponMesh(UStaticMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName = "WeaponStaticMesh");
	void SetupWeaponMesh(USkeletalMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName = "WeaponSkeletalMesh");

	// 指定したパスのアニメーションアセットを返す
	UAnimMontage* GetAnimationAsset(TCHAR* _animAssetPath);

	//-----------------プレイヤー管理用変数------------------------------------------------------------------
private:

	// プレイやーのアクション管理用
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FMainAction MainActionMapping;

protected:

	// プレイヤーのステータス管理用
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FPlayerStatus PlayerStatus;

	// プレイヤーのアニメーション管理用
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FPlayerAnimation PlayerAnimation;

	// 現在所持している武器を判別するEnum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponType")
		EWeaponType WeaponType;

	// プレイヤーの武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AWeaponBase> WeaponClass;

	// 武器のクラス
	AWeaponBase* Weapon;

	//-------------------------------------------------------------------------------------------------------------


	//-----------------コンポーネント変数--------------------------------------------------------------------------
	// 攻撃のアシスト用
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Assist", meta = (AllowPrivateAccess = "true"))
		UActionAssistComponent* ActionAssistComp;

	// 被ダメージ時のエフェクト
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
		class UNiagaraSystem* HitEffect;

	// カメラの揺れ方
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraShakeBase> ShakePattern;

	// キャラクターの動き
	UCharacterMovementComponent* CharacterMovementComp;
	//-------------------------------------------------------------------------------------------------------------

	// 死亡アニメーションで倒れてからの再生スピード(1で通常スピード)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"),Category = "GameOver")
		float DeadAnimRate = 0.01f;							

	// 死んでからゲームオーバーまでの時間(秒)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float DeadToGameOverTime = 3.f;					

	// 移動した距離
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float DistanceAdvanced;

private:

	// 入力可能かどうか
	bool IsControl;	

	// AI移動する
	bool IsAIPlayer;

	// ダッシュ中かどうか
	bool IsRunning;		

	// 右を向いているか
	bool IsLookRight;

	// 右に動いているか
	bool MoveRight = false;

	// 蹴り攻撃中かどうか
	bool IsKicking;

	// 武器を拾えるかどうか
	bool IsPickUpWeapon;

	// ジャンプ中かどうか
	bool IsJumping;

	// EnableRootMotion
	bool EnableRootMotion;

	// ルートモーションでの移動値
	float AnimationMoveValue;

	// デバッグ用絶対無敵
	bool AbsolutelyInvincible = false;

	// 無敵状態かどうか
	bool IsInvincibility;

	// 武器それぞれのソケットの名前
	FName SocketName[3];

	// 武器を持っているかどうか
	bool IsHoldWeapon;

	// 死んでいるかどうか
	bool IsDeath;

	// フレームカウント用
	float frames;	

	// ジャンプの計算
	const float radPerFrame = 3.14f / 30.f;

	// ジャンプ前の高さ
	float JumpBeforePos_Z;	

	// 前の位置
	FVector BeforePos;

	// 攻撃中かどうか
	bool IsAttacking;						

	// コンボ継続できるか
	bool CanCombo;							

	// 何段目のコンボか
	int ComboIndex;		

	// コンボの段数(First,Second,Third・・・)
	TArray<FName> ComboStartSectionNames;	

	// ハンドル
	FTimerHandle TimerHandle;		

	// ゲームモード保存
	class ANEOGameMode* pGameMode;

	class ANEOPlayerController* PlayerController;
};