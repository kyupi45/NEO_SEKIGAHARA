// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAssistComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "NiagaraComponent.h"									
#include "NiagaraFunctionLibrary.h"		
#include "NEOPlayerController.h"

#define DIRECTION_Y (90.f)

// Sets default values for this component's properties
UActionAssistComponent::UActionAssistComponent()
	: bUseCorrectAttackAngle(true)
	, bUseHitStop(true)
	, bUseHitEffect(true)
	, bUseFaceCamera(true)
{
	// Tick()を毎フレーム呼ばないようにする
	PrimaryComponentTick.bCanEverTick = false;

	RayLength_CorrectAngle = 300.f;
	MaxCorrectAngle = 10.f;
}


// Called when the game starts
void UActionAssistComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UActionAssistComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


/*
 * 関数名　　　　：CorrectAttackAngle()
 * 処理内容　　　：角度を補正して攻撃を当たりやすくする(直線状にいる敵)
 * 戻り値　　　　：なし
 */
void UActionAssistComponent::CorrectAttackAngle()
{
	// 機能のオン・オフ
	if (!bUseCorrectAttackAngle) { return; }

	// オーナー取得
	AActor* Owner = GetOwner();

	//　前方にいるエネミー取得
	AActor* InFrontOfEnemy = GetFrontActor();

	if (Owner && InFrontOfEnemy)
	{
		// プレイヤーと敵の方向を取得する
		FVector direction = InFrontOfEnemy->GetActorLocation() - Owner->GetActorLocation();
		direction.Normalize();

		// プレイヤーの向きを取得する
		FRotator CurrentRotation = Owner->GetActorRotation();

		// 補正角度取得
		float CorrectAngle = direction.Rotation().Yaw;

		// 最大補正角度を超えていたら値を補正
		if ( (0.f < CorrectAngle && CorrectAngle < DIRECTION_Y - MaxCorrectAngle) || (-180.f < CorrectAngle && CorrectAngle < DIRECTION_Y - MaxCorrectAngle))
		{
			CorrectAngle = -DIRECTION_Y - MaxCorrectAngle;
		}
		else if ( (180.f > CorrectAngle && CorrectAngle > -DIRECTION_Y + MaxCorrectAngle) || (0.f > CorrectAngle && CorrectAngle > -DIRECTION_Y + MaxCorrectAngle))
		{
			CorrectAngle = DIRECTION_Y + MaxCorrectAngle;
		}

		// プレイヤーの向きを変更する
		FRotator NewRotation(CurrentRotation.Pitch, CorrectAngle, CurrentRotation.Roll);
		Owner->SetActorRotation(NewRotation);
	}
}


/*
 * 関数名　　　　：HitStop()
 * 引数１　　　　：float _speedDuringHitStop・・・止まっているときのスピード(1.fで通常の速さ)
 * 引数２　　　　：float _stopTime・・・止める時間
 * 処理内容　　　：ヒットストップを起こす
 * 戻り値　　　　：なし
 */
void UActionAssistComponent::HitStop(float _speedDuringHitStop,float _stopTime)
{
	// 機能のオン・オフ
	if (!bUseHitStop) { return; }

	// 処理内容
	ACharacter* Character = Cast<ACharacter>(GetOwner());		//CharacterGet
	if (Character == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("AttackAssitComponent: Character is nullptr"));
		return;
	}

	// HitStopを開始
	Character->GetMesh()->GlobalAnimRateScale = _speedDuringHitStop;

	// HitStopを停止
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(TimerHandle_HitStop, this, &UActionAssistComponent::EndHitStop, _stopTime, false);
}


/*
 * 関数名　　　　：SpawnEffect()
 * 引数１　　　　：UNiagaraSystem* _hitParticle・・・出現させるエフェクト
 * 引数２　　　　：FVector _spawnPos・・・・・・・・・出現させる場所
 * 処理内容　　　：エフェクトを出現する
 * 戻り値　　　　：なし
 */
void UActionAssistComponent::SpawnEffect(UNiagaraSystem* _hitParticle, FVector _spawnPos)
{
	// 機能のオン・オフ
	if (!bUseHitEffect) { return; }

	// エフェクトの情報が取得できなければリターン
	if (!_hitParticle) { return; }

	//エフェクトを出す
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetOwner()->GetWorld(), _hitParticle, _spawnPos);
}


/*
 * 関数名　　　　：CameraShake()
 * 引数１　　　　：TSubclassOf<UCameraShakeBase> _shakePattern・・・揺らすパターン
 * 引数２　　　　：float _scale・・・・・・・・・・・・・・・・・・・・強さ
 * 処理内容　　　：カメラシェイク
 * 戻り値　　　　：なし
 */
void UActionAssistComponent::CameraShake(TSubclassOf<UCameraShakeBase> _shakePattern, float _scale /*= 1.f*/)
{
	// プレイヤー取得
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) { return; }

	// マネージャー取得
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (!CameraManager) { return; }

	// カメラシェイク開始
	CameraManager->StartCameraShake(_shakePattern, _scale);
}


/*
 * 関数名　　　　：PlayAnimation()
 * 処理内容　　　：プレイヤーのアニメーション再生(再生中は操作不可)
 * 引数１　　　　：UAnimMontage* _toPlayAnimMontage ・・・再生するアニメーション
 * 引数２　　　　：FName _startSectionName・・・・・・・・コンボの何段目から再生するか
 * 引数３　　　　：float _playRate・・・・・・・・・・・・アニメーションの再生速度
 * 戻り値　　　　：なし
 */
void UActionAssistComponent::PlayAnimation(UAnimMontage* _toPlayAnimMontage, FName _startSectionName /*= "None"*/, float _playRate /*= 1.f*/)
{
	// キャラクタークラスにキャスト
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) { return; }

	// 移動不能へ
	//Owner->GetCharacterMovement()->DisableMovement();

	// 再生するアニメーションを格納
	UAnimMontage* toPlayAnimMontage = _toPlayAnimMontage;

	// アニメーション再生
	if (toPlayAnimMontage != nullptr)
	{
		Owner->PlayAnimMontage(_toPlayAnimMontage, _playRate, _startSectionName);
	}
}


/*
 * 関数名　　　　：PlaySound()
 * 処理内容　　　：音声再生
 * 引数１　　　　：USoundBase* _sound_Obj・・・音声のアセット
 * 引数２　　　　：音声を開始する時間
 * 戻り値　　　　：なし
 */
void UActionAssistComponent::PlaySound(USoundBase* _sound_Obj,float _startTime /*= 0*/)
{
	if (_sound_Obj)
	{
		// 再生(7番目の引き数でオーナーアクターを自分に)
		UGameplayStatics::PlaySound2D(GetWorld(), _sound_Obj, 1.0f, 1.0f, _startTime, nullptr, GetOwner());
	}
}


/*
 * 関数名　　　　：GetFrontActor()
 * 処理内容　　　：敵が直線状にいるか判定
 * 戻り値　　　　：見つけた敵の情報を返す
 */
AActor* UActionAssistComponent::GetFrontActor()
{
	// 所有者の情報取得
	AActor* pOwner = GetOwner();

	if (pOwner)
	{
		// レイを飛ばす
		// 飛ばす方向指定
		float Rotation_Z = pOwner->GetActorRotation().Yaw;
		float LineDirection = (Rotation_Z > 0) ? (RayLength_CorrectAngle) : (-RayLength_CorrectAngle);

		// 始点
		FVector start = pOwner->GetActorLocation();

		// 終点
		FVector end = FVector(start.X, start.Y + LineDirection, start.Z);

		// 自身を除く
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(pOwner);

		// 当たったオブジェクト格納用
		FHitResult OutHit;

		// ヒットした場合true
		bool isHit = GetWorld()->LineTraceSingleByChannel(OutHit, start, end, ECC_WorldStatic, CollisionParams);
		//DrawDebugLine(GetWorld(), start, end, FColor::Green, true, 1.0f);

		if (isHit)
		{
			AActor* HitEnemy = OutHit.GetActor();

			// "Enemy"タグを持っているActorのみを返す
			if (HitEnemy->ActorHasTag("Enemy"))
			{
				return HitEnemy;
			}
		}
	}

	return nullptr;
}


/*
 * 関数名　　　　：EndHitStop()
 * 処理内容　　　：ヒットストップ終了
 * 戻り値　　　　：なし
 */
void UActionAssistComponent::EndHitStop()
{
	//CharacterGet
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("AttackAssitComponent: Character is nullptr"));
		return;
	}

	//HitStopを停止
	Character->GetMesh()->GlobalAnimRateScale = 1.f;
}


/*
 * 関数名　　　　：OwnerParallelToCamera()
 * 引数			 ：bool _lookRight・・・現在右を向いているか
 * 処理内容　　　：オーナーをカメラに向ける
 * 戻り値　　　　：なし
 */
void UActionAssistComponent::OwnerParallelToCamera(bool _lookRight)
{
	// 機能のオン・オフ
	if (!bUseFaceCamera) { return; }

	// プレイヤーコントローラー取得
	ANEOPlayerController* PlayerController = Cast<ANEOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));;
	if (!PlayerController) { return; }


	// カメラの角度を取得
	const FRotator CameraRotation = PlayerController->GetNowCameraRotation();


	// カメラのピッチとヨー
	double CameraPitch = CameraRotation.Pitch;
	double CameraYaw = CameraRotation.Yaw;

	// 角度補正
	if (!_lookRight)
	{
		CameraYaw += DIRECTION_Y;

	}
	else
	{
		CameraPitch = -CameraPitch;
		CameraYaw -= DIRECTION_Y;
	}

	// 新しい角度
	FRotator OwnerAngle = FRotator(0.0, CameraYaw, CameraPitch);

	// 回転
	GetOwner()->SetActorRotation(OwnerAngle);
}


// 壁とのレイキャストを行う関数
bool UActionAssistComponent::WallCheck(float _lineLength)
{
	// オーナー取得
	AActor* pOwner = GetOwner();

	if (!pOwner) { return false; }

	// レイキャストを実行する際のパラメータを設定する
	// レイキャストの開始位置はキャラクターの現在位置
	float Rotation_Z = pOwner->GetActorRotation().Yaw;
	float LineLength = (Rotation_Z > 0) ? (_lineLength) : (-_lineLength);

	// 始点
	FVector start = pOwner->GetActorLocation();
	// 終点
	FVector end = FVector(start.X, start.Y + LineLength, start.Z);

	//UE_LOG(LogTemp, Warning, TEXT("capsuleHeigth : %f"), Capsule->GetScaledCapsuleHalfHeight());
	//UE_LOG(LogTemp, Warning, TEXT("capsuleHeigth / 2 : %f"), Capsule->GetScaledCapsuleHalfHeight() / 2.0f);

	//UE_LOG(LogTemp, Warning, TEXT("StartLocation : %f"), StartLocation.Z);
	//UE_LOG(LogTemp, Warning, TEXT("EndLocation : %f"), EndLocation.Z);


	FCollisionQueryParams CollisionParams;
	// オーナー自身は除外
	CollisionParams.AddIgnoredActor(pOwner);

	FHitResult HitResult;

	// レイトレースを行う
	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, start, end, ECC_WorldStatic, CollisionParams);


	return IsHit;
}
