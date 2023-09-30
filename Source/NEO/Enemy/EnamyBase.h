// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DeathTrigger.h"
#include "NEO/GameSystem/GameSystem_BattleArea.h"
#include "NiagaraComponent.h"
#include "NEO/WeaponSystem/EnemySword.h"
#include "Components/SplineComponent.h"
#include "EnamyBase.generated.h"

UCLASS()
class NEO_API AEnamyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnamyBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
	bool IsAreaEnemy = false;
	//アニメーション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Attack2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Attack3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Death;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Damage_Reaction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* MovingAnimation;
	UPROPERTY()
		UAnimInstance* AnimInstance;
	UFUNCTION(BlueprintCallable, Category = "Area")
	virtual void DestoryEnemy();
	UPROPERTY()
		class ACharacter* PlayerCharacter; // プレイヤーキャラクターの参照

	
	// 攻撃のアシスト用コンポーネント
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AttackAssist, meta = (AllowPrivateAccess = "true"))
		class UActionAssistComponent* ActionAssistComp;
	// Damage to be dealt to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float Damage;
	UPROPERTY(VisibleAnywhere)
		USplineComponent* MoveSpline;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY()
		FTimerHandle TimerHandle_DestroyEnemy;
	AActor* GetPlayer();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		UNiagaraSystem* NiagaraEffect;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float  Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float  MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
		TSubclassOf<ADeathTrigger>DeathTriggerClass;
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class AEnemySword> EnemySwordClass;
	UFUNCTION(BlueprintCallable, Category = "Health")
		virtual void ApplyDamage(float DamageAmount);

	// 武器のクラス
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponClass", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AWeaponBase> WeaponClass;

	// 武器のクラス
	class AWeaponBase* Weapon;

	void MaintainDistanceFromEnemy();
	 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
		float DesiredDistanceFromEnemy = 300.0f;

	bool bHasPattern1Tag; // pattern1というTagを持っているかどうか
	bool bHasPattern2Tag;
	bool bHasPattern3Tag;
	bool bHasWeponTag;
	void AfterDeath();

	void DamageReac();

	bool bIsNowDamage;

	void CheckHealth();

	void SpawnDeathTrigger();

	virtual void CollisionOn() { return; }

	bool bIsShoot = false;

	UFUNCTION(BlueprintCallable)
		float GetHP()const { return Health; }
	
	bool bIsRotation;

	bool bIsRotationTag2;

	bool bIsDeath;
	
	
	UPROPERTY(BlueprintReadWrite)
		bool IsIdol;

	UPROPERTY(BlueprintReadWrite)
		bool IsRunning;

	
	///UI
public:

	AActor* GetEnemyActor() const;
	TArray<AGameSystem_BattleArea*> BattleAreaReferences;  // 複数のバトルエリアの参照
	float SpawnTime; // スポーンした時間を記録する変数
	USplineComponent* MovementSpline;

	// 移動を開始するためのフラグと時間
	bool bShouldMoveAlongSpline = false;
	float TimeSinceStartOfMovement = 0.0f;

	// 3秒間移動するための定数
	static constexpr float MovementDuration = 3.0f;
	ECollisionChannel EnemyCollison = ECollisionChannel::ECC_GameTraceChannel1;
	
	UFUNCTION()
    void OnOverlapBeginWall(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};