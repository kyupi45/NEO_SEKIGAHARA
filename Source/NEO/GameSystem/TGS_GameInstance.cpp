// Fill out your copyright notice in the Description page of Project Settings.


#include "TGS_GameInstance.h"


UTGS_GameInstance::UTGS_GameInstance(const FObjectInitializer& ObjectInitializer)
{
	BackupGameState = EGameState::EGame_Title;	// Default
	BackupPlayerType = EPlayerType::EPlayerType_1;
}

void UTGS_GameInstance::SaveGameStateData(EGameState& _SaveGameState)
{
	BackupGameState = _SaveGameState;			// Save
}

EGameState UTGS_GameInstance::LoadGameStateData() const
{
	return BackupGameState;						// Load	
}

void UTGS_GameInstance::SavePlayerType(EPlayerType& _SavePlayerType)
{
	BackupPlayerType = _SavePlayerType;
}

EPlayerType UTGS_GameInstance::LoadPlayerType() const
{
	return BackupPlayerType;
}

void UTGS_GameInstance::SaveRemainingLife(int _life)
{
	RemainingLife = _life;
}

int UTGS_GameInstance::LoadRemainingLife() const
{
	return RemainingLife;
}
