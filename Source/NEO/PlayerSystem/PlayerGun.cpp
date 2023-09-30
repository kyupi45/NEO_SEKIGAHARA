// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGun.h"
#include "PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "Engine/AssetManager.h"
#include "Async/Async.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerGun::APlayerGun()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// キャラクターコンポーネント取得
	CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->MaxWalkSpeed = 500.f;

	// プレイヤーのデータセット
	SetupPlayerData();

}

void APlayerGun::BeginPlay()
{
	Super::BeginPlay();

}

void APlayerGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerGun::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}

// プレイヤーのデータを初期化
void APlayerGun::SetupPlayerData()
{
	Super::SetupPlayerData();

	// 武器のメッシュのパス
	TCHAR* WeaponAssetPath = TEXT("/Game/0122/Player/Weapon/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight");

	// セット
	SetupWeaponMesh(WeaponSkeletalMesh, WeaponAssetPath, "WeaponMesh");

	// アニメーションセットアップ
	TCHAR* AnimationAssetPaths[2];

	// アニメーションアセットのパス
	AnimationAssetPaths[0] = TEXT("/Game/0207/GunAnimation/montage/GunCombo_1.GunCombo_1");
	//AnimationAssetPaths[0] = TEXT("/Game/0122/Player/Animation/Montage/Combo/SwordCombo");

	AnimationAssetPaths[1] = TEXT("/Game/0122/Player/Animation/Montage/Combo/SwordCombo2");

	for (int i = 0; i < 2; ++i)
	{
		PlayerAnimation.ComboAttack[i] = GetAnimationAsset(AnimationAssetPaths[i]);
	}
}

//// 攻撃
//void APlayerGun::ComboAttack(int AttackNum)
//{
//	//if (!IsAttacking)
//	//{
//	//	// 攻撃中フラグオン
//	//	IsAttacking = true;
//	//}
//	//else
//	//{
//	//	// コンボ可能な時,継続
//	//	if (CanCombo)
//	//	{
//	//		// ラストアタックまでコンボ継続
//	//		if (ComboStartSectionNames[ComboIndex] != ComboStartSectionNames.Last())
//	//		{
//	//			++ComboIndex;
//	//		}
//	//	}
//	//}
//
//	//// 攻撃のアニメーション再生
//	//PlayAnimation(ComboAnimMontages[AttackNum], ComboStartSectionNames[ComboIndex]);
//
//}
//
//// 攻撃
//void APlayerGun::Combo1()
//{
//	//// コントロール可能か
//	//if (!IsControl) { return; }
//
//	//// 攻撃
//	//Attack(0);
//
//}
//
//
//// 攻撃
//void APlayerGun::Combo2()
//{
//	//// コントロール可能か
//	//if (!IsControl) { return; }
//
//	//// 攻撃
//	//Attack(1);
//
//}