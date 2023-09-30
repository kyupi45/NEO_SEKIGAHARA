// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"


class ACharacter;
class UNiagaraSystem;

//-----------------所持者の種類-----------------------------------
UENUM(BlueprintType)
enum class EOwnerType :uint8
{
	OwnerType_Player   UMETA(DisplayName = "PlayerType"),
	OwnerType_Enemy    UMETA(DisplayName = "EnemyType"),
	OwnerType_Boss     UMETA(DisplayName = "BossType")
};
//----------------------------------------------------------------

//-----------------武器の種類-------------------------------------
UENUM(BlueprintType)
enum class EWeaponType :uint8
{
	WeaponType_Sword   UMETA(DisplayName = "Sword"),
	WeaponType_Lance   UMETA(DisplayName = "Lance"),
	WeaponType_Gun     UMETA(DisplayName = "Gun"),
	WeaponType_None	   UMETA(DisplayName = "None")
};
//----------------------------------------------------------------


UCLASS()
class NEO_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// コンストラクタ
	AWeaponBase();

public:

	// プレイヤーの手に付ける
	void AttachToHand(ACharacter* _owner, FName _socketName, EOwnerType _ownerType);

	// プレイヤーの手から外れる
	void DetachToHand();

	// ダメージを与える処理(オーバーライド用)
	virtual void SetCollision() { return; }

	// 持たれている状態か
	UFUNCTION(BlueprintCallable, Category = "State")
		bool GetIsHeld()const { return IsHeld; }

	// 飛んでいる状態か
	UFUNCTION(BlueprintCallable, Category = "State")
		bool GetIsFalling()const { return IsFalling; }

	// プレイヤーが近くにいるか
	UFUNCTION(BlueprintCallable, Category = "State")
		bool GetIsPickUpDistanceEnteredPlayer()const { return IsPickUpDistanceEnteredPlayer; }

	// オーナーの種類判別用
	UFUNCTION(BlueprintCallable, Category = "State")
		EOwnerType GetOwnerType()const { return OwnerType; }

	// 武器の種類判別用
	UFUNCTION(BlueprintCallable, Category = "State")
		EWeaponType GetWeaponType()const { return WeaponType; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// StaticMeshのセットアップ
	void SetupWeaponMesh(UStaticMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName = "WeaponMesh");

// ---------コリジョンコンポーネント作成テンプレート
/*
 * 関数名　　　　：SetupCollisionComponent()
 * 処理内容　　　：武器のコリジョンセットアップ
 * 引数１　　　　：T* CollisionComp・・・・・・・・・コリジョンコンポーネント(「T」は UBoxComponent,USphereComponent,UCapsuleComponent のいずれか)
 * 引数２　　　　：FName PublicName・・・・・・・・・エディタでの公開名
 * 戻り値　　　　：なし
 */
	template<class T>
	void SetupCollisionComponent(T*& CollisionComp, FName PublicName = "CollisionComp")
	{
		static_assert(std::is_same<T, UBoxComponent>::value || std::is_same<T, USphereComponent>::value || std::is_same<T, UCapsuleComponent>::value,
			"「T」は UBoxComponent,USphereComponent,UCapsuleComponent のいずれか ");

		// 対応するオブジェクト生成
		CollisionComp = CreateDefaultSubobject<T>(PublicName);

		if (CollisionComp)
		{
			// 武器のメッシュに追従
			CollisionComp->SetupAttachment(WeaponStaticMesh, "hand_rSocket");
		}
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------


	// プレイヤーの攻撃記述用
	virtual void PlayerAttack() { return; }

	// 敵の攻撃記述用
	virtual void EnemyAttack() { return; }

	// ボスの攻撃記述用
	virtual void BossAttack() { return; }

	// 攻撃が当たったかどうか
	bool GetHitResults(FVector _start,FVector _end, TArray<FHitResult>& _outHitResults);

	// 破壊可能オブジェクトに攻撃した時の処理
	void AttackObject(AActor* _object, float _damageAmount, class USoundBase* _hitSoundObj);

	// 敵に攻撃した時の処理
	virtual void AttackEnemy(AActor* _enemy, float _damageAmount, class USoundBase* _hitSoundObj);

	// 敵に攻撃した時のヒットストップの時間を設定
	virtual float SetHitStopTime(int _comboNum) { return 0.f; }

	// 敵のノックバック処理
	void EnemyKnockBack(int _comboNum, class AEnamyBase* _enemy);


private:

	// プレイヤーとの距離を計算する処理
	void DistanceCalculationToPlayer();

	// オーナーがいなくなったとき武器が落ちる
	void BlowsAway();


protected:

		// 武器のメッシュ
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMesh")
			class UStaticMeshComponent* WeaponStaticMesh;

		// 武器のコリジョン
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponCollision")
			class UCapsuleComponent* WeaponCollision;

		// アクション補助の役割をするコンポーネント
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActionAssist")
			class UActionAssistComponent* ActionAssistComp;

		// 落ちている武器にかかるエフェクト
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
			UNiagaraSystem* AuraEffect;

		// 武器を落とした時の音
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
			class USoundBase* DropWeaponSoundObj;

		// 武器を判別するEnum
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
			FVector DropLocation;

		// 武器を判別するEnum
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
			FRotator DropAngle;

		// プレイヤーとの距離が近い時の計算を行う間隔
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calculate")
			float CalculateInterval_nearPlayer;

		// プレイヤーとの距離が遠い時の計算を行う間隔
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calculate")
			float CalculateInterval_farPlayer;

		// 距離計算を省く距離
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calculate")
			float DistanceToOmitCalc;

		// ジャンプの高さ
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calculate")
			float JumpHeight;

		// 所持者の情報
		ACharacter* pOwner;

		// オーナーを判別するEnum
		EOwnerType OwnerType;

		// 武器を判別するEnum
		EWeaponType WeaponType;

private:

	// プレイヤーに持たれているか
	bool IsHeld;

	// 飛んでいるかどうか
	bool IsFalling;

	// プレイヤーが一定距離内に入ったか
	bool IsPickUpDistanceEnteredPlayer;

	// フレームカウント用
	float Frames;

	// プレイヤーとの距離
	float DistanceToPlayer;

	// ジャンプの計算
	const float RadPerFrame = 3.14f / 30.f;

	// 飛ぶ前の位置
	FVector FlyBeforePos;

	// プレイヤーコントローラーの情報
	class ANEOPlayerController* PlayerController;

	// ハンドル
	FTimerHandle TimerHandle;
};
