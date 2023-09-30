// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include <type_traits>
#include "ActionAssistComponent.h"
#include "NEO/WeaponSystem/WeaponBase.h"
#include <Runtime/Engine/Classes/Components/CapsuleComponent.h>

#include "PlayerBase.generated.h"

// �O���錾
class UBoxComponent;
class USphereComponent;
class UCapsuleComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USplineComponent;


//-----------------inputAction------------------------------------------------------------
USTRUCT(BlueprintType)
struct FMainAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* RunAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* JumpAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* ComboAction1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* ComboAction2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInputAction* PickAction = nullptr;
};
//----------------------------------------------------------------------------------------

//-----------------�v���C���[�̃X�e�[�^�X-------------------------------------------------
USTRUCT(BlueprintType)
struct FPlayerStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float HP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float MaxHP = HP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int WeaponDropLimit = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int DefaultWeaponDropLimit = WeaponDropLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int RemainingLife = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float DamageAmount = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float InvincibilityTime_Short = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float InvincibilityTime_Long = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float ComboDamageFactor = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float JumpHeight = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float RunSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float NowMoveSpeed = 600.f;
};
//----------------------------------------------------------------------------------------

//-----------------�A�j���[�V�����ۊǗp---------------------------------------------------
USTRUCT(BlueprintType)
struct FPlayerAnimation
{
	GENERATED_BODY()

	// �R���{
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		TArray<UAnimMontage*> ComboAttack = { nullptr,nullptr };

	// �e�ł̍U��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* GunAttack = nullptr;

	// �e�ł̍U���Q
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* GunAttack2 = nullptr;

	// �󒆂ɂ���Ƃ��̍U��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* AirAttack = nullptr;

	// ��_���[�W
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* TakeDamage = nullptr;

	// ��_���[�W
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* KnockBack = nullptr;

	// ���S��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Death = nullptr;
};
//----------------------------------------------------------------------------------------

UCLASS()
class NEO_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

public:

	// �R���X�g���N�^
	APlayerBase();

protected:

	//----------------���͂ŌĂяo�����֐�-----------------------------
	// �ړ�
	virtual void Move(const FInputActionValue& _value);

	// �f�����̈ړ�
	void AIMove();

	// �f�����̍U��
	void AIAttack();

	// �f�����̕���E��
	void AIPickUpWeapon();

	// �_�b�V���؂�ւ�
	void Run();

	// �W�����v
	virtual void JumpStart();
	virtual void Jump();

	// �R���{�U��
	virtual void ComboAttack(int _attackNum = 0);

	// ��ڂ̃R���{
	virtual void Attack1();

	// ��ڂ̃R���{
	virtual void Attack2();

	// ���̍U��
	void SwordAttack(int _attackNum);

	// ���̍U��
	void LanceAttack(int _attackNum);

	// �e�̍U��
	void GunAttack(int _attackNum);

	// ����E��
	void PickUpWeapon();
	//-------------------------------------------------------------------


public:

	//-----------------���N���X�ŌĂяo����--------------------------------------------------------------------------------------------
	// AI����
	void SetRelease(bool _isAIPlayer) { IsAIPlayer = _isAIPlayer; }

	// �R���{�p��
	void ContinuationCombo();

	// �R���{���Z�b�g
	void ResetCombo();

	void StopMontage();

	int GetComboIndex()const { return ComboIndex; }

	bool GetKicking()const { return IsKicking; }

	// ���G���Ԑݒ�
	void SetInvincibility(bool _invincibility) { IsInvincibility = _invincibility; }

	// �_���[�W��^���鏈��
	virtual void SetCollision();

	// �A�N�V�����A�V�X�g�R���|�[�l���g���擾
	UActionAssistComponent* GetActionAssistComponent()const { return ActionAssistComp; }

	// �q�b�g�X�g�b�v
	void HitStop(float _speedDuringHitStop, float _stopTime) { ActionAssistComp->HitStop(_speedDuringHitStop,_stopTime); }

	// �J�����V�F�C�N
	void CameraShake() { ActionAssistComp->CameraShake(ShakePattern); }

	// �G�t�F�N�g�X�|�[��
	void SpawnEffect() { ActionAssistComp->SpawnEffect(HitEffect,GetActorLocation()); }
	
	// ����E�s��؂�ւ��鏈��
	UFUNCTION(BlueprintCallable, Category = "Action")
		void SetControl(bool _isControl) { IsControl = _isControl; }

	// ���S���̃A�j���[�V�����̍Đ���x������
	void SlowDownDeathAnimationRate();

	// �^��RootMotion�J�n
	void SetEnableRootMotion(bool _enableRootMotion, float _distance = 0);
	
	// RootMotion
	void RootMotion(float _distance);

	// �_���[�W�ʂ�Ԃ��֐�
	UFUNCTION(BlueprintCallable, Category = "Action")
		float GetDamageAmount()const { return PlayerStatus.DamageAmount * (((float)ComboIndex + 1.f) * PlayerStatus.ComboDamageFactor); }

	// ���݂�HP��Ԃ�
	UFUNCTION(BlueprintCallable, Category = "GetStatus")
		float GetHP()const { return PlayerStatus.HP; }

	// �_���[�W���󂯂鏈��
	UFUNCTION(BlueprintCallable, Category = "Action")
		void TakedDamage(float _damage, bool _isLastAttack = false);

	// ����̎�ގ擾
	UFUNCTION(BlueprintCallable, Category = "WeaponType")
		EWeaponType GetWeaponType()const { return WeaponType; }

	// �_���[�W���󂯂鏈��
	UFUNCTION(BlueprintCallable, Category = "Invincible")
		void SetAbsolutelyInvincible(bool _absolutelyInvincible) { AbsolutelyInvincible = _absolutelyInvincible; }

	// �_���[�W���󂯂鏈��
	UFUNCTION(BlueprintCallable, Category = "SetStatus")
		void SetDeath();

	// �v���C���[���n�ʂɂ��邩
	bool IsPlayerGrounded()const;

	// �ڐG�J�n���ɍs������
	UFUNCTION()
		virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// �ڐG�J�n���ɍs������
	//UFUNCTION()
	//	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//---------------------------------------------------------------------------------------------------------------------------------------------------


private:

	// �L�����N�^�[�̉�]
	void RotateCharacter(float _nowInput_X);

	// �L�����N�^�[�̈ړ��ʎ擾
	void AmountOfMovement(FVector _nowPos);

	// ���S�����Ăяo��
	void CallGameModeFunc_DestroyPlayer();

	// �U���Ɋւ���t���O�����ׂă��Z�b�g
	void ResetAllAttackFlags();

	// �A�j���[�V�����Đ�
	void PlayAnimation(UAnimMontage* _toPlayAnimMontage, FName _startSectionName = "None", float _playRate = 1.f);

protected:

	// �Q�[���J�n���ɌĂяo����鏈��
	virtual void BeginPlay() override;

	// ���t���[���Ăяo����鏈��
	virtual void Tick(float DeltaTime) override;

	// ���͂̃Z�b�g�A�b�v
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//------------------�v���C���[�̃Z�b�g�A�b�v---------------------------------------------------------------------------------------------------------
	// �v���C���[�̃f�[�^��������
	virtual void SetupPlayerData();

	// �v���C���[�̃X�e�[�^�X������
	void SetupPlayerStatus(float _hp = 100.f, int _remainingLife = 3.f, float _damageAmount = 10.f,
							float _invincibilityTime_Short = 0.3f, float _invincibilityTime_Long = 0.5f,
							float _jumpHeight = 150.f,float _comboDamageFactor = 1.f, float _walkSpeed = 10.f, float _runSpeed = 30.f);

	// �{�^���̐ݒ�
	void SetupMainActionMapping();

	// �A�j���[�V�����A�Z�b�g�̐ݒ�
	void SetupAnimationAssets();

	// �����ɂ���ăX�^�e�B�b�N���b�V�����X�P���^�����b�V���̃Z�b�g�A�b�v
	void SetupWeaponMesh(UStaticMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName = "WeaponStaticMesh");
	void SetupWeaponMesh(USkeletalMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName = "WeaponSkeletalMesh");

	// �w�肵���p�X�̃A�j���[�V�����A�Z�b�g��Ԃ�
	UAnimMontage* GetAnimationAsset(TCHAR* _animAssetPath);

	//-----------------�v���C���[�Ǘ��p�ϐ�------------------------------------------------------------------
private:

	// �v���C��[�̃A�N�V�����Ǘ��p
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FMainAction MainActionMapping;

protected:

	// �v���C���[�̃X�e�[�^�X�Ǘ��p
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FPlayerStatus PlayerStatus;

	// �v���C���[�̃A�j���[�V�����Ǘ��p
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FPlayerAnimation PlayerAnimation;

	// ���ݏ������Ă��镐��𔻕ʂ���Enum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponType")
		EWeaponType WeaponType;

	// �v���C���[�̕���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AWeaponBase> WeaponClass;

	// ����̃N���X
	AWeaponBase* Weapon;

	//-------------------------------------------------------------------------------------------------------------


	//-----------------�R���|�[�l���g�ϐ�--------------------------------------------------------------------------
	// �U���̃A�V�X�g�p
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Assist", meta = (AllowPrivateAccess = "true"))
		UActionAssistComponent* ActionAssistComp;

	// ��_���[�W���̃G�t�F�N�g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
		class UNiagaraSystem* HitEffect;

	// �J�����̗h���
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraShakeBase> ShakePattern;

	// �L�����N�^�[�̓���
	UCharacterMovementComponent* CharacterMovementComp;
	//-------------------------------------------------------------------------------------------------------------

	// ���S�A�j���[�V�����œ|��Ă���̍Đ��X�s�[�h(1�Œʏ�X�s�[�h)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"),Category = "GameOver")
		float DeadAnimRate = 0.01f;							

	// ����ł���Q�[���I�[�o�[�܂ł̎���(�b)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float DeadToGameOverTime = 3.f;					

	// �ړ���������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float DistanceAdvanced;

private:

	// ���͉\���ǂ���
	bool IsControl;	

	// AI�ړ�����
	bool IsAIPlayer;

	// �_�b�V�������ǂ���
	bool IsRunning;		

	// �E�������Ă��邩
	bool IsLookRight;

	// �E�ɓ����Ă��邩
	bool MoveRight = false;

	// �R��U�������ǂ���
	bool IsKicking;

	// ������E���邩�ǂ���
	bool IsPickUpWeapon;

	// �W�����v�����ǂ���
	bool IsJumping;

	// EnableRootMotion
	bool EnableRootMotion;

	// ���[�g���[�V�����ł̈ړ��l
	float AnimationMoveValue;

	// �f�o�b�O�p��Ζ��G
	bool AbsolutelyInvincible = false;

	// ���G��Ԃ��ǂ���
	bool IsInvincibility;

	// ���킻�ꂼ��̃\�P�b�g�̖��O
	FName SocketName[3];

	// ����������Ă��邩�ǂ���
	bool IsHoldWeapon;

	// ����ł��邩�ǂ���
	bool IsDeath;

	// �t���[���J�E���g�p
	float frames;	

	// �W�����v�̌v�Z
	const float radPerFrame = 3.14f / 30.f;

	// �W�����v�O�̍���
	float JumpBeforePos_Z;	

	// �O�̈ʒu
	FVector BeforePos;

	// �U�������ǂ���
	bool IsAttacking;						

	// �R���{�p���ł��邩
	bool CanCombo;							

	// ���i�ڂ̃R���{��
	int ComboIndex;		

	// �R���{�̒i��(First,Second,Third�E�E�E)
	TArray<FName> ComboStartSectionNames;	

	// �n���h��
	FTimerHandle TimerHandle;		

	// �Q�[�����[�h�ۑ�
	class ANEOGameMode* pGameMode;

	class ANEOPlayerController* PlayerController;
};