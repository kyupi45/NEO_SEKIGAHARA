#include "Soldier.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "NEO/WeaponSystem/WeaponBase.h"

ASoldier::ASoldier()
{
    PrimaryActorTick.bCanEverTick = true;

    ComboCounter = 0;
    AttackCooldown = 1.0f;
    LastAttackTime = -AttackCooldown;
    bIsAttacking = false;
    bIsOnCooldown = false;
    IsIdol = false;
    IsRunning = true;
}
/*
 * �֐����@�@�@�@�FASoldier::BeginPlay
 * �������e�@�@�@�F�����ݒ���s���܂��B����̃X�|�[���ȂǁB
 * �߂�l�@�@�@�@�F�Ȃ��ivoid�j
 */
void ASoldier::BeginPlay()
{
    Super::BeginPlay();

    // �����Spawn
    if (WeaponClass && !Weapon)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);

        if (Weapon)
        {
            Weapon->AttachToHand(this, "enemy_R_handSocket", EOwnerType::OwnerType_Enemy);
        }
    }
}
/*
 * �֐����@�@�@�@�FASoldier::AttackCombo
 * �������e�@�@�@�F�R���{�U���̃��W�b�N�����s���܂��B
 * �߂�l�@�@�@�@�F�Ȃ��ivoid�j
 */
void ASoldier::AttackCombo()
{
    if (bIsOnCooldown) return;
    ComboCounter++;
    if (ComboCounter == 1)
    {
        PlayAnimMontage(Attack, 1, NAME_None);
        Damage = 5.f;
    }
    else if (ComboCounter == 2)
    {
        PlayAnimMontage(Attack2, 1, NAME_None);
        Damage = 10.f;
    }
    else if (ComboCounter >= 3)
    {
        PlayAnimMontage(Attack3, 1, NAME_None);
        bIsOnCooldown = true;
        GetWorld()->GetTimerManager().SetTimer(ComboCooldownTimer, this, &ASoldier::EndComboCooldown, 1.5f, false);
        Damage = 15.f;
        ComboCounter = 0;
    }

    GetWorld()->GetTimerManager().ClearTimer(ComboResetTimer);

    GetWorld()->GetTimerManager().SetTimer(ComboResetTimer, this, &ASoldier::ResetCombo, 2.0f, false);
    IsRunning = false;
    bIsAttacking = true;
}
/*
 * �֐����@�@�@�@�FASoldier::ResetCombo
 * �������e�@�@�@�F�R���{�J�E���^�[�����Z�b�g���A�U����Ԃ��X�V���܂��B
 * �߂�l�@�@�@�@�F�Ȃ��ivoid�j
 */
void ASoldier::ResetCombo()
{
    ComboCounter = 0;
    bIsAttacking = false;
    IsRunning = true;
    IsIdol = false;
}
/*
 * �֐����@�@�@�@�FASoldier::CollisionOn
 * �������e�@�@�@�F����̃R���W������L���ɂ��܂��B
 * �߂�l�@�@�@�@�F�Ȃ��ivoid�j
 */
void ASoldier::CollisionOn()
{
    if (Weapon)
    {
        Weapon->SetCollision();
    }
}
/*
 * �֐����@�@�@�@�FASoldier::Tick
 * �������e�@�@�@�F���t���[���Ăяo����A�L�����N�^�[�̏�Ԃ�ʒu���X�V���܂��B
 * �߂�l�@�@�@�@�F�Ȃ��ivoid�j
 */
void ASoldier::Tick(float DeltaTime)
{
    if (bIsNowDamage )
    {
       
        return;
    }
    Super::Tick(DeltaTime);
    if (PlayerCharacter)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector EnemyLocation = GetActorLocation();
        float DistanceOnY = FMath::Abs(PlayerLocation.Y - EnemyLocation.Y);
        if(DistanceOnY <= 100.0f && GetWorld()->GetTimeSeconds() - LastAttackTime >= AttackCooldown&&Health>0)
        {
            AttackCombo();
            LastAttackTime = GetWorld()->GetTimeSeconds();
        }
    }
    PlayerCharacter = Cast<ACharacter>(GetPlayer());
    if (!PlayerCharacter) return;

    float CurrentDistance = GetDistanceToPlayer();
    FVector DirectionToPlayer = GetPlayerDirection();
    FVector SnappedDirection;
    FVector MoveVector;

    if (Health > 0 && bIsAttacking == false)
    {
        if (CurrentDistance > DesiredDistance)
        {
            SnappedDirection = GetSnappedDirection(DirectionToPlayer);
            MoveVector = SnappedDirection * MoveSpeed * DeltaTime;
            
        }
       
        else
        {
            return; // ���̑��̏ꍇ�͈ړ����Ȃ�
        }
    }
    SetActorLocation(GetActorLocation() + MoveVector);
    if (Health <= 0)
    {
        if (Weapon)
        {
            Weapon->DetachToHand();
            Weapon = nullptr;
        }
    }
}
/*
 * �֐����@�@�@�@�FASoldier::GetSnappedDirection
 * �������e�@�@�@�F�^����ꂽ�����x�N�g���𒲐����܂��B
 * �߂�l�@�@�@�@�F�������ꂽ�����x�N�g���iFVector�j
 */
FVector ASoldier::GetSnappedDirection(const FVector& Direction) const
{
    FVector SnappedDirection = Direction;

    if (FMath::Abs(SnappedDirection.X) > FMath::Abs(SnappedDirection.Y))
    {
        SnappedDirection.Y = 0.0f;
    }
    else
    {
        SnappedDirection.X = 0.0f;
    }

    return SnappedDirection.GetSafeNormal();
}


void ASoldier::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}
/*
 * �֐����@�@�@�@�FASoldier::GetPlayerDirection
 * �������e�@�@�@�F�v���C���[�ւ̕����x�N�g�����擾���܂��B
 * �߂�l�@�@�@�@�F�v���C���[�ւ̕����x�N�g���iFVector�j
 */

FVector ASoldier::GetPlayerDirection() const
{
    if (!PlayerCharacter) return FVector::ZeroVector;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector LancerLocation = GetActorLocation();
    return FVector(PlayerLocation.X - LancerLocation.X, PlayerLocation.Y - LancerLocation.Y, 0.0f).GetSafeNormal();
}
/*
 * �֐����@�@�@�@�FASoldier::GetDistanceToPlayer
 * �������e�@�@�@�F�v���C���[�܂ł̋������擾���܂��B
 * �߂�l�@�@�@�@�F�v���C���[�܂ł̋����ifloat�j
 */
float ASoldier::GetDistanceToPlayer() const
{
    if (!PlayerCharacter) return 0.0f;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector LancerLocation = GetActorLocation();
    return FVector::Distance(PlayerLocation, LancerLocation);
}

/*
 * �֐����@�@�@�@�FASoldier::EndComboCooldown
 * �������e�@�@�@�F�R���{�̃N�[���_�E�����I�����A�L�����N�^�[�̏�Ԃ��X�V���܂��B
 * �߂�l�@�@�@�@�F�Ȃ��ivoid�j
 */
void ASoldier::EndComboCooldown()
{
    bIsOnCooldown = false;
    IsRunning = true;
    IsIdol = false;
}




