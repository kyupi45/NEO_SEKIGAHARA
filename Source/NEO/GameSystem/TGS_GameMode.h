// 更新日：2023/6/5		更新者：董		更新内容：ゲームモードの基本クラスを作成
// 更新日：2023/6/8		更新者：董		更新内容：ゲームモードのKeyInput機能、基本的な機能を作成

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameMode.h"
#include "InputCharacter.h"

#include "TGS_GameMode.generated.h"

enum class EChangeLevel : uint8;

/**
 *
 */
UCLASS()
class NEO_API ATGS_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ATGS_GameMode();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameClass")
		TSubclassOf<class AActor> PlayerBaseClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameClass")
		TSubclassOf<class AActor> EnemyBaseClass;


public:
	virtual void BeginPlay() override;

	virtual void StartPlay() override;

	void Tick(float DeltaTime) override;

	void NextLevel(FName _nextLevelName);

	UFUNCTION(BlueprintCallable, Category = "Level")
		void ChangeNextLevel(ENextLevel NextLevel);					//目標のLevelに遷移する

//---------------------------------------スポーン＆デストロイ---------------------------------------
public:
	class APlayerBase* PlayerPtr;
	FTransform DeathTrans;

	void SpawnPlayer(AActor* _player, FTransform _tranceform);
	void DestroyPlayer(AActor* _player);
	void RespawnPlayer();

	//バトルエリアにいる敵をスポーンする
	void SpawnEnemyInBattleArea();

	void SpawnEnemy(AActor* _enemy, FTransform _tranceform);
	AActor* SpawnEnemy(ASpawnPoint* spawnPoint);

	UFUNCTION(BlueprintCallable, Category = "GameSystem")
	void DestroyEnemy(AActor* _enemy, bool BattleAreaEnemy = false);

	void ClearEnemy();

//--------------------------------------------キー入力---------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "GameSystem")
		void SetState_GameOver(float _time = 0);

	UFUNCTION(BlueprintCallable, Category = "GameSystem")
		void SetState_GameClear(float _time = 0);

	void SetSubAction(ESubAction _eSubAction);

//-----------------------------------------バトルエリア-------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Area")
		void SetIsOnBattleArea(bool bIsOnArea, TArray<class ASpawnPoint*> SpawnPoints, 
			AActor* Camera,
			class UProceduralMeshComponent* LeftMesh,
			class UProceduralMeshComponent* RightMesh, 
			class UProceduralMeshComponent* NearMesh
			);
	
	UFUNCTION(BlueprintCallable, Category = "Area")
		bool GetIsOnBattleArea();

	int32 GetBattleAreaEnemyNum();

//---------------------------------------ゲームUI---------------------------------------
public:
	void SetUI_Enemy(FName _ActorName, int32 _NowHp, int32 _MaxHp);

	UFUNCTION(BlueprintCallable, Category = "TitleUI")
		EPlayerType GetPlayerType();

	uint8 GetCurrentState();

	UFUNCTION(BlueprintCallable, Category = "GamerPlayerUI")
		float GetPlayerP();

	UFUNCTION(BlueprintCallable, Category = "TitleUI")
		ETitleState GetTitleState();

	UFUNCTION(BlueprintCallable, Category = "TitleUI")
		void SetTitleState(ETitleState _NewState);
	
///////////////////////////////////////////////////////////
///Player選択
void SelctorPlayerType();

class UTGS_GameInstance* GetGameInstance();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		TSubclassOf<class ACharacter> PlayerCharacterClassOne;			// プレイヤー1のキャラクター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		TSubclassOf<class ACharacter> PlayerCharacterClassTwo;			// プレイヤー2のキャラクター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		TSubclassOf<class ACharacter> PlayerCharacterClassThree;			// プレイヤー3のキャラクター

	AActor* GetPlayStartPoint();

	///////////////////////////////////////////////
	///カメラ処理
public:
	AActor* CameraActor;
	FVector GetCameraLocation();
	FRotator GetCameraRotation();
	AActor* GetCameraActor();

	void SetSplineCamera(AActor* _splineCamera);

	//UFUNCTION(BlueprintCallable, Category = "GameSystem")
	//	AActor* SetSplineCamera(AActor* _splineCamera);

//使い方
//　ATGS_GameMode* GameMode = Cast<ATGS_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
//　GameMode->SetViewTargetWithBlend(Actor, 0.5f);
	UFUNCTION(BlueprintCallable, Category = "GameSystem")
void SetViewTargetWithBlend(class AActor* NewViewTarget, float BlendTime = 0, enum EViewTargetBlendFunction BlendFunc = VTBlend_Linear, float BlendExp = 0, bool bLockOutgoing = false);

protected:
	UFUNCTION(BlueprintCallable, Category = "GameSystem")
	class ATGS_GameStateBase* GetGameState();

	ATGS_GameStateBase* GameState = nullptr;		//キャシュー


public:
	UFUNCTION(BlueprintCallable, Category = "Level")
		void MoveToInGameLevel();	
};
