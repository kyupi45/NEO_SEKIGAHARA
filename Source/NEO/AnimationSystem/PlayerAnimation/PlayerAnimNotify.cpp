// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimNotify.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "NEO/PlayerSystem/PlayerBase.h"


/*
 * 関数名　　　　：CanBePlaced()
 * 処理内容　　　：エディタ上に公開するかどうか
 * 引数１　　　　：UAnimSequenceBase* Animation・・・再生するアニメーション
 * 戻り値　　　　：true・falseで公開判定
 */
bool UPlayerAnimNotify::CanBePlaced(UAnimSequenceBase* Animation) const
{
    // モンタージュのみに公開
    if (Animation->IsA(UAnimMontage::StaticClass()))
    {
        return true;
    }


    return false;
}


/*
 * 関数名　　　　：NotifyAction()
 * 処理内容　　　：通知時に行う処理
 * 引数１　　　　：AActor* _Owner・・・オーナー
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生するアニメーション
 * 戻り値　　　　：なし
 */
void UPlayerAnimNotify::NotifyAction(AActor* _Owner, UAnimSequenceBase* Animation)
{
    // プレイヤーのベースクラス取得
    APlayerBase* pPlayer = Cast<APlayerBase>(_Owner);

    if (pPlayer)
    {
        // 当たり判定開始の処理が来た時
        if (NotifyType == EPlayerNotifyType::NotifyType_SetCollision)
        {
            pPlayer->SetCollision();
        }
        // コンボ継続の通知が来た時
        else if (NotifyType == EPlayerNotifyType::NotifyType_SaveCombo)
        {
            pPlayer->ContinuationCombo();
        }
        // コンボリセットの通知が来た時
        else if (NotifyType == EPlayerNotifyType::NotifyType_ResetCombo)
        {
            pPlayer->ResetCombo();
        }
        // コントロール可能になる通知が来た時
        else if (NotifyType == EPlayerNotifyType::NotifyType_CanControl)
        {
            pPlayer->SetControl(true);
        }
        // アニメーションを遅くする通知が来た時
        else if (NotifyType == EPlayerNotifyType::NotifyType_SlowDown)
        {
            pPlayer->SlowDownDeathAnimationRate();
        }
        else if (NotifyType == EPlayerNotifyType::NotifyType_StopMontage)
        {
            pPlayer->StopMontage();
        }
    }
}

