// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TGS_GameStateBase.h"

#include "TGS_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API UTGS_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
//コンストラクタ
	UTGS_GameInstance(const FObjectInitializer& ObjectInitializer);

protected:
	//保存のデータ
	EGameState BackupGameState;

	//PlayerTypeの状態を保存
	EPlayerType BackupPlayerType;

	int RemainingLife;
	////Playerの状態を保存
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameState")
	//EPlayerState BackupPlayerState;

public:

	//ゲームの状態を保存
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void SaveGameStateData(EGameState& _SaveGameState);

	//ゲームの状態を取得
	UFUNCTION(BlueprintCallable, Category = "GameState")
	EGameState LoadGameStateData() const;

	//プレイヤーの状態を保存
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void SavePlayerType(EPlayerType& _SavePlayerType);

	//プレイヤーの状態を取得
	UFUNCTION(BlueprintCallable, Category = "GameState")
	EPlayerType LoadPlayerType() const;

	void SaveRemainingLife(int _life);
	int LoadRemainingLife() const;

};
