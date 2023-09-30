
// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimNotifyState.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "NEO/PlayerSystem/PlayerBase.h"


/*
 * 関数名　　　　：NotifyAction()
 * 処理内容　　　：通知時に行う処理
 * 引数１　　　　：AActor* _Owner・・・オーナー
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生するアニメーション
 * 引数３　　　　：float TotalDuration・・・再生にかかる時間
 * 戻り値　　　　：なし
 */
void UPlayerAnimNotifyState::NotifyBeginAction(AActor* _Owner, UAnimSequenceBase* Animation, float TotalDuration)
{
    APlayerBase* pPlayer = Cast<APlayerBase>(_Owner);


    if (pPlayer)
    {
        // アニメーションでの移動開始
        if (NotifyType == EPlayerNotifyStateType::NotifyType_RootMotion_Attack1)
        {
            pPlayer->SetEnableRootMotion(true,2.f);
        }
        else if (NotifyType == EPlayerNotifyStateType::NotifyType_RootMotion_Attack2)
        {
            pPlayer->SetEnableRootMotion(true,5.f);
        }
        else if (NotifyType == EPlayerNotifyStateType::NotifyType_RootMotion_Attack3)
        {
            pPlayer->SetEnableRootMotion(true,8.f);
        }
        else if (NotifyType == EPlayerNotifyStateType::NotifyType_RootMotion_KnockBack)
        {
            pPlayer->SetEnableRootMotion(true, -5.f);
        }
        else if (NotifyType == EPlayerNotifyStateType::NotifyType_SetIsInvincibility)
        {
            pPlayer->SetInvincibility(true);
        }

    }
}


/*
 * 関数名　　　　：NotifyAction()
 * 処理内容　　　：通知時に行う処理
 * 引数１　　　　：AActor* _Owner・・・オーナー
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生するアニメーション
 * 戻り値　　　　：なし
 */
void UPlayerAnimNotifyState::NotifyEndAction(AActor* _Owner, UAnimSequenceBase* Animation)
{
    APlayerBase* pPlayer = Cast<APlayerBase>(_Owner);


    if (pPlayer)
    {
        // アニメーションでの移動終了
        if (NotifyType == EPlayerNotifyStateType::NotifyType_RootMotion_Attack1)
        {
            pPlayer->SetEnableRootMotion(false);
        }
        else if (NotifyType == EPlayerNotifyStateType::NotifyType_RootMotion_Attack2)
        {
            pPlayer->SetEnableRootMotion(false);
        }
        else if (NotifyType == EPlayerNotifyStateType::NotifyType_RootMotion_Attack3)
        {
            pPlayer->SetEnableRootMotion(false);
        }
        else if (NotifyType == EPlayerNotifyStateType::NotifyType_RootMotion_KnockBack)
        {
            pPlayer->SetEnableRootMotion(false);
        }
        else if (NotifyType == EPlayerNotifyStateType::NotifyType_SetIsInvincibility)
        {
            pPlayer->SetInvincibility(false);
        }
    }
}


