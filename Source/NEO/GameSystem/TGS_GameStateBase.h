// 更新日：2023/6/5		更新者：董		更新内容：ゲームの状態を管理するクラスを作成

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "InputCharacter.h"
#include "Engine/TargetPoint.h"

#include "TGS_GameStateBase.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	EGame_None UMETA(DisplayName = "Game_None"),					// ゲームスタート前

	EGame_Title UMETA(DisplayName = "Game_Title"),					// タイトル画面
	EGame_Playing UMETA(DisplayName = "Game_Playing"),				// ゲーム中	
	EGame_InBattleArea UMETA(DisplayName = "EGame_InBattleArea"),	// バトルエリア
	EGame_Over UMETA(DisplayName = "Game_Over"),					// ゲームオーバー
	EGame_Clear UMETA(DisplayName = "Game_Clear"),					// ゲームクリア
	EGame_Opening UMETA(DisplayName = "Game_Opening"),				//オープニング
	EGame_ReSpawnPlayer UMETA(DisplayName = "Game_ReSpawnPlayer"),	// プレイヤーの復活
	EGame_Menu UMETA(DisplayName = "Game_Menu"),					// メニュー画面	
	EGame_Pause UMETA(DisplayName = "Game_Pause"),					// ポーズ画面

	EGame_Unknown UMETA(DisplayName = "Game_Unknown")
};

UENUM(BlueprintType)
enum class ENextLevel : uint8
{
	ENextLevel_None UMETA(DisplayName = "NextLevel_None"),					// None
	ENextLevel_Title UMETA(DisplayName = "NextLevel_Title"),				// タイトル画面
	ENextLevel_Playing UMETA(DisplayName = "NextLevel_Playing"),			// ゲーム中
	ENextLevel_Over UMETA(DisplayName = "NextLevel_Over"),					// ゲームオーバー
	ENextLevel_Clear UMETA(DisplayName = "NextLevel_Clear"),				// ゲームクリア
	ENextLevel_Opening UMETA(DisplayName = "NextLevel_Opening"),			// オープニング
};

UENUM(BlueprintType)
enum class EChangeLevel : uint8
{
	EChangeLevel_None UMETA(DisplayName = "ChangeLevel_None"),					// None
	EChangeLevel_Title UMETA(DisplayName = "ChangeLevel_Title"),				// タイトル画面
	EChangeLevel_Playing UMETA(DisplayName = "ChangeLevel_Playing"),			// ゲーム中
	EChangeLevel_Over UMETA(DisplayName = "ChangeLevel_Over"),					// ゲームオーバー

};

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	EPlayerType_None UMETA(DisplayName = "PlayerType_None"),					// None
	EPlayerType_1 UMETA(DisplayName = "PlayerType_1"),							// プレイヤー1
	EPlayerType_2 UMETA(DisplayName = "PlayerType_2"),							// プレイヤー2
	EPlayerType_3 UMETA(DisplayName = "PlayerType_3"),							// プレイヤー3
};

UENUM(BlueprintType)
enum class ETitleState : uint8
{
	ETitle_None UMETA(DisplayName = "ETitle_None"),								// None
	ETitle_DemoScene UMETA(DisplayName = "ETitle_DemoScene"),					// DEMO動画
	ETitle_TitleScene UMETA(DisplayName = "ETitle_TitleScene"),					// タイトルシーン
};

/**
 *
 */
UCLASS()
class NEO_API ATGS_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
	ACharacter* PlayerCharacter;					// プレイヤーのキャラクター

	TArray<class AActor*> Enemies;					// 敵の配列
	EGameState ECurrentState;						// 現在のゲームの状態

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALevelName")
		FName GameTitleLevelName = FName(TEXT("GameTitle"));	// ゲームタイトルのレベル名

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALevelName")
		FName GamePlayLevelName = FName(TEXT("GamePlay"));		// ゲームプレイのレベル名

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALevelName")
		FName GameOverLevelName = FName(TEXT("GameOver"));		// ゲームオーバーのレベル名

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALevelName")
		FName GameClearLevelName = FName(TEXT("GameClear"));		// ゲームオーバーのレベル名

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALevelName")
		FName GameOpeningLevelName = FName(TEXT("GameOpening"));	// ゲームオープニングのレベル名


public:
	FDateTime GameStartTime;						// ゲームの開始時間
	FDateTime GameEndTime;							// ゲームの終了時間


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameOver")
		bool bIsGameOver;								// 外部からゲームオーバーを設定　するためのフラグ


public:
	ATGS_GameStateBase();

	//-------------------------------------------------------------------------------------------------------

	void InitGameState();							// ゲームの状態を初期化
	void UpdateGameState(float DeltaTime);			// ゲームの状態を更新

	void AddEnemy(class AActor* Enemy);
	void RemoveEnemy(class AActor* Enemy);
	TArray<class AActor*> GetEnemies() const;
	void ClearEnemies();							// 敵の配列をクリア

	void SetCurrentState(EGameState NewState) { ECurrentState = NewState; }
	inline const EGameState& GetCurrentState() const { return ECurrentState; }		// 最適化ために　return const reference

	//CurrentStateの初期化
	void InitCurrentState();

	//PlayerTypeの初期化
	void InitPlayerType();


	bool IsGameOver();								// ゲームオーバーかどうかを判定

	bool bIsGameClear;								// ゲームクリアかどうかを判定
	bool IsGameClear();								// ゲームクリアかどうかを判定

	//-------------------------------------------------------------------------------------------------------
protected:
	//ゲームタイトルの処理
	void OnGameTitle();
	//ゲームプレイ中の処理
	void OnGamePlaying(float DeltaTime);
	//ゲームオーバーの処理
	void OnGameOver();
	//ゲームクリアの処理
	void OnGameClear();
	//ゲームオープニング
	void OnGameOpening();

	//プレイヤーの復活処理
	void OnReSpawnPlayer();
	//メニュー画面の処理
	void OnGameMenu();
	//ポーズ画面の処理
	void OnGamePause();
	//バトルエリアの処理
	void OnInBattleArea();

	//-------------------------------------------------------------------------------------------------------
		//バトルエリアに入る
	void EnterBattleArea();
	//バトルエリアから出る
	void ExitBattleArea();

	//-----------------------------------コリジョンの当たり判定の切り替え--------------------------------------
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Area)
		bool NowBattleArea;



	//バトルエリアの壁Actors
	TArray<class AActor*> BattleAreaWalls;
	//バトルエリアの壁Actorsのタグ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
		FName AreaWallsTag;

	//バトルエリアの初期化処理
	void InitBattleArea();

public:
	TArray<class ASpawnPoint*> BattleAreaSpawnPoints;
	int32 BattleAreaEnemyCount = 0;

public:
	//バトルエリアのフラグ
	bool bIsOnBattleArea;

	//バトルエリアのカメラ
	AActor* BattleAreaCamera;

	TArray<class UProceduralMeshComponent*> BattleAreaMeshs;

	AActor* GetBattleAreaCamera();

	AActor* SplineCamera;

//---------------------------------------------------------------------------------------------------------------------
protected:
	//ゲームインスタンスの取得
	class UTGS_GameInstance* GetGameInstance();


	inline void ChangeLevel(FName NextLevelName);

public:
	void ChangeNextLevel(ENextLevel NextLevel);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		EChangeLevel EchangeLevel;						// レベルの変更

	//EChangeLevel GetChangeLevel();


protected:
	//----------------------------------------------------------------------------
		//現在のサブアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		ESubAction CurrentSubAction;

public:
	ESubAction UseSubAction();
	void SetSubAction(ESubAction _subAction);
	inline const ESubAction& GetCurrentSubAction() const { return CurrentSubAction; }


///////////////////////////////////////////
///UI

protected:			// ゲームメニューのUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		TSubclassOf<class UUserWidget> Widget_PlayerStatusClass;		// プレイヤーのステータスのUI

public:
	EPlayerType ECurrentPlayerType;

	UFUNCTION(CallInEditor)
		void NextPlayerType();

	UFUNCTION(CallInEditor)
		void BackPlayerType();

	ETitleState ECurrentTitleState;

//////////////////////////////////////////////////////////////////////////
///プレイヤーキャラクターの選択
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		TSubclassOf<class ACharacter> PlayerCharacterClassOne;			// プレイヤー1のキャラクター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		TSubclassOf<class ACharacter> PlayerCharacterClassTwo;			// プレイヤー2のキャラクター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		TSubclassOf<class ACharacter> PlayerCharacterClassThree;			// プレイヤー3のキャラクター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		int32 Life = 2;

public:
	void MoveToIngameLevel();
};