// Fill out your copyright notice in the Description page of Project Settings.


#include "NEOGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "ProceduralMeshComponent.h"
#include "NEO/GameSystem/SplineCamera.h"
#include "NEO/GameSystem/GameSystem_BattleArea.h"
#include "NEO/GameSystem/SpawnPoint.h"
#include "NEO/OdaBase.h"
#include "NEO/Enemy/EnamyBase.h"
#include "PlayerBase.h"
#include "NEOPlayerController.h"


ANEOGameMode::ANEOGameMode()
{

}


void ANEOGameMode::BeginPlay()
{
	Super::BeginPlay();

	// ゲームステートを取得
	pGameState = Cast<ANEOGameState>(UGameplayStatics::GetGameState(GetWorld()));

	// プレイヤーコントローラーを取得
	PlayerController = Cast<ANEOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ANEOGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// ゲームの状態を更新
	if (pGameState) 
	{
		// ゲームの状態更新
		pGameState->UpdateGameState(DeltaTime);
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("Game State is not found"));
		pGameState = Cast<ANEOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	}
}


/*
 * 関数名　　　　：SetViewTargetWithBlend()
 * 処理内容　　　：現在のカメラの情報を返す
 * 引数１　　　　：AActor* _newViewTarget・・・新しいカメラの情報
 * 引数２　　　　：float _blendTime・・・・・・切り替えにかける時間
 * 引数３　　　　：EViewTargetBlendFunction _blendFunc
 * 引数４　　　　：float _blendExp
 * 引数５　　　　：bool _bLockOutgoing
 * 戻り値　　　　：現在のカメラの情報
 */
void ANEOGameMode::SetViewTargetWithBlend(AActor* _newViewTarget, float _blendTime, EViewTargetBlendFunction _blendFunc, float _blendExp, bool _bLockOutgoing)
{
	// プレイヤーコントローラーを取得
	PlayerController = Cast<ANEOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// コントローラーがある時カメラをブレンド
	if (PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(_newViewTarget, _blendTime);
	}

	pCamera = _newViewTarget;
}

// カメラの初期設定
void ANEOGameMode::InitCameraOnPlayer()
{
	SetViewTargetWithBlend(PlayerCamera);
}

/*
 * 関数名　　　　：GetNowPlayerCamera()
 * 処理内容　　　：現在のカメラの情報を返す
 * 戻り値　　　　：現在のカメラの情報
 */
AActor* ANEOGameMode::GetNowPlayerCamera()const
{
	// バトルエリア内にいるかどうかでカメラを選択
	AActor* NowCamera = (bIsOnBattleArea) ? (pCamera) : (PlayerCamera);

	if (NowCamera)
	{
		return NowCamera;
	}
	
	return nullptr;
}


/*
 * 関数名　　　　：SetIsOnBattleArea()
 * 処理内容　　　：バトルエリアを起動する
 * 引数１　　　　：bool _IsBattleArea・・・・・・・・・・・・・バトルエリアを発動するか
 * 引数２　　　　：TArray<class ASpawnPoint*> SpawnPoints・・・出現場所の配列情報
 * 引数３　　　　：AActor* Camera・・・・・・・・・・・・・・・バトルエリアのカメラ情報
 * 引数４　　　　：UProceduralMeshComponent* LeftMesh・・・・・バトルエリアの壁の情報(左)
 * 引数５　　　　：UProceduralMeshComponent* RightMesh ・・・・バトルエリアの壁の情報(右)
 * 引数６　　　　：UProceduralMeshComponent* NearMesh・・・・・バトルエリアの壁の情報(手前)
 * 戻り値　　　　：なし
 */
void ANEOGameMode::SetIsOnBattleArea(bool _IsBattleArea,TArray<class ASpawnPoint*> SpawnPoints,
	AActor* Camera = nullptr,
	class UProceduralMeshComponent* LeftMesh = nullptr,
	class UProceduralMeshComponent* RightMesh = nullptr,
	class UProceduralMeshComponent* NearMesh = nullptr)
{
	// NULL Check
	if (Camera && LeftMesh && RightMesh && NearMesh) {
		BattleAreaMeshs.Reset();

		BattleAreaMeshs.Add(LeftMesh);
		BattleAreaMeshs.Add(RightMesh);
		BattleAreaMeshs.Add(NearMesh);

		BattleAreaCamera = Camera;

		//スポーンポイントをゲームステートに登録する
		BattleAreaSpawnPoints.Reset();
		for (auto spawnPoint : SpawnPoints) {
			BattleAreaSpawnPoints.Add(spawnPoint);
		}
	}


	//バトルエリアを有効化
	for (auto Mesh : BattleAreaMeshs) {
		if (Mesh) {
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			//-------------------コリジョンを有効化----------------------

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("MeshWall is not found"));
		}
	}

	//プレイヤーのカメラを固定カメラに変更
	if (Camera) 
	{
		SetViewTargetWithBlend(Camera);
	}

	//GameModeを使って、バトルエリアに敵をスポーンさせる。　＜設計が悪い＞
	SpawnEnemyInBattleArea();

	bIsOnBattleArea = _IsBattleArea;
}


/*
 * 関数名　　　　：SpawnEnemy()
 * 処理内容　　　：敵の出現処理
 * 引数１　　　　：出現場所の情報
 * 戻り値　　　　：なし
 */
AActor* ANEOGameMode::SpawnEnemy(ASpawnPoint* spawnPoint)
{
	//NULL Check
	if (!spawnPoint) {
		UE_LOG(LogTemp, Error, TEXT("SpawnPoint is not found"));
		return nullptr;
	}

	//Transformを取得する
	FTransform spawnTransform = spawnPoint->GetTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* spawn_Actor = GetWorld()->SpawnActor<AActor>(spawnPoint->GetSpawnActorClass(), spawnPoint->GetActorLocation(), spawnPoint->GetActorRotation(), SpawnParams);


	AOdaBase* Boss = Cast<AOdaBase>(spawn_Actor);
	AEnamyBase* Enemy = Cast<AEnamyBase>(spawn_Actor);

	if (Enemy) {
		Enemy->SetActorTransform(spawnTransform);
		Enemy->IsAreaEnemy = true;		//Flag Set 

		// エネミーを追加
		++BattleAreaEnemyCount;

		return Enemy;
	}
	else if (Boss) {
		Boss->SetActorTransform(spawnTransform);
		Boss->IsAreaEnemy = true;		//Flag Set 

		// エネミーを追加
		++BattleAreaEnemyCount;

		return Boss;
	}

	return nullptr;

}


/*
 * 関数名　　　　：SetCameraOnPlayer()
 * 処理内容　　　：プレイヤーのカメラ設定
 * 戻り値　　　　：なし
 */
void ANEOGameMode::SetCameraOnPlayer()
{
	if (PlayerController)
	{
		// プレイヤーが生きていることを確認
		if (!PlayerController->GetPlayerIsDead())
		{
			// バトルエリア内にいるかどうかでカメラを選択
			AActor* NowCamera = (bIsOnBattleArea)?(pCamera):(PlayerCamera);

			// プレイヤーのカメラに設定
			SetViewTargetWithBlend(NowCamera);
		}
	}
}


/*
 * 関数名　　　　：RestartGame()
 * 処理内容　　　：ゲームリセット
 * 戻り値　　　　：なし
 */
void ANEOGameMode::RestartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(),"GameMap");

}

/*
 * 関数名　　　　：DestroyEnemy()
 * 処理内容　　　：敵の削除処理
 * 戻り値　　　　：なし
 */
void ANEOGameMode::DestroyEnemy(AActor* _enemy, bool _bBattleAreaEnemy)
{
	if (_enemy)
	{
		for (int i = 0; i < Enemies.Num(); ++i)
		{
			if (Enemies[i] == _enemy)
			{
				// エネミー削除
				Enemies.Remove(_enemy);
				_enemy->Destroy();
				break;
			}
		}


		// エリア内のエネミーだったらカウントを減らす
		if (_bBattleAreaEnemy)
		{
			--BattleAreaEnemyCount;


			if (BattleAreaEnemyCount == 0 && bIsOnBattleArea)
			{
				ExitBattleArea();
			}
		}
	}
}


/*
 * 関数名　　　　：SpawnEnemyInBattleArea()
 * 処理内容　　　：バトルエリアに敵を出現させる
 * 戻り値　　　　：なし
 */
void ANEOGameMode::SpawnEnemyInBattleArea()
{
	//Check SpawnPoints
	if (BattleAreaSpawnPoints.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("SpawnPoints is not found"));
		return;
	}

	//スポーン
	UWorld* World = GetWorld();
	if (!World) return;


	for (ASpawnPoint* spawnPoint : BattleAreaSpawnPoints) {
		if (!spawnPoint) continue; //Check SpawnPoint

		//敵を生成する
		Enemies.Add(SpawnEnemy(spawnPoint));
	}
}


/*
 * 関数名　　　　：ExitBattleArea()
 * 処理内容　　　：バトルエリアから抜ける処理
 * 戻り値　　　　：なし
 */
void ANEOGameMode::ExitBattleArea()
{
	// 策破壊イベント
	AGameSystem_BattleArea* Area = Cast<AGameSystem_BattleArea>(GetNowPlayerCamera());
	if (Area) 
	{
		Area->ExitAreaEvent();
	}


	//バトルエリアを無効化
	bIsOnBattleArea = false;
	for (auto Mesh : BattleAreaMeshs) {
		if (Mesh) {
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("MeshWall is not found"));
		}
	}


	//固定カメラをプレイヤーのカメラに変更
	if (!PlayerController->GetPlayerIsDead())
	{
		if (PlayerCamera)
		{
			SetViewTargetWithBlend(PlayerCamera, 1.f);
		}
	}
	else
	{
		//Log
		UE_LOG(LogTemp, Warning, TEXT("Player.GetOwner() : <CameraActor> is not found"));
	}
}