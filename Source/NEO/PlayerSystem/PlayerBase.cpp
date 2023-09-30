// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NEOPlayerController.h"
#include "NEOGameMode.h"

// Sets default values
APlayerBase::APlayerBase()
	: IsControl(true)
	, IsAIPlayer(true)
	, IsRunning(false)
	, IsLookRight(true)
	, IsJumping(false)
	, IsInvincibility(false)
	, IsHoldWeapon(true)
	, IsDeath(false)
	, frames(0.f)
	, IsAttacking(false)
	, CanCombo(false)
	, ComboIndex(0)
{
	// Tick()をマイフレーム呼ぶかどうかの判定
	PrimaryActorTick.bCanEverTick = true;

	// プレイヤーの設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// プレイヤーに回転の制限
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// タグ設定
	Tags.Add("Player");

	// キャラクターコンポーネント取得
	CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->MaxWalkSpeed = 500.f;

	// アタックアシストコンポーネント作成
	ActionAssistComp = CreateDefaultSubobject<UActionAssistComponent>(TEXT("AttackAssist"));

	// コリジョンイベントを設定
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnOverlap);

	// Playerのセットアップ
	SetupPlayerData();
}


// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	// ゲームモード取得
	pGameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// コントローラー取得
	PlayerController = Cast<ANEOPlayerController>(Controller);

	// 入力の設定
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainActionMapping.DefaultMappingContext, 0);
		}
	}

	// 武器をSpawn
	if (WeaponClass && !Weapon)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);

		if (Weapon)
		{
			WeaponType = Weapon->GetWeaponType();
			Weapon->AttachToHand(this, SocketName[int32(WeaponType)], EOwnerType::OwnerType_Player);
		}
	}
}


// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ジャンプ中のみ処理
	if (IsJumping)
	{
		Jump();
	}

	// アニメーションに合わせて移動
	if (EnableRootMotion && !ActionAssistComp->WallCheck(50.f) && !ActionAssistComp->WallCheck(-50.f))
	{
		RootMotion(AnimationMoveValue);
	}
}


// 入力のバインド
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 移動
		EnhancedInputComponent->BindAction(MainActionMapping.MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);

		// 走る							
		EnhancedInputComponent->BindAction(MainActionMapping.RunAction, ETriggerEvent::Started, this, &APlayerBase::Run);
		EnhancedInputComponent->BindAction(MainActionMapping.RunAction, ETriggerEvent::Completed, this, &APlayerBase::Run);

		// ジャンプ
		EnhancedInputComponent->BindAction(MainActionMapping.JumpAction, ETriggerEvent::Started, this, &APlayerBase::JumpStart);

		// 攻撃アクション
		EnhancedInputComponent->BindAction(MainActionMapping.ComboAction1, ETriggerEvent::Started, this, &APlayerBase::Attack1);
		EnhancedInputComponent->BindAction(MainActionMapping.ComboAction2, ETriggerEvent::Started, this, &APlayerBase::Attack2);

		// 武器を拾う
		EnhancedInputComponent->BindAction(MainActionMapping.PickAction, ETriggerEvent::Started, this, &APlayerBase::PickUpWeapon);	
	}
}


/*
 * 関数名　　　　：SetupPlayerData()
 * 処理内容　　　：プレイヤーのデータ初期化
 * 戻り値　　　　：なし
 */
void APlayerBase::SetupPlayerData()
{
	// アクションをマッピング
	SetupMainActionMapping();

	// ステータス設定
	SetupPlayerStatus();

	// 武器種によってソケットを変更
	SocketName[0] = "hand_rSocket_Sword";
	SocketName[1] = "hand_rSocket_Lance";
	SocketName[2] = "hand_rSocket_Gun";

	// コンボの名前格納
	ComboStartSectionNames = { "First", "Second", "Third","Fourth" };

	// アニメーションアセット設定
	SetupAnimationAssets();
}


/*
 * 関数名　　　　：SetupPlayerStatus()
 * 処理内容　　　：プレイヤーのステータス初期化
 * 引数１　　　　：float _hp・・・・・・・・・・・・・・HPの初期値
 * 引数２		 ：int _remainingLife ・・・・・・・・・復活回数
 * 引数３　　　　：float _damageAmount・・・・・・・・・攻撃力の初期値
 * 引数４		 ：float _invincibilityTime_Short ・・・短い無敵時間
 * 引数５        ：float _invincibilityTime_Long  ・・・長い無敵時間
 * 引数６　　　　：float _jumpHeight・・・・・・・・・・ジャンプ力の初期値
 * 引数７　　　　：float _comboDamageFactor ・・・・・・コンボごとのダメージの倍率
 * 引数８　　　　：float _walkSpeed ・・・・・・・・・・歩く速度
 * 引数９　　　　：float _runSpeed  ・・・・・・・・・・走る速度
 * 戻り値　　　　：なし
 */
void APlayerBase::SetupPlayerStatus(float _hp /*= 100.f*/, int _remainingLife /*= 3.f*/, float _damageAmount /*= 10.f*/,
									float _invincibilityTime_Short /*= 0.3f*/, float _invincibilityTime_Long /*= 0.5f*/,
									float _jumpHeight /*= 150.f*/, float _comboDamageFactor /*= 1.f*/, float _walkSpeed /*= 100.f*/, float _runSpeed /*= 300.f*/)
{
	PlayerStatus.HP = _hp;
	PlayerStatus.MaxHP = _hp;
	PlayerStatus.RemainingLife = _remainingLife;
	PlayerStatus.DamageAmount = _damageAmount;
	PlayerStatus.InvincibilityTime_Short = _invincibilityTime_Short;
	PlayerStatus.InvincibilityTime_Long = _invincibilityTime_Long;
	PlayerStatus.JumpHeight = _jumpHeight;
	PlayerStatus.ComboDamageFactor = _comboDamageFactor;
	PlayerStatus.WalkSpeed = _walkSpeed;
	PlayerStatus.RunSpeed = _runSpeed;
	PlayerStatus.NowMoveSpeed = PlayerStatus.WalkSpeed;
}	

/*
 * 関数名　　　　：SetupMainActionMapping()
 * 処理内容　　　：プレイヤーの入力マッピング
 * 戻り値　　　　：なし
 */
void APlayerBase::SetupMainActionMapping()
{
	// ボタン設定
	TCHAR* defaultMappingContext = TEXT("/Game/0122/Player/Input/IMC_MainAction");

	// それぞれのアクションのパス
	TArray<TCHAR*> inputActionPaths;
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Move"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Dash"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Jump"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Combo1"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Combo2"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_PickUp"));

	// ボタンのマッピング設定
	MainActionMapping.DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, defaultMappingContext);

	// 各アクションのマッピング
	for (int i = 0; i < inputActionPaths.Num(); ++i)
	{
		// 一時保存用
		UInputAction* tempInputAction = LoadObject<UInputAction>(nullptr, inputActionPaths[i]);

		switch (i)
		{
		case 0:
			MainActionMapping.MoveAction = tempInputAction;
			break;
		case 1:
			MainActionMapping.RunAction = tempInputAction;
			break;
		case 2:
			MainActionMapping.JumpAction = tempInputAction;
			break;
		case 3:
			MainActionMapping.ComboAction1 = tempInputAction;
			break;
		case 4:
			MainActionMapping.ComboAction2 = tempInputAction;
			break;
		case 5:
			MainActionMapping.PickAction = tempInputAction;
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("InputAction Array reference error"));
			break;
		}
	}
}


/*
 * 関数名　　　　：SetupAnimationAssets()
 * 処理内容　　　：アニメーションアセットのセットアップ
 * 戻り値　　　　：なし
 */
void APlayerBase::SetupAnimationAssets()
{
	// コンボアニメーションのパス保管用
	{
		TCHAR* ComboAnimationAssetPaths[2];

		// アニメーションアセットのパス
		ComboAnimationAssetPaths[0] = TEXT("/Game/0122/Player/Animation/Montage/Combo/Combo1");
		ComboAnimationAssetPaths[1] = TEXT("/Game/0122/Player/Animation/Montage/Combo/Combo2");

		for (int i = 0; i < 2; ++i)
		{
			PlayerAnimation.ComboAttack[i] = GetAnimationAsset(ComboAnimationAssetPaths[i]);
		}
	}

	{
		// 空中での攻撃アニメーション
		TCHAR* AirAttackAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/Combo/JumpAttack_Montage");

		PlayerAnimation.AirAttack = GetAnimationAsset(AirAttackAnimationAssetPath);
	}

	{
		// 銃所持時のアニメーション
		TCHAR* GunAttackAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/GunAttack_Montage");

		PlayerAnimation.GunAttack = GetAnimationAsset(GunAttackAnimationAssetPath);
	}

	{
		// 銃所持時のアニメーション
		TCHAR* GunAttack2AnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/Kick_Montage");

		PlayerAnimation.GunAttack2 = GetAnimationAsset(GunAttack2AnimationAssetPath);
	}


	{
		// 被ダメージアニメーションのパス保管用
		TCHAR* DamageAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/Damaged_Montage");

		PlayerAnimation.TakeDamage = GetAnimationAsset(DamageAnimationAssetPath);
	}

	{
		// 被ダメージアニメーションのパス保管用
		TCHAR* KnockBackAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/KnockBack_Montage");

		PlayerAnimation.KnockBack = GetAnimationAsset(KnockBackAnimationAssetPath);
	}

	{
		// 死亡時アニメーションのパス保管
		TCHAR* DeathAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/Death_Montage");

		PlayerAnimation.Death = GetAnimationAsset(DeathAnimationAssetPath);
	}
}


/*
 * 関数名　　　　：SetupWeaponMesh()
 * 処理内容　　　：プレイヤーのメッシュをセットアップ(引数がStaticMeshの場合)
 * 引数１　　　　：UStaticMeshComponent*& MeshComp・・・メッシュコンポーネント
 * 引数２　　　　：TCHAR* WeaponAssetPath ・・・・・・・武器のアセットのパス
 * 引数３　　　　：FName PublicName ・・・・・・・・・・エディタでの公開名
 * 戻り値　　　　：なし
 */
void APlayerBase::SetupWeaponMesh(UStaticMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName /*= "WeaponMesh"*/)
{
	// 武器のコンポーネントを作成
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(PublicName);

	if (WeaponAssetPath)
	{
		// 武器のアセット設定
		ConstructorHelpers::FObjectFinder< UStaticMesh > weaponMesh(WeaponAssetPath);

		if (weaponMesh.Succeeded())
		{
			MeshComp->SetStaticMesh(weaponMesh.Object);
		}

		// 体のメッシュに追従
		MeshComp->SetupAttachment(GetMesh(), "hand_rSocket");
	}
}


/*
 * 関数名　　　　：SetupWeaponMesh()
 * 処理内容　　　：プレイヤーのメッシュをセットアップ(引数がSkeletalMeshの場合)
 * 引数１　　　　：USkeletalMeshComponent*& MeshComp・・・メッシュコンポーネント
 * 引数２　　　　：TCHAR* WeaponAssetPath ・・・・・・・・武器のアセットのパス
 * 引数３　　　　：FName PublicName ・・・・・・・・・・・エディタでの公開名
 * 戻り値　　　　：なし
 */
void APlayerBase::SetupWeaponMesh(USkeletalMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName /*= "WeaponMesh"*/)
{
	// 武器のコンポーネントを作成
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(PublicName);

	if (WeaponAssetPath)
	{
		// 武器のアセット設定
		ConstructorHelpers::FObjectFinder< USkeletalMesh > weaponMesh(WeaponAssetPath);

		if (weaponMesh.Succeeded())
		{
			MeshComp->SetSkeletalMeshAsset(weaponMesh.Object);
		}

		// 体のメッシュに追従
		MeshComp->SetupAttachment(GetMesh(), "hand_rSocket");
	}
}


/*
 * 関数名　　　　：GetAnimationAsset()
 * 処理内容　　　：アニメーションアセットを返す
 * 引数１　　　　：TCHAR* _animAssetPath ・・・アニメーションアセットのパス
 * 戻り値　　　　：見つかったアニメーション
 */
UAnimMontage* APlayerBase::GetAnimationAsset(TCHAR* _animAssetPath)
{
	// アセットを探してセット
	ConstructorHelpers::FObjectFinder<UAnimMontage> SearchAnimMontage(_animAssetPath);

	// そのアセットが存在したら格納
	if (SearchAnimMontage.Succeeded())
	{
		UAnimMontage* FoundAnimMontage = SearchAnimMontage.Object;

		return FoundAnimMontage;
	}

	return nullptr;
}


/*
 * 関数名　　　　：Move()
 * 処理内容　　　：プレイヤーの入力受付(移動処理)
 * 引数１　　　　：FInputActionValue& Value・・・入力量
 * 戻り値　　　　：なし
 */
void APlayerBase::Move(const FInputActionValue& _value)
{
	// コントロール可能か
	if (!IsControl) { return; }

	// 2軸で入力取得
	FVector2D MovementVector = _value.Get<FVector2D>();

	if (PlayerController)
	{
		// カメラの角度取得
		const FRotator Rotation = PlayerController->GetNowCameraRotation();

		// 回転設定
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// スプラインに沿った移動方向取得(X,Y)
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 移動
		AddMovementInput(RightDirection, MovementVector.X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		// 移動方向に回転
		RotateCharacter(MovementVector.Y);
	}
}


/*
 * 関数名　　　　：AIMove()
 * 処理内容　　　：プレイヤーの移動処理(デモ画面)
 * 戻り値　　　　：なし
 */
void APlayerBase::AIMove()
{

}

/*
 * 関数名　　　　：AIAttack()
 * 処理内容　　　：プレイヤーの攻撃処理(デモ画面)
 * 戻り値　　　　：なし
 */
void APlayerBase::AIAttack()
{

}


/*
 * 関数名　　　　：AIPickUpWeapon()
 * 処理内容　　　：プレイヤーの武器を拾う処理(デモ画面)
 * 戻り値　　　　：なし
 */
void APlayerBase::AIPickUpWeapon()
{

}


/*
 * 関数名　　　　：Run()
 * 処理内容　　　：プレイヤーの入力受付(ダッシュ切り替え)
 * 戻り値　　　　：なし
 */
void APlayerBase::Run()
{
	// コントロール可能か
	if (!IsControl) { return; }

	if (!IsRunning)
	{
		// ダッシュオン
		IsRunning = true;
		PlayerStatus.NowMoveSpeed = PlayerStatus.RunSpeed;
	}
	else
	{
		// ダッシュオン
		IsRunning = false;
		PlayerStatus.NowMoveSpeed = PlayerStatus.WalkSpeed;
	}
}


/*
 * 関数名　　　　：JumpStart()
 * 処理内容　　　：プレイヤーの入力受付(ジャンプ開始)
 * 戻り値　　　　：なし
 */
void APlayerBase::JumpStart()
{
	// コントロール可能か
	if (!IsControl) { return; }

	// 地上にいたらジャンプ開始
	if (!IsJumping)
	{
		// ジャンプ時間
		frames = 0.f;

		JumpBeforePos_Z = GetActorLocation().Z;

		IsJumping = true;
	}
}


/*
 * 関数名　　　　：Jump()
 * 処理内容　　　：プレイヤーのジャンプ中処理
 * 戻り値　　　　：なし
 */
void APlayerBase::Jump()
{
	// 現在位置
	FVector NowPos = GetActorLocation();

	// Sinで高さ更新
	float SinValue = PlayerStatus.JumpHeight * FMath::Sin(radPerFrame * frames);

	// ジャンプ前の高さから位置更新
	const FVector nextPos(FVector(NowPos.X, NowPos.Y, SinValue + JumpBeforePos_Z));

	SetActorLocation(nextPos);

	// 着地処理 下降開始から判定開始
	if (frames >= 10.f && (IsPlayerGrounded() || nextPos.Z <= JumpBeforePos_Z))
	{
		IsJumping = false;
	}

	// フレーム+1
	frames += 1.f;
}


/*
 * 関数名　　　　：IsPlayerGrounded()
 * 処理内容　　　：プレイヤーが地面についているか判定
 * 戻り値　　　　：地面についていたらtrue
 */
bool APlayerBase::IsPlayerGrounded() const
{
	bool IsGrounded = false;

	// 地面にいるかの確認
	if (CharacterMovementComp)
	{
		IsGrounded = CharacterMovementComp->IsFalling() == false;
	}

	return IsGrounded;
}


// 接触開始時に行う処理
void APlayerBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// オーバーラップした際に実行したいイベント
	if (OtherActor && (OtherActor != this))
	{
		// 当たったのがプレイヤーの時装備させる
		if (OtherActor->ActorHasTag("Weapon") && !IsHoldWeapon)
		{
				//// 新しい武器を作成
				//AWeaponBase* NewWeapon = Cast<AWeaponBase>(OtherActor);

				//if (!NewWeapon->GetIsHeld() && !NewWeapon->GetIsFalling())
				//{
				//	// 持っている武器を更新
				//	Weapon = NewWeapon;
				//	
				//	// 武器の種類判別
				//	WeaponType = Weapon->GetWeaponType();
				//	
				//	// プレイヤーに装備させる
				//	Weapon->AttachToHand(this, SocketName[int32(WeaponType)], EOwnerType::OwnerType_Player);
				//	
				//	// 武器を落とすまでの回数をリセット
				//	PlayerStatus.WeaponDropLimit = PlayerStatus.DefaultWeaponDropLimit;
				//	
				//	// 武器を持っている状態に
				//	IsHoldWeapon = true;
				//}
		}
	}
}


/*
 * 関数名　　　　：ComboAttack()
 * 処理内容　　　：プレイヤーの攻撃処理
 * 引数１　　　　：int _attackNum・・・攻撃アニメーションの種類判別用
 * 戻り値　　　　：なし
 */
void APlayerBase::ComboAttack(int _attackNum /*= 0*/)
{
	switch (WeaponType)
	{
	case EWeaponType::WeaponType_Sword:
		SwordAttack(_attackNum);
		break;
	case EWeaponType::WeaponType_Lance:
		LanceAttack(_attackNum);
		break;
	case EWeaponType::WeaponType_Gun:
		GunAttack(_attackNum);
		break;
	case EWeaponType::WeaponType_None:

		break;
	}
}


/*
 * 関数名　　　　：Attack1()
 * 処理内容　　　：プレイヤーの入力受付(攻撃１つ目)
 * 戻り値　　　　：なし
 */
void APlayerBase::Attack1()
{
	if (!IsControl) { return; }

	// コンボ攻撃
	ComboAttack(0);
}

/*
 * 関数名　　　　：Attack2()
 * 処理内容　　　：プレイヤーの入力受付(攻撃２つ目)
 * 戻り値　　　　：なし
 */
void APlayerBase::Attack2()
{
	if (!IsControl) { return; }

	// コンボ攻撃
	ComboAttack(1);
}


/*
 * 関数名　　　　：SwordAttack()
 * 処理内容　　　：溜め攻撃
 * 戻り値　　　　：なし
 */
void APlayerBase::SwordAttack(int _attackNum)
{
	// コントロール不能へ
	IsControl = false;

	if (!IsJumping)
	{
		if (!IsAttacking)
		{
			// 攻撃中フラグオン
			IsAttacking = true;
		}
		else
		{
			// コンボ可能な時,継続
			if (CanCombo)
			{
				// ラストアタックまでコンボ継続
				if (ComboStartSectionNames[ComboIndex] != ComboStartSectionNames.Last())
				{
					++ComboIndex;
				}
			}
		}

		// 攻撃のアニメーション再生
		PlayAnimation(PlayerAnimation.ComboAttack[_attackNum], ComboStartSectionNames[ComboIndex]);
	}
	else
	{
		// 攻撃のアニメーション再生
		PlayAnimation(PlayerAnimation.AirAttack);
		ComboIndex = 2;
	}
}


/*
 * 関数名　　　　：LanceAttack()
 * 処理内容　　　：溜め攻撃
 * 戻り値　　　　：なし
 */
void APlayerBase::LanceAttack(int _attackNum)
{
	// コントロール不能へ
	IsControl = false;

	// プレイヤーの角度修正
	ActionAssistComp->CorrectAttackAngle();

	if (!IsJumping)
	{
		if (!IsAttacking)
		{
			// 攻撃中フラグオン
			IsAttacking = true;
		}
		else
		{
			// コンボ可能な時,継続
			if (CanCombo)
			{
				// ラストアタックまでコンボ継続
				if (ComboStartSectionNames[ComboIndex] != ComboStartSectionNames.Last())
				{
					++ComboIndex;
				}
			}
		}

		// 攻撃のアニメーション再生
		PlayAnimation(PlayerAnimation.ComboAttack[_attackNum], ComboStartSectionNames[ComboIndex]);
	}
	else
	{
		// 攻撃のアニメーション再生
		PlayAnimation(PlayerAnimation.AirAttack);
		ComboIndex = 2;
	}
}


/*
 * 関数名　　　　：GunAttack()
 * 処理内容　　　：銃の攻撃
 * 戻り値　　　　：なし
 */
void APlayerBase::GunAttack(int _attackNum)
{
	if (_attackNum == 0)
	{
		IsKicking = true;
		PlayAnimation(PlayerAnimation.GunAttack2);
	}
	else
	{
		PlayAnimation(PlayerAnimation.GunAttack);
	}
}


/*
 * 関数名　　　　：PickUpWeapon()
 * 処理内容　　　：武器を拾う処理
 * 戻り値　　　　：なし
 */
void APlayerBase::PickUpWeapon()
{
	if (!IsControl) { return; }

	if (PlayerController->GetIsPickUpWeapon())
	{
		// 別の武器を持っているとき
		if (Weapon != nullptr)
		{
			// 今持っている武器を手放す
			Weapon->DetachToHand();
			Weapon = nullptr;
		}
		// それ以外
		else
		{
			IsHoldWeapon = true;
		}

		// 武器を更新
		Weapon = PlayerController->GetClosestDistanceWeapons();
		WeaponType = Weapon->GetWeaponType();
		Weapon->AttachToHand(this, SocketName[int32(WeaponType)], EOwnerType::OwnerType_Player);

		// 武器を落とすまでの回数をリセット
		PlayerStatus.WeaponDropLimit = PlayerStatus.DefaultWeaponDropLimit;
	}
}



/*
 * 関数名　　　　：RotateCharacter()
 * 処理内容　　　：プレイヤーのステータス初期化
 * 引数１　　　　：float _nowInput_X・・・現在の移動入力値
 * 戻り値　　　　：なし
 */
void APlayerBase::RotateCharacter(float _nowInput_X)
{
	// 入力がない場合は何もしない
	if (_nowInput_X == 0) { return; }

	// 右を向いているか確認
	IsLookRight = (_nowInput_X != 1.f) ? (true) : (false);

	// 回転
	ActionAssistComp->OwnerParallelToCamera(IsLookRight);
}


/*
 * 関数名　　　　：RotateCharacter()
 * 処理内容　　　：キャラクターの移動量取得
 * 引数１　　　　：FVector _nowPos・・・現在位置
 * 戻り値　　　　：なし
 */
void APlayerBase::AmountOfMovement(FVector _nowPos)
{
	// 同じ位置にいたらスキップ
	if (BeforePos == _nowPos) { return; }

	// 距離計算
	const float AmountOfMovement = FVector::Dist(_nowPos, BeforePos);

	// 移動量保存
	DistanceAdvanced += AmountOfMovement;

	// 現在の位置を保存
	BeforePos = _nowPos;
}


/*
 * 関数名　　　　：SlowDownDeathAnimationRate()
 * 処理内容　　　：死亡時アニメーション引き伸ばし
 * 戻り値　　　　：なし
 */
void APlayerBase::SlowDownDeathAnimationRate()
{
	// 遅くする
	GetMesh()->GlobalAnimRateScale = DeadAnimRate;


	// プレイヤーを削除
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(TimerHandle, this, &APlayerBase::CallGameModeFunc_DestroyPlayer, DeadToGameOverTime, false);
}



/*
 * 関数名　　　　：SetEnableRootMotion()
 * 処理内容　　　：RootMotionの開始と移動距離
 * 引数１　　　　：bool _enableRootMotion・・・ルートモーションのオンオフ
 * 引数２　　　　：float _distance・・・・・・・アニメーションで移動した距離
 * 戻り値　　　　：なし
 */
void APlayerBase::SetEnableRootMotion(bool _enableRootMotion, float _distance /*= 0*/)
{
	// アニメーション移動開始
	EnableRootMotion = _enableRootMotion; 

	// アニメーションでの移動値
	AnimationMoveValue = _distance;
}


/*
 * 関数名　　　　：RootMotion()
 * 処理内容　　　：RootMotionの実装
 * 引数１　　　　：float _distance・・・アニメーションで移動した距離
 * 戻り値　　　　：なし
 */
void APlayerBase::RootMotion(float _distance)
{
	// 向いている方向によって値を変換
	const float Distance = (IsLookRight) ? (-_distance) : (_distance);

	// 前方ベクトル取得
	FVector ForwardVector = GetActorForwardVector();

	// 現在向いている方向に移動する
	FVector newVector = ForwardVector * _distance;

	// 加算
	AddActorWorldOffset(newVector,false);
}


/*
 * 関数名　　　　：CallGameModeFunc_DestroyPlayer()
 * 処理内容　　　：死亡時のゲームモード内の関数呼び出し
 * 　　　　　　　　残機が残ってたらリスポーン,なくなったらプレイヤー削除
 * 戻り値　　　　：なし
 */
void APlayerBase::CallGameModeFunc_DestroyPlayer()
{
	// コントローラーからプレイヤーを削除
	if (PlayerController)
	{
		// 残機があるうちはリスポーン
		if (PlayerController->GetRemainingLives() > 0)
		{
			// プレイヤーリスポーン
			PlayerController->RespawnPlayer();
		}
		// それ以外はゲームオーバー
		else
		{
			// プレイヤー削除
			PlayerController->DestroyPlayer();
		}
	}
}


/*
 * 関数名　　　　：ResetAllAttackFlags()
 * 処理内容　　　：攻撃に関するフラグをすべてリセット
 * 戻り値　　　　：なし
 */void APlayerBase::ResetAllAttackFlags()
{
	// 攻撃中のフラグリセット
	IsAttacking = false;
	CanCombo = false;
	IsControl = false;
	IsKicking = false;
	ComboIndex = 0;
}


/*
 * 関数名　　　　：ContinuationCombo()
 * 処理内容　　　：コンボの継続
 * 戻り値　　　　：なし
 */
void APlayerBase::ContinuationCombo()
{
	IsControl = true;
	CanCombo = true;
}


/*
 * 関数名　　　　：ResetCombo()
 * 処理内容　　　：コンボリセット
 * 戻り値　　　　：なし
 */
void APlayerBase::ResetCombo()
{
	// 攻撃中のフラグをすべてリセット
	ResetAllAttackFlags();

	// コントロール可能状態へ
	IsControl = true;
}


void APlayerBase::StopMontage()
{
	//アニメーションを止める
	GetMesh()->GetAnimInstance()->StopSlotAnimation();
}


/*
 * 関数名　　　　：SetCollision()
 * 処理内容　　　：攻撃判定
 * 戻り値　　　　：なし
 */
void APlayerBase::SetCollision()
{
	// 武器を持っているとき当たり判定をつける
	if (Weapon)
	{
		Weapon->SetCollision();
	}
}

/*
 * 関数名　　　　：TakedDamage()
 * 処理内容　　　：プレイヤーの被ダメージ処理
 * 引数１　　　　：float _damage・・・被ダメージ量
 * 引数２　　　　：bool _isLastAttack・・・コンボの最終かどうか
 * 戻り値　　　　：なし
 */
void APlayerBase::TakedDamage(float _damage, bool _isLastAttack /*= false*/)
{
	if (IsDeath || IsInvincibility || AbsolutelyInvincible) { return; }

	// 武器を持っていないときに攻撃を受けたら死亡
	if (!IsHoldWeapon && !IsDeath)
	{
		// コントロール不能へ
		IsControl = false;

		// 死亡状態へ
		IsDeath = true;

		// コリジョンをオフに
		SetActorEnableCollision(true);

		// ヒットエフェクト発生
		ActionAssistComp->SpawnEffect(HitEffect, GetActorLocation());

		// 死亡アニメーション再生
		PlayAnimation(PlayerAnimation.Death);
	}
	// 武器を持っているとき
	else
	{
		// 攻撃中のフラグをすべてリセット
		ResetAllAttackFlags();

		// 敵のコンボが最終段だった時必ず武器を落とす
		if (_isLastAttack)
		{
			PlayerStatus.WeaponDropLimit = 0;
		}

		// 無敵解除時間格納用
		float InvincibilityReleaseTime;

		// 被ダメージアニメーション
		if (PlayerStatus.WeaponDropLimit <= 0)
		{
			// 長い無敵時間を適用
			InvincibilityReleaseTime = PlayerStatus.InvincibilityTime_Long;

			// 武器を落とす
			if (Weapon && IsHoldWeapon)
			{
				Weapon->DetachToHand();
				Weapon = nullptr;
				WeaponType = EWeaponType::WeaponType_None;
				IsHoldWeapon = false;
			}

			// ノックバックアニメーション再生
			PlayAnimation(PlayerAnimation.KnockBack);
		}
		else
		{
			// 短い無敵時間を適用
			InvincibilityReleaseTime = PlayerStatus.InvincibilityTime_Long;

			// 攻撃を受ける
			--PlayerStatus.WeaponDropLimit;

			// のけぞりアニメーション再生
			PlayAnimation(PlayerAnimation.TakeDamage);			
		}

		// ヒットエフェクト発生
		ActionAssistComp->SpawnEffect(HitEffect, GetActorLocation());

	}
}

/*
 * 関数名　　　　：SetDeath()
 * 処理内容　　　：死ぬ
 * 戻り値　　　　：なし
 */
void APlayerBase::SetDeath()
{
	// 死亡アニメーション再生
	PlayAnimation(PlayerAnimation.Death);
}

/*
 * 関数名　　　　：PlayAnimation()
 * 処理内容　　　：プレイヤーのアニメーション再生(再生中は操作不可)
 * 戻り値　　　　：なし
 */
void APlayerBase::PlayAnimation(UAnimMontage* _toPlayAnimMontage, FName _startSectionName /*= "None"*/, float _playRate /*= 1.f*/)
{
	// コントロール不能へ
	IsControl = false;

	// 再生するアニメーションを格納
	UAnimMontage* toPlayAnimMontage = _toPlayAnimMontage;

	// アニメーション再生
	if (toPlayAnimMontage != nullptr)
	{
		ActionAssistComp->PlayAnimation(_toPlayAnimMontage, _startSectionName, _playRate);
	}
}


