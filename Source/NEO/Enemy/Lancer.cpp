#include "Lancer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "NEO/WeaponSystem/WeaponBase.h"


ALancer::ALancer()
{
    // キャラクターの移動方式を設
    MaxHealth = 100;
    Health = MaxHealth;
    bIsRandMove = false;
    // キャラクターの移動方式を設定
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
    IsRunning = false;
    IsIdol = true;
}


/*
 * 関数名　　　　：ALancer::BeginPlay
 * 処理内容　　　：初期設定を行います。タイマーの設定、武器のスポーンなど。
 * 戻り値　　　　：なし（void）
 */
void ALancer::BeginPlay()
{
    Super::BeginPlay();

    // プレイヤーキャラクターの参照を取得
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    // 5秒ごとにCheckPlayerInFront関数を実行するタイマーをセット
    GetWorldTimerManager().SetTimer(TimerHandle_CheckPlayerInFront, this, &ALancer::CheckPlayerInFront, 3.0f, true);
    GetWorld()->GetTimerManager().SetTimer(MoveToTargetTimer, this, &ALancer::ChooseNewTarget, 3.0f, true);
    SpawnTime = GetWorld()->GetTimeSeconds();
    // 武器をSpawn
    if (WeaponClass && !Weapon)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);

        if (Weapon)
        {
            Weapon->AttachToHand(this, "enemy_R_handLance", EOwnerType::OwnerType_Enemy);
        }
    }
}
/*
 * 関数名　　　　：ALancer::GetSnappedDirection
 * 処理内容　　　：与えられた方向ベクトルを調整します。
 * 戻り値　　　　：調整された方向ベクトル（FVector）
 */

FVector ALancer::GetSnappedDirection(const FVector& Direction) const
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
/*
 * 関数名　　　　：ALancer::CollisionOn
 * 処理内容　　　：武器のコリジョンを有効にします。
 * 戻り値　　　　：なし（void）
 */
void ALancer::CollisionOn()
{
    if (Weapon)
    {
        Weapon->SetCollision();
    }
}
/*
 * 関数名　　　　：ALancer::Tick
 * 処理内容　　　：毎フレーム呼び出され、キャラクターの状態や位置を更新します。
 * 戻り値　　　　：なし（void）
 */
void ALancer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bIsNowDamage || bShouldSkipNextMovement)
    {
        bShouldSkipNextMovement = false;
        return;
    }
    
   
    
   
    PlayerCharacter = Cast<ACharacter>(GetPlayer());
    if (!PlayerCharacter) return;

    float CurrentDistance = GetDistanceToPlayer();
    FVector DirectionToPlayer = GetPlayerDirection();
    FVector SnappedDirection;
    FVector MoveVector;
    

    if (CurrentDistance < DesiredDistance+100 )
    {
        bIsRandMove = true;
    }
    else if(CurrentDistance > DesiredDistance - 100)
    {
        bIsRandMove = false;
    }
    if (Health > 0)
    {

        
        if (CurrentDistance > DesiredDistance&& bIsRandMove==false)
        {
            SnappedDirection = GetSnappedDirection(DirectionToPlayer);
            MoveVector = SnappedDirection * Speed*3 * DeltaTime;
            IsRunning = true;
            IsIdol = false;
           
        }
       else if(CurrentDistance < DesiredDistance + 100 && CurrentTarget&& bIsRandMove==true) // DesiredDistanceより400m遠い場合
       {
           FVector DirectionToTarget = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
           MoveVector = DirectionToTarget * Speed/2* DeltaTime;
           IsRunning = true;
           IsIdol = false;
           
       }

      
       else if (FMath::Abs(CurrentDistance - DesiredDistance) < 10.0f)
        {
            SnappedDirection = GetSnappedDirection(DirectionToPlayer);
            MoveVector = SnappedDirection * Speed * 3 * DeltaTime;
            IsRunning = false;
            IsIdol = true;
        }
    else
    {
            IsRunning = false;
            IsIdol = true;
    }
    }
    if (Health <= 0)
    {
        if (Weapon)
        {
            Weapon->DetachToHand();
            Weapon = nullptr;
        }
    }
 
    SetActorLocation(GetActorLocation() + MoveVector);
    
}
/*
 * 関数名　　　　：ALancer::ChooseNewTarget
 * 処理内容　　　：新しいターゲットをランダムに選択します。
 * 戻り値　　　　：なし（void）
 */
void ALancer::ChooseNewTarget()
{
    TArray<AActor*> FoundTargetPoints;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Random"), FoundTargetPoints);

    if (FoundTargetPoints.Num() > 0)
    {
        // ランダムなTargetPointを選択する
        CurrentTarget = Cast<ATargetPoint>(FoundTargetPoints[FMath::RandRange(0, FoundTargetPoints.Num() - 1)]);
    }
}
/*
 * 関数名　　　　：ALancer::GetPlayerDirection
 * 処理内容　　　：プレイヤーへの方向ベクトルを取得します。
 * 戻り値　　　　：プレイヤーへの方向ベクトル（FVector）
 */
FVector ALancer::GetPlayerDirection() const
{
    if (!PlayerCharacter) return FVector::ZeroVector;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector LancerLocation = GetActorLocation();
    return FVector(PlayerLocation.X - LancerLocation.X, PlayerLocation.Y - LancerLocation.Y, 0.0f).GetSafeNormal();
}
/*
 * 関数名　　　　：ALancer::GetDistanceToPlayer
 * 処理内容　　　：プレイヤーまでの距離を取得します。
 * 戻り値　　　　：プレイヤーまでの距離（float）
 */
float ALancer::GetDistanceToPlayer() const
{
    if (!PlayerCharacter) return 0.0f;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector LancerLocation = GetActorLocation();
    return FVector::Distance(PlayerLocation, LancerLocation);
}
/*
 * 関数名　　　　：ALancer::CheckPlayerInFront
 * 処理内容　　　：プレイヤーがLancerの前にいるかどうかを確認します。
 * 戻り値　　　　：なし（void）
 */
void ALancer::CheckPlayerInFront()
{
    // 自分の位置を取得
    FVector MyLocation = GetActorLocation();
    IsRunning = true;
    IsIdol = false;
    if (bIsNowDamage || bShouldSkipNextMovement)
    {
        bShouldSkipNextMovement = false;
        return;
    }
        UWorld* World = GetWorld();
        if (World)
        {
            APlayerController* PlayerController = World->GetFirstPlayerController();
            if (PlayerController)
            {
                APawn* Pawn = PlayerController->GetPawn();
                if (Pawn)
                {
                    FVector PlayerLocation = Pawn->GetActorLocation();
                    // 自プレイヤーがLancerの目の前にいるかどうかを判定
                    FVector DirectionToPlayer = PlayerLocation - MyLocation;
                    float DotProduct = FVector::DotProduct(DirectionToPlayer.GetSafeNormal(), GetActorForwardVector());
                    bIsPlayerInFront = DotProduct > 0.0f;

                    if (bIsPlayerInFront&& Health > 0)
                    {
                        if (FMath::FRand() < 0.8f)
                        {
                            PlayAnimMontage(Attack, 1, NAME_None);
                              
                        }

                    }
                }
            }
        }
    
   
    
}












