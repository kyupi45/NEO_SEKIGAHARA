// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttacked_AN.h"
#include "OdaBase.h"

void UBossAttacked_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// 取得
	TObjectPtr< AOdaBase> aOda = Cast< AOdaBase>(MeshComp->GetOwner());
	//NULLチェック
	if (aOda == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveOda is not Found"));
		return;
	}
	// プレイヤーのHPを減らさないようにする
	aOda->toPlayerAttacked();
}