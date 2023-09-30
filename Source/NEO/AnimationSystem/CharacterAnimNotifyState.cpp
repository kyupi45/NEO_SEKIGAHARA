// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimNotifyState.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"


/*
 * 関数名　　　　：NotifyBegin()
 * 処理内容　　　：通知開始時に行う処理
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生するアニメーション
 * 引数３　　　　：float TotalDuration・・・アニメーション再生総所要時間
 * 戻り値　　　　：なし
 */
void UCharacterAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	// オーナー取得
	AActor* OwnerActor = GetOwnerActor(MeshComp);
	if (!OwnerActor) { return; }


	// 通知開始時の処理
	NotifyBeginAction(OwnerActor, Animation,TotalDuration);
}


/*
 * 関数名　　　　：NotifyTick()
 * 処理内容　　　：通知中常に行う処理
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生中のアニメーション
 * 引数３　　　　：float FrameDeltaTime・・・1フレームの時間
 * 戻り値　　　　：なし
 */
void UCharacterAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	// オーナー取得
	AActor* OwnerActor = GetOwnerActor(MeshComp);
	if (!OwnerActor) { return; }


	// 通知中常に処理
	NotifyTickAction(OwnerActor,Animation,FrameDeltaTime);
}


/*
 * 関数名　　　　：NotifyEnd()
 * 処理内容　　　：通知終了時に行う処理
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生していたアニメーション
 * 戻り値　　　　：なし
 */
void UCharacterAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// オーナー取得
	AActor* OwnerActor = GetOwnerActor(MeshComp);
	if (!OwnerActor) { return; }


	// 通知終了時の処理
	NotifyEndAction(OwnerActor,Animation);
}


/*
 * 関数名　　　　：GetOwnerActor()
 * 処理内容　　　：オーナーをAActor*で取得
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 戻り値　　　　：オーナーをAActor*で取得
 */
AActor* UCharacterAnimNotifyState::GetOwnerActor(USkeletalMeshComponent* MeshComp)
{
	// プレイヤーのベースクラス取得
	AActor* pOwner = Cast<AActor>(MeshComp->GetOwner());
	if (!pOwner) { return nullptr; }

	return pOwner;
}