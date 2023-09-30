// Fill out your copyright notice in the Description page of Project Settings.

#include "BossHPLock_AN.h"
#include "OdaBase.h"

void UBossHPLock_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

	// 取得
	TObjectPtr< AOdaBase> aOda = Cast< AOdaBase>(MeshComp->GetOwner());
	//NULLチェック
	if (aOda == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveOda is not Found"));
		return;
	}
	// HPを減らさないようにする
	aOda->HPLock();
}