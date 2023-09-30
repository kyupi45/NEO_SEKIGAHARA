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
	// Tick()���}�C�t���[���ĂԂ��ǂ����̔���
	PrimaryActorTick.bCanEverTick = true;

	// �v���C���[�̐ݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// �v���C���[�ɉ�]�̐���
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// �^�O�ݒ�
	Tags.Add("Player");

	// �L�����N�^�[�R���|�[�l���g�擾
	CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->MaxWalkSpeed = 500.f;

	// �A�^�b�N�A�V�X�g�R���|�[�l���g�쐬
	ActionAssistComp = CreateDefaultSubobject<UActionAssistComponent>(TEXT("AttackAssist"));

	// �R���W�����C�x���g��ݒ�
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnOverlap);

	// Player�̃Z�b�g�A�b�v
	SetupPlayerData();
}


// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	// �Q�[�����[�h�擾
	pGameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// �R���g���[���[�擾
	PlayerController = Cast<ANEOPlayerController>(Controller);

	// ���͂̐ݒ�
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainActionMapping.DefaultMappingContext, 0);
		}
	}

	// �����Spawn
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

	// �W�����v���̂ݏ���
	if (IsJumping)
	{
		Jump();
	}

	// �A�j���[�V�����ɍ��킹�Ĉړ�
	if (EnableRootMotion && !ActionAssistComp->WallCheck(50.f) && !ActionAssistComp->WallCheck(-50.f))
	{
		RootMotion(AnimationMoveValue);
	}
}


// ���͂̃o�C���h
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// �ړ�
		EnhancedInputComponent->BindAction(MainActionMapping.MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);

		// ����							
		EnhancedInputComponent->BindAction(MainActionMapping.RunAction, ETriggerEvent::Started, this, &APlayerBase::Run);
		EnhancedInputComponent->BindAction(MainActionMapping.RunAction, ETriggerEvent::Completed, this, &APlayerBase::Run);

		// �W�����v
		EnhancedInputComponent->BindAction(MainActionMapping.JumpAction, ETriggerEvent::Started, this, &APlayerBase::JumpStart);

		// �U���A�N�V����
		EnhancedInputComponent->BindAction(MainActionMapping.ComboAction1, ETriggerEvent::Started, this, &APlayerBase::Attack1);
		EnhancedInputComponent->BindAction(MainActionMapping.ComboAction2, ETriggerEvent::Started, this, &APlayerBase::Attack2);

		// ������E��
		EnhancedInputComponent->BindAction(MainActionMapping.PickAction, ETriggerEvent::Started, this, &APlayerBase::PickUpWeapon);	
	}
}


/*
 * �֐����@�@�@�@�FSetupPlayerData()
 * �������e�@�@�@�F�v���C���[�̃f�[�^������
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SetupPlayerData()
{
	// �A�N�V�������}�b�s���O
	SetupMainActionMapping();

	// �X�e�[�^�X�ݒ�
	SetupPlayerStatus();

	// �����ɂ���ă\�P�b�g��ύX
	SocketName[0] = "hand_rSocket_Sword";
	SocketName[1] = "hand_rSocket_Lance";
	SocketName[2] = "hand_rSocket_Gun";

	// �R���{�̖��O�i�[
	ComboStartSectionNames = { "First", "Second", "Third","Fourth" };

	// �A�j���[�V�����A�Z�b�g�ݒ�
	SetupAnimationAssets();
}


/*
 * �֐����@�@�@�@�FSetupPlayerStatus()
 * �������e�@�@�@�F�v���C���[�̃X�e�[�^�X������
 * �����P�@�@�@�@�Ffloat _hp�E�E�E�E�E�E�E�E�E�E�E�E�E�EHP�̏����l
 * �����Q		 �Fint _remainingLife �E�E�E�E�E�E�E�E�E������
 * �����R�@�@�@�@�Ffloat _damageAmount�E�E�E�E�E�E�E�E�E�U���͂̏����l
 * �����S		 �Ffloat _invincibilityTime_Short �E�E�E�Z�����G����
 * �����T        �Ffloat _invincibilityTime_Long  �E�E�E�������G����
 * �����U�@�@�@�@�Ffloat _jumpHeight�E�E�E�E�E�E�E�E�E�E�W�����v�͂̏����l
 * �����V�@�@�@�@�Ffloat _comboDamageFactor �E�E�E�E�E�E�R���{���Ƃ̃_���[�W�̔{��
 * �����W�@�@�@�@�Ffloat _walkSpeed �E�E�E�E�E�E�E�E�E�E�������x
 * �����X�@�@�@�@�Ffloat _runSpeed  �E�E�E�E�E�E�E�E�E�E���鑬�x
 * �߂�l�@�@�@�@�F�Ȃ�
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
 * �֐����@�@�@�@�FSetupMainActionMapping()
 * �������e�@�@�@�F�v���C���[�̓��̓}�b�s���O
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SetupMainActionMapping()
{
	// �{�^���ݒ�
	TCHAR* defaultMappingContext = TEXT("/Game/0122/Player/Input/IMC_MainAction");

	// ���ꂼ��̃A�N�V�����̃p�X
	TArray<TCHAR*> inputActionPaths;
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Move"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Dash"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Jump"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Combo1"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_Combo2"));
	inputActionPaths.Add(TEXT("/Game/0122/Player/Input/Actions/IA_PickUp"));

	// �{�^���̃}�b�s���O�ݒ�
	MainActionMapping.DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, defaultMappingContext);

	// �e�A�N�V�����̃}�b�s���O
	for (int i = 0; i < inputActionPaths.Num(); ++i)
	{
		// �ꎞ�ۑ��p
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
 * �֐����@�@�@�@�FSetupAnimationAssets()
 * �������e�@�@�@�F�A�j���[�V�����A�Z�b�g�̃Z�b�g�A�b�v
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SetupAnimationAssets()
{
	// �R���{�A�j���[�V�����̃p�X�ۊǗp
	{
		TCHAR* ComboAnimationAssetPaths[2];

		// �A�j���[�V�����A�Z�b�g�̃p�X
		ComboAnimationAssetPaths[0] = TEXT("/Game/0122/Player/Animation/Montage/Combo/Combo1");
		ComboAnimationAssetPaths[1] = TEXT("/Game/0122/Player/Animation/Montage/Combo/Combo2");

		for (int i = 0; i < 2; ++i)
		{
			PlayerAnimation.ComboAttack[i] = GetAnimationAsset(ComboAnimationAssetPaths[i]);
		}
	}

	{
		// �󒆂ł̍U���A�j���[�V����
		TCHAR* AirAttackAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/Combo/JumpAttack_Montage");

		PlayerAnimation.AirAttack = GetAnimationAsset(AirAttackAnimationAssetPath);
	}

	{
		// �e�������̃A�j���[�V����
		TCHAR* GunAttackAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/GunAttack_Montage");

		PlayerAnimation.GunAttack = GetAnimationAsset(GunAttackAnimationAssetPath);
	}

	{
		// �e�������̃A�j���[�V����
		TCHAR* GunAttack2AnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/Kick_Montage");

		PlayerAnimation.GunAttack2 = GetAnimationAsset(GunAttack2AnimationAssetPath);
	}


	{
		// ��_���[�W�A�j���[�V�����̃p�X�ۊǗp
		TCHAR* DamageAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/Damaged_Montage");

		PlayerAnimation.TakeDamage = GetAnimationAsset(DamageAnimationAssetPath);
	}

	{
		// ��_���[�W�A�j���[�V�����̃p�X�ۊǗp
		TCHAR* KnockBackAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/KnockBack_Montage");

		PlayerAnimation.KnockBack = GetAnimationAsset(KnockBackAnimationAssetPath);
	}

	{
		// ���S���A�j���[�V�����̃p�X�ۊ�
		TCHAR* DeathAnimationAssetPath = TEXT("/Game/0122/Player/Animation/Montage/Death_Montage");

		PlayerAnimation.Death = GetAnimationAsset(DeathAnimationAssetPath);
	}
}


/*
 * �֐����@�@�@�@�FSetupWeaponMesh()
 * �������e�@�@�@�F�v���C���[�̃��b�V�����Z�b�g�A�b�v(������StaticMesh�̏ꍇ)
 * �����P�@�@�@�@�FUStaticMeshComponent*& MeshComp�E�E�E���b�V���R���|�[�l���g
 * �����Q�@�@�@�@�FTCHAR* WeaponAssetPath �E�E�E�E�E�E�E����̃A�Z�b�g�̃p�X
 * �����R�@�@�@�@�FFName PublicName �E�E�E�E�E�E�E�E�E�E�G�f�B�^�ł̌��J��
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SetupWeaponMesh(UStaticMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName /*= "WeaponMesh"*/)
{
	// ����̃R���|�[�l���g���쐬
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(PublicName);

	if (WeaponAssetPath)
	{
		// ����̃A�Z�b�g�ݒ�
		ConstructorHelpers::FObjectFinder< UStaticMesh > weaponMesh(WeaponAssetPath);

		if (weaponMesh.Succeeded())
		{
			MeshComp->SetStaticMesh(weaponMesh.Object);
		}

		// �̂̃��b�V���ɒǏ]
		MeshComp->SetupAttachment(GetMesh(), "hand_rSocket");
	}
}


/*
 * �֐����@�@�@�@�FSetupWeaponMesh()
 * �������e�@�@�@�F�v���C���[�̃��b�V�����Z�b�g�A�b�v(������SkeletalMesh�̏ꍇ)
 * �����P�@�@�@�@�FUSkeletalMeshComponent*& MeshComp�E�E�E���b�V���R���|�[�l���g
 * �����Q�@�@�@�@�FTCHAR* WeaponAssetPath �E�E�E�E�E�E�E�E����̃A�Z�b�g�̃p�X
 * �����R�@�@�@�@�FFName PublicName �E�E�E�E�E�E�E�E�E�E�E�G�f�B�^�ł̌��J��
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SetupWeaponMesh(USkeletalMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName /*= "WeaponMesh"*/)
{
	// ����̃R���|�[�l���g���쐬
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(PublicName);

	if (WeaponAssetPath)
	{
		// ����̃A�Z�b�g�ݒ�
		ConstructorHelpers::FObjectFinder< USkeletalMesh > weaponMesh(WeaponAssetPath);

		if (weaponMesh.Succeeded())
		{
			MeshComp->SetSkeletalMeshAsset(weaponMesh.Object);
		}

		// �̂̃��b�V���ɒǏ]
		MeshComp->SetupAttachment(GetMesh(), "hand_rSocket");
	}
}


/*
 * �֐����@�@�@�@�FGetAnimationAsset()
 * �������e�@�@�@�F�A�j���[�V�����A�Z�b�g��Ԃ�
 * �����P�@�@�@�@�FTCHAR* _animAssetPath �E�E�E�A�j���[�V�����A�Z�b�g�̃p�X
 * �߂�l�@�@�@�@�F���������A�j���[�V����
 */
UAnimMontage* APlayerBase::GetAnimationAsset(TCHAR* _animAssetPath)
{
	// �A�Z�b�g��T���ăZ�b�g
	ConstructorHelpers::FObjectFinder<UAnimMontage> SearchAnimMontage(_animAssetPath);

	// ���̃A�Z�b�g�����݂�����i�[
	if (SearchAnimMontage.Succeeded())
	{
		UAnimMontage* FoundAnimMontage = SearchAnimMontage.Object;

		return FoundAnimMontage;
	}

	return nullptr;
}


/*
 * �֐����@�@�@�@�FMove()
 * �������e�@�@�@�F�v���C���[�̓��͎�t(�ړ�����)
 * �����P�@�@�@�@�FFInputActionValue& Value�E�E�E���͗�
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::Move(const FInputActionValue& _value)
{
	// �R���g���[���\��
	if (!IsControl) { return; }

	// 2���œ��͎擾
	FVector2D MovementVector = _value.Get<FVector2D>();

	if (PlayerController)
	{
		// �J�����̊p�x�擾
		const FRotator Rotation = PlayerController->GetNowCameraRotation();

		// ��]�ݒ�
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// �X�v���C���ɉ������ړ������擾(X,Y)
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// �ړ�
		AddMovementInput(RightDirection, MovementVector.X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		// �ړ������ɉ�]
		RotateCharacter(MovementVector.Y);
	}
}


/*
 * �֐����@�@�@�@�FAIMove()
 * �������e�@�@�@�F�v���C���[�̈ړ�����(�f�����)
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::AIMove()
{

}

/*
 * �֐����@�@�@�@�FAIAttack()
 * �������e�@�@�@�F�v���C���[�̍U������(�f�����)
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::AIAttack()
{

}


/*
 * �֐����@�@�@�@�FAIPickUpWeapon()
 * �������e�@�@�@�F�v���C���[�̕�����E������(�f�����)
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::AIPickUpWeapon()
{

}


/*
 * �֐����@�@�@�@�FRun()
 * �������e�@�@�@�F�v���C���[�̓��͎�t(�_�b�V���؂�ւ�)
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::Run()
{
	// �R���g���[���\��
	if (!IsControl) { return; }

	if (!IsRunning)
	{
		// �_�b�V���I��
		IsRunning = true;
		PlayerStatus.NowMoveSpeed = PlayerStatus.RunSpeed;
	}
	else
	{
		// �_�b�V���I��
		IsRunning = false;
		PlayerStatus.NowMoveSpeed = PlayerStatus.WalkSpeed;
	}
}


/*
 * �֐����@�@�@�@�FJumpStart()
 * �������e�@�@�@�F�v���C���[�̓��͎�t(�W�����v�J�n)
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::JumpStart()
{
	// �R���g���[���\��
	if (!IsControl) { return; }

	// �n��ɂ�����W�����v�J�n
	if (!IsJumping)
	{
		// �W�����v����
		frames = 0.f;

		JumpBeforePos_Z = GetActorLocation().Z;

		IsJumping = true;
	}
}


/*
 * �֐����@�@�@�@�FJump()
 * �������e�@�@�@�F�v���C���[�̃W�����v������
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::Jump()
{
	// ���݈ʒu
	FVector NowPos = GetActorLocation();

	// Sin�ō����X�V
	float SinValue = PlayerStatus.JumpHeight * FMath::Sin(radPerFrame * frames);

	// �W�����v�O�̍�������ʒu�X�V
	const FVector nextPos(FVector(NowPos.X, NowPos.Y, SinValue + JumpBeforePos_Z));

	SetActorLocation(nextPos);

	// ���n���� ���~�J�n���画��J�n
	if (frames >= 10.f && (IsPlayerGrounded() || nextPos.Z <= JumpBeforePos_Z))
	{
		IsJumping = false;
	}

	// �t���[��+1
	frames += 1.f;
}


/*
 * �֐����@�@�@�@�FIsPlayerGrounded()
 * �������e�@�@�@�F�v���C���[���n�ʂɂ��Ă��邩����
 * �߂�l�@�@�@�@�F�n�ʂɂ��Ă�����true
 */
bool APlayerBase::IsPlayerGrounded() const
{
	bool IsGrounded = false;

	// �n�ʂɂ��邩�̊m�F
	if (CharacterMovementComp)
	{
		IsGrounded = CharacterMovementComp->IsFalling() == false;
	}

	return IsGrounded;
}


// �ڐG�J�n���ɍs������
void APlayerBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �I�[�o�[���b�v�����ۂɎ��s�������C�x���g
	if (OtherActor && (OtherActor != this))
	{
		// ���������̂��v���C���[�̎�����������
		if (OtherActor->ActorHasTag("Weapon") && !IsHoldWeapon)
		{
				//// �V����������쐬
				//AWeaponBase* NewWeapon = Cast<AWeaponBase>(OtherActor);

				//if (!NewWeapon->GetIsHeld() && !NewWeapon->GetIsFalling())
				//{
				//	// �����Ă��镐����X�V
				//	Weapon = NewWeapon;
				//	
				//	// ����̎�ޔ���
				//	WeaponType = Weapon->GetWeaponType();
				//	
				//	// �v���C���[�ɑ���������
				//	Weapon->AttachToHand(this, SocketName[int32(WeaponType)], EOwnerType::OwnerType_Player);
				//	
				//	// ����𗎂Ƃ��܂ł̉񐔂����Z�b�g
				//	PlayerStatus.WeaponDropLimit = PlayerStatus.DefaultWeaponDropLimit;
				//	
				//	// ����������Ă����Ԃ�
				//	IsHoldWeapon = true;
				//}
		}
	}
}


/*
 * �֐����@�@�@�@�FComboAttack()
 * �������e�@�@�@�F�v���C���[�̍U������
 * �����P�@�@�@�@�Fint _attackNum�E�E�E�U���A�j���[�V�����̎�ޔ��ʗp
 * �߂�l�@�@�@�@�F�Ȃ�
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
 * �֐����@�@�@�@�FAttack1()
 * �������e�@�@�@�F�v���C���[�̓��͎�t(�U���P��)
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::Attack1()
{
	if (!IsControl) { return; }

	// �R���{�U��
	ComboAttack(0);
}

/*
 * �֐����@�@�@�@�FAttack2()
 * �������e�@�@�@�F�v���C���[�̓��͎�t(�U���Q��)
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::Attack2()
{
	if (!IsControl) { return; }

	// �R���{�U��
	ComboAttack(1);
}


/*
 * �֐����@�@�@�@�FSwordAttack()
 * �������e�@�@�@�F���ߍU��
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SwordAttack(int _attackNum)
{
	// �R���g���[���s�\��
	IsControl = false;

	if (!IsJumping)
	{
		if (!IsAttacking)
		{
			// �U�����t���O�I��
			IsAttacking = true;
		}
		else
		{
			// �R���{�\�Ȏ�,�p��
			if (CanCombo)
			{
				// ���X�g�A�^�b�N�܂ŃR���{�p��
				if (ComboStartSectionNames[ComboIndex] != ComboStartSectionNames.Last())
				{
					++ComboIndex;
				}
			}
		}

		// �U���̃A�j���[�V�����Đ�
		PlayAnimation(PlayerAnimation.ComboAttack[_attackNum], ComboStartSectionNames[ComboIndex]);
	}
	else
	{
		// �U���̃A�j���[�V�����Đ�
		PlayAnimation(PlayerAnimation.AirAttack);
		ComboIndex = 2;
	}
}


/*
 * �֐����@�@�@�@�FLanceAttack()
 * �������e�@�@�@�F���ߍU��
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::LanceAttack(int _attackNum)
{
	// �R���g���[���s�\��
	IsControl = false;

	// �v���C���[�̊p�x�C��
	ActionAssistComp->CorrectAttackAngle();

	if (!IsJumping)
	{
		if (!IsAttacking)
		{
			// �U�����t���O�I��
			IsAttacking = true;
		}
		else
		{
			// �R���{�\�Ȏ�,�p��
			if (CanCombo)
			{
				// ���X�g�A�^�b�N�܂ŃR���{�p��
				if (ComboStartSectionNames[ComboIndex] != ComboStartSectionNames.Last())
				{
					++ComboIndex;
				}
			}
		}

		// �U���̃A�j���[�V�����Đ�
		PlayAnimation(PlayerAnimation.ComboAttack[_attackNum], ComboStartSectionNames[ComboIndex]);
	}
	else
	{
		// �U���̃A�j���[�V�����Đ�
		PlayAnimation(PlayerAnimation.AirAttack);
		ComboIndex = 2;
	}
}


/*
 * �֐����@�@�@�@�FGunAttack()
 * �������e�@�@�@�F�e�̍U��
 * �߂�l�@�@�@�@�F�Ȃ�
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
 * �֐����@�@�@�@�FPickUpWeapon()
 * �������e�@�@�@�F������E������
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::PickUpWeapon()
{
	if (!IsControl) { return; }

	if (PlayerController->GetIsPickUpWeapon())
	{
		// �ʂ̕���������Ă���Ƃ�
		if (Weapon != nullptr)
		{
			// �������Ă��镐��������
			Weapon->DetachToHand();
			Weapon = nullptr;
		}
		// ����ȊO
		else
		{
			IsHoldWeapon = true;
		}

		// ������X�V
		Weapon = PlayerController->GetClosestDistanceWeapons();
		WeaponType = Weapon->GetWeaponType();
		Weapon->AttachToHand(this, SocketName[int32(WeaponType)], EOwnerType::OwnerType_Player);

		// ����𗎂Ƃ��܂ł̉񐔂����Z�b�g
		PlayerStatus.WeaponDropLimit = PlayerStatus.DefaultWeaponDropLimit;
	}
}



/*
 * �֐����@�@�@�@�FRotateCharacter()
 * �������e�@�@�@�F�v���C���[�̃X�e�[�^�X������
 * �����P�@�@�@�@�Ffloat _nowInput_X�E�E�E���݂̈ړ����͒l
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::RotateCharacter(float _nowInput_X)
{
	// ���͂��Ȃ��ꍇ�͉������Ȃ�
	if (_nowInput_X == 0) { return; }

	// �E�������Ă��邩�m�F
	IsLookRight = (_nowInput_X != 1.f) ? (true) : (false);

	// ��]
	ActionAssistComp->OwnerParallelToCamera(IsLookRight);
}


/*
 * �֐����@�@�@�@�FRotateCharacter()
 * �������e�@�@�@�F�L�����N�^�[�̈ړ��ʎ擾
 * �����P�@�@�@�@�FFVector _nowPos�E�E�E���݈ʒu
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::AmountOfMovement(FVector _nowPos)
{
	// �����ʒu�ɂ�����X�L�b�v
	if (BeforePos == _nowPos) { return; }

	// �����v�Z
	const float AmountOfMovement = FVector::Dist(_nowPos, BeforePos);

	// �ړ��ʕۑ�
	DistanceAdvanced += AmountOfMovement;

	// ���݂̈ʒu��ۑ�
	BeforePos = _nowPos;
}


/*
 * �֐����@�@�@�@�FSlowDownDeathAnimationRate()
 * �������e�@�@�@�F���S���A�j���[�V���������L�΂�
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SlowDownDeathAnimationRate()
{
	// �x������
	GetMesh()->GlobalAnimRateScale = DeadAnimRate;


	// �v���C���[���폜
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(TimerHandle, this, &APlayerBase::CallGameModeFunc_DestroyPlayer, DeadToGameOverTime, false);
}



/*
 * �֐����@�@�@�@�FSetEnableRootMotion()
 * �������e�@�@�@�FRootMotion�̊J�n�ƈړ�����
 * �����P�@�@�@�@�Fbool _enableRootMotion�E�E�E���[�g���[�V�����̃I���I�t
 * �����Q�@�@�@�@�Ffloat _distance�E�E�E�E�E�E�E�A�j���[�V�����ňړ���������
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SetEnableRootMotion(bool _enableRootMotion, float _distance /*= 0*/)
{
	// �A�j���[�V�����ړ��J�n
	EnableRootMotion = _enableRootMotion; 

	// �A�j���[�V�����ł̈ړ��l
	AnimationMoveValue = _distance;
}


/*
 * �֐����@�@�@�@�FRootMotion()
 * �������e�@�@�@�FRootMotion�̎���
 * �����P�@�@�@�@�Ffloat _distance�E�E�E�A�j���[�V�����ňړ���������
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::RootMotion(float _distance)
{
	// �����Ă�������ɂ���Ēl��ϊ�
	const float Distance = (IsLookRight) ? (-_distance) : (_distance);

	// �O���x�N�g���擾
	FVector ForwardVector = GetActorForwardVector();

	// ���݌����Ă�������Ɉړ�����
	FVector newVector = ForwardVector * _distance;

	// ���Z
	AddActorWorldOffset(newVector,false);
}


/*
 * �֐����@�@�@�@�FCallGameModeFunc_DestroyPlayer()
 * �������e�@�@�@�F���S���̃Q�[�����[�h���̊֐��Ăяo��
 * �@�@�@�@�@�@�@�@�c�@���c���Ă��烊�X�|�[��,�Ȃ��Ȃ�����v���C���[�폜
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::CallGameModeFunc_DestroyPlayer()
{
	// �R���g���[���[����v���C���[���폜
	if (PlayerController)
	{
		// �c�@�����邤���̓��X�|�[��
		if (PlayerController->GetRemainingLives() > 0)
		{
			// �v���C���[���X�|�[��
			PlayerController->RespawnPlayer();
		}
		// ����ȊO�̓Q�[���I�[�o�[
		else
		{
			// �v���C���[�폜
			PlayerController->DestroyPlayer();
		}
	}
}


/*
 * �֐����@�@�@�@�FResetAllAttackFlags()
 * �������e�@�@�@�F�U���Ɋւ���t���O�����ׂă��Z�b�g
 * �߂�l�@�@�@�@�F�Ȃ�
 */void APlayerBase::ResetAllAttackFlags()
{
	// �U�����̃t���O���Z�b�g
	IsAttacking = false;
	CanCombo = false;
	IsControl = false;
	IsKicking = false;
	ComboIndex = 0;
}


/*
 * �֐����@�@�@�@�FContinuationCombo()
 * �������e�@�@�@�F�R���{�̌p��
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::ContinuationCombo()
{
	IsControl = true;
	CanCombo = true;
}


/*
 * �֐����@�@�@�@�FResetCombo()
 * �������e�@�@�@�F�R���{���Z�b�g
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::ResetCombo()
{
	// �U�����̃t���O�����ׂă��Z�b�g
	ResetAllAttackFlags();

	// �R���g���[���\��Ԃ�
	IsControl = true;
}


void APlayerBase::StopMontage()
{
	//�A�j���[�V�������~�߂�
	GetMesh()->GetAnimInstance()->StopSlotAnimation();
}


/*
 * �֐����@�@�@�@�FSetCollision()
 * �������e�@�@�@�F�U������
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SetCollision()
{
	// ����������Ă���Ƃ������蔻�������
	if (Weapon)
	{
		Weapon->SetCollision();
	}
}

/*
 * �֐����@�@�@�@�FTakedDamage()
 * �������e�@�@�@�F�v���C���[�̔�_���[�W����
 * �����P�@�@�@�@�Ffloat _damage�E�E�E��_���[�W��
 * �����Q�@�@�@�@�Fbool _isLastAttack�E�E�E�R���{�̍ŏI���ǂ���
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::TakedDamage(float _damage, bool _isLastAttack /*= false*/)
{
	if (IsDeath || IsInvincibility || AbsolutelyInvincible) { return; }

	// ����������Ă��Ȃ��Ƃ��ɍU�����󂯂��玀�S
	if (!IsHoldWeapon && !IsDeath)
	{
		// �R���g���[���s�\��
		IsControl = false;

		// ���S��Ԃ�
		IsDeath = true;

		// �R���W�������I�t��
		SetActorEnableCollision(true);

		// �q�b�g�G�t�F�N�g����
		ActionAssistComp->SpawnEffect(HitEffect, GetActorLocation());

		// ���S�A�j���[�V�����Đ�
		PlayAnimation(PlayerAnimation.Death);
	}
	// ����������Ă���Ƃ�
	else
	{
		// �U�����̃t���O�����ׂă��Z�b�g
		ResetAllAttackFlags();

		// �G�̃R���{���ŏI�i���������K������𗎂Ƃ�
		if (_isLastAttack)
		{
			PlayerStatus.WeaponDropLimit = 0;
		}

		// ���G�������Ԋi�[�p
		float InvincibilityReleaseTime;

		// ��_���[�W�A�j���[�V����
		if (PlayerStatus.WeaponDropLimit <= 0)
		{
			// �������G���Ԃ�K�p
			InvincibilityReleaseTime = PlayerStatus.InvincibilityTime_Long;

			// ����𗎂Ƃ�
			if (Weapon && IsHoldWeapon)
			{
				Weapon->DetachToHand();
				Weapon = nullptr;
				WeaponType = EWeaponType::WeaponType_None;
				IsHoldWeapon = false;
			}

			// �m�b�N�o�b�N�A�j���[�V�����Đ�
			PlayAnimation(PlayerAnimation.KnockBack);
		}
		else
		{
			// �Z�����G���Ԃ�K�p
			InvincibilityReleaseTime = PlayerStatus.InvincibilityTime_Long;

			// �U�����󂯂�
			--PlayerStatus.WeaponDropLimit;

			// �̂�����A�j���[�V�����Đ�
			PlayAnimation(PlayerAnimation.TakeDamage);			
		}

		// �q�b�g�G�t�F�N�g����
		ActionAssistComp->SpawnEffect(HitEffect, GetActorLocation());

	}
}

/*
 * �֐����@�@�@�@�FSetDeath()
 * �������e�@�@�@�F����
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::SetDeath()
{
	// ���S�A�j���[�V�����Đ�
	PlayAnimation(PlayerAnimation.Death);
}

/*
 * �֐����@�@�@�@�FPlayAnimation()
 * �������e�@�@�@�F�v���C���[�̃A�j���[�V�����Đ�(�Đ����͑���s��)
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void APlayerBase::PlayAnimation(UAnimMontage* _toPlayAnimMontage, FName _startSectionName /*= "None"*/, float _playRate /*= 1.f*/)
{
	// �R���g���[���s�\��
	IsControl = false;

	// �Đ�����A�j���[�V�������i�[
	UAnimMontage* toPlayAnimMontage = _toPlayAnimMontage;

	// �A�j���[�V�����Đ�
	if (toPlayAnimMontage != nullptr)
	{
		ActionAssistComp->PlayAnimation(_toPlayAnimMontage, _startSectionName, _playRate);
	}
}

