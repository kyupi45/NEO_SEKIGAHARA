// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInfo_UI_API.h"
#include "TGS_GameMode.h"

/**
 * キャラクター情報を取得する
 *
 * @return	キャラクター情報のタプル。見つからなかった場合は Not Found、0、0 のタプルを返す。
 */
TTuple<FName, int, int> UCharacterInfo_UI_API::GetCharacterInfo()
{
	return TTuple<FName, int, int>("Not Found", 0, 0);
}

/**
 * UI を敵キャラクター用に設定する
 *
 * @remarks	ATGS_GameMode が見つからなかった場合は Warning ログを出力する。
 */
void UCharacterInfo_UI_API::SetUI_Enemy()
{
	ATGS_GameMode* GameMode = Cast<ATGS_GameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		TTuple<FName, int, int> CharacterInfo = GetCharacterInfo();

		GameMode->SetUI_Enemy(CharacterInfo.Get<0>(), CharacterInfo.Get<1>(), CharacterInfo.Get<2>());
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT("GameMode is not found") );
	}
}
