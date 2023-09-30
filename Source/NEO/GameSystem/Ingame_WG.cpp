// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame_WG.h"

void UIngame_WG::SetWidgetValue(FName _EnemyName, int32 _NowHp, int32 _MaxHp)
{
	WidgetValue_EnemyName = _EnemyName;
	WidgetValue_EnemyHP = _NowHp;
	WidgetValue_EnemyMaxHP = _MaxHp;
}
