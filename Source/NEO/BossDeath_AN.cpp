// Fill out your copyright notice in the Description page of Project Settings.


#include "BossDeath_AN.h"
#include "OdaBase.h"


void UBossDeath_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// 取得
	TObjectPtr< AOdaBase> aOda = Cast< AOdaBase>(MeshComp->GetOwner());
	//NULLチェック
	if (aOda == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveOda is not Found"));
		return;
	}
	//体力が0以下になったときの処理
	aOda->WorldTimeReturn();
}