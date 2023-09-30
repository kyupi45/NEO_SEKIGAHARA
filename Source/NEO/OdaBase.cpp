// Fill out your copyright notice in the Description page of Project Settings.


#include "OdaBase.h"
#include "GameSystem/TGS_GameMode.h"
#include "GameSystem/EnemyBase_WidgetComponent.h"



// Sets default values
//コンストラクタ+変数の初期化
AOdaBase::AOdaBase() :
	SpawnTimer(0),						//---------------------------
	FlameCounter(0),					//タイマー系
	WaitTime(0),						//---------------------------
	isMove(true),
	OdaMoveEnum(ECPPOdaEnum::Stay1),	//列挙型
	Attack1Delay(0),					//近接攻撃の待機時間
	RandomNum(-1),
	BladeDamage(0.f),
	ShockWaveDamage(10.f),
	UltShockWaveDamage(5.f),
	UltDamage(30.f),
	OdaSpeed(1.f),
	OneMoreShockWave(false),
	ChangeFlontTimer(200),
	isMotionPlaying(false),
	isShockWaveSpawnTiming(false),
	isUltShotTiming(false),
	isUltShot(true),
	UltTimer(550),
	isNotAttackNow(false),
	NotAttackCount(0),
	Attack1WaitTimer(0),
	Attack1WaitingTime(60),
	SwordFirstDamage(5),
	HoldDamageAdd(0),
	SwordAddDamage(5),
	EnemyOneTimeSpawn(0),
	bIsAttacked(false),
	Health(1000.f),
	MaxHealth(1000.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	swordConp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"), true);
	swordConp->SetupAttachment(GetMesh(), "R_Weapon");


	//Box Create
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordComponent"), true);
	BoxComp->SetupAttachment(GetMesh(), "R_Weapon");
	BoxComp->SetRelativeLocation(FVector(0.0f, 0.f, 0.0f));
	BoxComp->SetBoxExtent(FVector(10.f, 70.f, 10.f));

	////UI Create
	//EnemyWidget = CreateDefaultSubobject<UEnemyBase_WidgetComponent>(TEXT("EnemyWidget"));
	//EnemyWidget->SetupAttachment(RootComponent);
	// Assist Create
	ActionAssistComp = CreateDefaultSubobject<UActionAssistComponent>(TEXT("ActionAssist"));

}

// Called when the game starts or when spawned
void AOdaBase::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultController();

	NobunagaMovement = (GetCharacterMovement());
	PlayerChara = nullptr;

	//ディレイを一定時間か攻撃を受けたら処理を切り替える
	SpawnDelay = true;

	//
	isBossHPRock = true;
}


// Called to bind functionality to input
void AOdaBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called every frame
void AOdaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//プレイヤーがリスポーンした時に中身がなくなってしまうので更新
	AActor* Player = GetPlayer();
	//プレイヤーがなかったらこれ以降の処理をスルーする
	if (Player == nullptr)  return;

	if (FVector::Dist(GetActorLocation(), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()) <= 2000.f)
	{

		//Enemy Hp Set
		//EnemyWidget->SetHPInfo(Health, MaxHealth);

		//フレームごとに加算する
		FlameCounter++;


		if (isMove == false)
		{
			return;
		}
		//向きをプレイヤーの方に向ける(60フレーム毎に更新)
		if (FlameCounter % 60 == 0)
		{
			//プレイヤーの方を向く
			ToPlayerRotate();
		}

		//距離を取る
		//X軸
		BossPosX = FVector(GetActorLocation().X, 0.f, 0.f);
		PlayerPosX = FVector(Player->GetActorLocation().X, 0.f, 0.f);

		//Y軸
		BossPosY = FVector(0.f, GetActorLocation().Y, 0.f);
		PlayerPosY = FVector(0.f, Player->GetActorLocation().Y, 0.f);

		//カウンター起動
		WaitTime++;
		if (isAttack1Waiting)
		{
			Attack1Wait();
		}

		//体力が半分以下になったら
		if (Health < MaxHealth / 2.f)
		{
			//二回目を撃つためのタイマーを起動
			UltTimer++;
			if (UltTimer % 600 == 0)//600フレーム後に必殺を撃てるようにする
			{
				//必殺技を撃てるようにする
				isUltShot = false;
			}
		}

		//状態ごとに動きを切り替える
		switch (OdaMoveEnum)
		{
			//待機,動き
		case ECPPOdaEnum::Stay1:
			OdaStay1(WaitTime);
			break;

			//待機,動き
		case ECPPOdaEnum::Stop:
			
			if (FlameCounter > 30)
			{
				OdaMoveEnum = ECPPOdaEnum::Stay1;
			}
			break;

			//正面ダッシュ
		case ECPPOdaEnum::Moveflont:
			//右向きか左向きかで正負を変える
			if (GetActorLocation().Y < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
			{
				BossMove(OdaSpeed, FVector(0.f, 3.f, 0.f));
			}
			else
			{
				BossMove(OdaSpeed, FVector(0.f, -3.f, 0.f));
			}
			//一定時間たったら
			if (WaitTime % 25 == 0)
			{
				//待機に戻す
				BacktoStayCase();
			}
			break;

			//背面ダッシュ
		case ECPPOdaEnum::MoveBack:
			//右向きか左向きかで正負を変える
			if (GetActorLocation().Y < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
			{
				BossMove(OdaSpeed, FVector(0.f, -3.f, 0.f));
			}
			else
			{
				BossMove(OdaSpeed, FVector(0.f, 3.f, 0.f));
			}
			//一定時間たったら
			if (WaitTime % 60 == 0)
			{
				//待機に戻す
				BacktoStayCase();
			}
			break;

			//攻撃１
		case ECPPOdaEnum::Attack1:
			if (Attack1WaitTimer % Attack1WaitingTime == 0)
			{
				OdaAttack1(WaitTime);
			}
			else if (WaitTime % Attack1WaitingTime == 0)
			{
				OdaAttack1(WaitTime);
			}
			break;

			//攻撃２
		case ECPPOdaEnum::Attack2:
			OdaAttack2(WaitTime);

			break;

			//必殺技
		case ECPPOdaEnum::Ultimate:
			OdaUlt(WaitTime);
			break;

		default:
			break;
		}
	}
}

/*
 * 関数名　　　　：ToPlayerRotate()
 * 処理内容　　　：Y軸だけを見てどっち側にいるかを判定する関数
 * 戻り値　　　　：なし
 */
void AOdaBase::ToPlayerRotate()
{
	bool LookRight;

	//ボスがプレイヤーの右にいるとき
	if (GetActorLocation().Y < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
	{
		LookRight = true;
	}
	else
	{
		LookRight = false;
	}

	ActionAssistComp->OwnerParallelToCamera(LookRight);
}

/*
 * 関数名　　　　：Attack1Wait()
 * 処理内容　　　：近接攻撃をした後待機するカウンターを起動する関数
 * 戻り値　　　　：なし
 */
void AOdaBase::Attack1Wait()
{
	Attack1WaitTimer++;
}

/*
 * 関数名　　　　：OdaStay1()
 * 処理内容　　　：攻撃せず待機しているときの関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaStay1(int Timer)
{
	//モーションを流す状態かどうか(isMotionPlayingをtrueにする)
	if (isMotionPlaying != true)
	{
		isMotionPlaying = true;
	}

	else if (FVector::Dist(BossPosX, PlayerPosX) <= 50.f)
	{

		if (isUltShot == false)
		{
			//必殺技
			OdaMoveEnum = ECPPOdaEnum::Ultimate;
			isUltShot = true;
			UltTimer = 0;

			//random固定のリセット
			if (RandomNum != -1)
			{
				RandomNum = -1;
			}
		}
		//プレイヤーとの距離の判定
		else if (FVector::Dist(BossPosY, PlayerPosY) <= 200.f)
		{
			//近接攻撃
			OdaMoveEnum = ECPPOdaEnum::Attack1;
			WaitTime = 0;
			//もし攻撃のディレイの値が入っていなかったら
			if (Attack1Delay != 0)
			{
				Attack1Delay = 10;
			}

			//random固定のリセット
			if (RandomNum != -1)
			{
				RandomNum = -1;
			}
		}
		//離れていたら
		else if (FVector::Dist(BossPosY, PlayerPosY) >= 300.f)
		{
			//遠距離
			OdaMoveEnum = ECPPOdaEnum::Attack2;
			WaitTime = 0;
			//random固定のリセット
			if (RandomNum != -1)
			{
				RandomNum = -1;
			}
		}
		else
		{
			//軸を合わせに行く
			OdaMove1(Timer, 60);
			Attack1Delay = 15;
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------------

	}
	else
	{
		//軸を合わせに行く
		if (GetActorLocation().X < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().X)
		{
			BossMove(OdaSpeed * 2, FVector(1.f, 0.f, 0.f));
		}
		else
		{
			BossMove(OdaSpeed * 2, FVector(-1.f, 0.f, 0.f));
		}

	}
}

/*
 * 関数名　　　　：OdaMove1()
 * 処理内容　　　：軸を合わせるなど動きについての関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaMove1(int DeltaTime, int StopTimer)
{
	if (isAttack1Waiting)
	{
		isAttack1Waiting = false;
	}

	//軸を合わせに行く
	//randomの中に値が入っているか(-1が入っていない状態)
	if (RandomNum == -1)
	{
		//randomで0か1を入れる
		RandomNum = FMath::RandRange(0, 1);
	}

	//前か後ろに動く処理
	if (RandomNum == 0)
	{
		BossMove(OdaSpeed * 2, FVector(0.f, 1.f, 0.f));
	}
	else if (RandomNum == 1)
	{
		BossMove(OdaSpeed * 2, FVector(0.f, -1.f, 0.f));
	}

}
/*
 * 関数名　　　　：OdaAttack1()
 * 処理内容　　　：近距離攻撃をするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaAttack1(int Timer) {
	//UKismetSystemLibrary::PrintString(this, "Attack1", true, true, FColor::Cyan, 2.f, TEXT("None"));
	
	if (Attack1Delay == 0)
	{
		//0だと割り切れないので1を代入しておく
		Attack1Delay = 1;
	}

	//設定した時間を超えたか
	if (Timer % Attack1Delay == 0)
	{
		//モーション(アニメーション)が起動したか
		if (isMotionPlaying == true)
		{
			//アニメーションを流す(今は仮)
			PlayAnimMontage(AnimMontage_BossAttack1);
			//ボスがノックバックしないようにする
			BossHPRock();
			//一度だけ流したいのでフラグを切り替える
			isMotionPlaying = false;
			//ディレイをリセットする
			Attack1Delay = 0;
		}
	}
	if (Timer > 150)
	{
		//もしHPロックが外れていたら
		if (isBossHPRock != true)
		{
			//ダメージを受けた時ノックバックするように
			BossHPRock();
		}
	}

	//アニメーションが終了したら
	if (Timer > 300)
	{
		//ステートを切り替える
		BacktoStayCase();
		//リセット
		NotAttackCount = 0;
		//ノックバック中に攻撃モーションに入るとHPロックが作動し続けてしまうのでここで切り替える
		if (isHPLock == true)
		{
			HPLock();
		}
	}
}


/*
 * 関数名　　　　：OdaAttack2()
 * 処理内容　　　：遠距離攻撃をするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaAttack2(int Timer) {
	//UKismetSystemLibrary::PrintString(this, "Attack2", true, true, FColor::Cyan, 2.f, TEXT("None"));

	//モーション(アニメーション)が起動したか
	if (isMotionPlaying == true)
	{
		//ダメージ値を代入
		BladeDamage = ShockWaveDamage;
		//アニメーションを流す
		PlayAnimMontage(AnimMontage_BossAttack2);
		//一度だけ流したいのでフラグを切り替える
		isMotionPlaying = false;
	}


	//アニメーションの最後に衝撃波を出したいのでNotifyを使って制御する(ShockWaveSpawnFlagChangeにて変数の中身を変更)
	if (isShockWaveSpawnTiming == true)
	{
		//アクターのスポーン処理(ShockWaveSpawnはブループリント上で設定)
		GetWorld()->SpawnActor<AActor>(ShockWaveSpawn, GetActorTransform());
		//一度だけスポーンさせたいので切り替えておく
		isShockWaveSpawnTiming = false;
	}

	//170フレームたったら
	if (Timer % 170 == 0)
	{
		//前ダッシュしてみる
		OdaMoveEnum = ECPPOdaEnum::Moveflont;

		//切り替えるにあたって変数を初期化する
		WaitTime = 0;
		//リセット
		NotAttackCount = 0;
		//ノックバック中に攻撃モーションに入るとHPロックが作動し続けてしまうのでここで切り替える
		if (isHPLock == true)
		{
			HPLock();
		}
	}
}
/*
 * 関数名　　　　：OdaUlt()
 * 処理内容　　　：必殺技攻撃をするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaUlt(int Timer)
{

	if (Timer % 30 == 0 && isUltShotTiming != true)
	{
		isUltShotTiming = true;
	}
	//軸を合わせに行く
	if (isUltShotTiming != true)
	{
		if (GetActorLocation().Y > UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
		{
			BossMove(OdaSpeed * 2, FVector(0.f, 1.f, 0.f));
		}
		else
		{
			BossMove(OdaSpeed * 2, FVector(0.f, -1.f, 0.f));
		}
	}

	//モーション(アニメーション)が起動したか
	else if (isMotionPlaying == true)
	{
		//アニメーションを流す(今は仮)
		PlayAnimMontage(AnimMontage_BossUltimate);
		//ボスがノックバックしないようにする
		BossHPRock();
		//一度だけ流したいのでフラグを切り替える
		isMotionPlaying = false;
	}

	//衝撃波を出したいのでNotifyを使って制御する(UltSpawnFlagChangeにて変数の中身を変更)
	if (isShockWaveSpawnTiming == true)
	{
		//ダメージ値を代入
		BladeDamage = UltShockWaveDamage;
		//アクターのスポーン処理(ShockWaveSpawnはブループリント上で設定)
		GetWorld()->SpawnActor<AActor>(ShockWaveSpawn, GetActorTransform());
		//一度だけスポーンさせたいので切り替えておく
		isShockWaveSpawnTiming = false;
	}

	//アニメーションの最後に必殺技を出したいのでNotifyを使って制御する(UltSpawnFlagChangeにて変数の中身を変更)
	if (isUltSpawnTiming == true)
	{
		//アクターのスポーン処理(UltSpawnはブループリント上で設定)
		GetWorld()->SpawnActor<AActor>(UltSpawn, GetActorTransform());
		//一度だけスポーンさせたいので切り替えておく
		isUltSpawnTiming = false;
	}



	//250フレームたったら
	if (Timer > 250)
	{
		//ステートを切り替える
		OdaMoveEnum = ECPPOdaEnum::Moveflont;
		//切り替えるにあたって変数を初期化する
		WaitTime = 0;
		isUltShotTiming = false;
	}
}
/*
 * 関数名　　　　：AttackFlagChange()
 * 処理内容　　　：AnimNotifyにて変更、攻撃のフラグのon,offについての関数
 * 戻り値　　　　：なし
 */
void AOdaBase::AttackFlagChange()
{
	//反転させる
	IsAttackNow = !IsAttackNow;
}


/*
 * 関数名　　　　：ShockWaveSpawnFlagChange()
 * 処理内容　　　：衝撃波をスポーンさせるための変数を切り替える為の関数
 * 戻り値　　　　：なし
 */
void AOdaBase::ShockWaveSpawnFlagChange()
{
	//衝撃波を出現させる為の変数をtrueに変える
	isShockWaveSpawnTiming = true;
	//もしIsAttackNowがtrueになっていたら
	if (IsAttackNow == true)
	{
		//falseにする
		IsAttackNow = false;
	}
}

/*
 * 関数名　　　　：UltSpawnFlagChange()
 * 処理内容　　　：必殺技をスポーンさせるための変数を切り替える為の関数
 * 戻り値　　　　：なし
 */
void AOdaBase::UltSpawnFlagChange()
{
	//必殺技を出現させる為の変数をtrueに変える
	isUltSpawnTiming = true;
	//もしIsAttackNowがtrueになっていたら
	if (IsAttackNow == true)
	{
		//falseにしてみる
		IsAttackNow = false;
	}
}

/*
 * 関数名　　　　：ApplyDamage()
 * 処理内容　　　：ダメージを受けた時の処理
 * 戻り値　　　　：なし
 */
void AOdaBase::ApplyDamage(float Damage)
{
	//動きを止める
	OdaMoveEnum = ECPPOdaEnum::Stop;

	if (isHPLock != true)
	{
		//実数回分ダメージを受けたら反撃させる処理
		if (NotAttackCount < 5)
		{
			//タイマーをリセット
			WaitTime = 0;
			NotAttackCount++;
		}
		else
		{
			//ここに入るときはボスが反撃する処理なのでそれ相応の速度を返す
			Attack1Delay = 1;
		}
		//ボスのスポーン時待機時間中に攻撃を受けた時
		if (SpawnDelay == true)
		{
			//フラグを切り替える
			SpawnDelay = false;
			//攻撃のディレイをセット
			Attack1Delay = 10;
		}
		
		if (isMove)
		{
			Health -= Damage;
			if (isBossHPRock)
			{
				if (FMath::RandRange(0, 100) <= 15)
				{
					//ノックバックアニメーションを流す
					PlayAnimMontage(AnimMontage_BossBlowAway);

					//背面ダッシュに切り替えて仕切り直す
					OdaMoveEnum = ECPPOdaEnum::MoveBack;

					//アニメーション終了後HPロックを解除してStayに戻す
					FTimerManager& TimerManager = GetWorld()->GetTimerManager();
					TimerManager.SetTimer(TimerHandle_KnockbackAnimationFinish, this, &AOdaBase::BossHPRock, AnimMontage_BossBlowAway->GetPlayLength() - 1.1f, false);
					TimerManager.SetTimer(TimerHandle_KnockbackAnimationFinish, this, &AOdaBase::BacktoStayCase, AnimMontage_BossBlowAway->GetPlayLength(), false);


				}
				else
				{
					//ノックバックのアニメーションを流す
					PlayAnimMontage(AnimMontage_BossKnockMontage);
					// アニメーション終了後Stayに戻す
					FTimerManager & TimerManager = GetWorld()->GetTimerManager();
					TimerManager.SetTimer(TimerHandle_KnockbackAnimationFinish, this, &AOdaBase::BossHPRock, AnimMontage_BossKnockMontage->GetPlayLength() - 1.1f, false);
					TimerManager.SetTimer(TimerHandle_KnockbackAnimationFinish, this, &AOdaBase::BacktoStayCase, AnimMontage_BossKnockMontage->GetPlayLength(), false);

				}
			}
		}

		//HPが0になったら
		if (Health <= 0.f)
		{
			//動けるかどうかの変数をfalseにしておく
			if (isMove == true)
			{
				//フラグを使ってダメージを受けるアニメーションを動かさないようにする
				isMove = false;
				//一応アニメーションを止めておく
				this->StopAnimMontage();

				//ゲーム全体の速度を遅くする
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .2f);

				//アニメーションを流す(今は仮)
				PlayAnimMontage(AnimMontage_BossDeath);

				//アニメーションの長さを判定してその時間が過ぎたら死亡処理の関数に飛ばせる処理
				FTimerManager& TimerManager2 = GetWorld()->GetTimerManager();
				TimerManager2.SetTimer(TimerHandle_DeathToGameOver, this, &AOdaBase::Death, AnimMontage_BossDeath->GetPlayLength() - 0.5f, false);

				//一度だけ流したいのでフラグを切り替える
				isMotionPlaying = false;
			}
		}
		//
		else if (Health <= 900.f && EnemyOneTimeSpawn == 0)
		{
			CallEnemy(GunPath, FVector(6350.f,1800.f,1200.f));
			CallEnemy(GunPath, FVector(7150.f, 1800.f, 1200.f));
			//1加算
			EnemyOneTimeSpawn++;
		}
		else if (Health <= 700.f && EnemyOneTimeSpawn == 1)
		{
			CallEnemy(GunPath, FVector(6350.f, 1800.f, 1200.f));
			CallEnemy(GunPath, FVector(7150.f, 1800.f, 1200.f));
			CallEnemy(LancePath, FVector(6670.f, 1800.f, 1200.f));
			//1加算
			EnemyOneTimeSpawn++;
		}
		else if (Health <= 400.f && EnemyOneTimeSpawn == 2)
		{
			CallEnemy(GunPath, FVector(6350.f, 1800.f, 1200.f));
			CallEnemy(GunPath, FVector(7150.f, 1800.f, 1200.f));
			CallEnemy(LancePath, FVector(6670.f, 1800.f, 1200.f));
			CallEnemy(SwordPath, FVector(6510.f, 1800.f, 1200.f));
			CallEnemy(SwordPath, FVector(6830.f, 1800.f, 1200.f));
			//1加算
			EnemyOneTimeSpawn++;
		}
	}
}

/*
 * 関数名　　　　：CallEnemy()
 * 処理内容　　　：敵を呼ぶ処理
 * 引数１	　　：パスの文字列(敵の種類によって変更)
 * 引数２	　　：座標の設定
 * 戻り値　　　　：なし
 */
void AOdaBase::CallEnemy(FString path , FVector Location)
{
	TSubclassOf<class AActor> Enemy = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous(); // 上記で設定したパスに該当するクラスを取得
	if (Enemy != nullptr)
	{
		AActor* a = GetWorld()->SpawnActor<AActor>(Enemy); // スポーン処理
		a->SetActorLocation(Location); // 確認しやすいように座標を設定
	}
}

/*
 * 関数名　　　　：WorldTimeReturn()
 * 処理内容　　　：ずっと遅いままだとストレスがたまるので元の速度に戻す(アニメーション通知で管理)
 * 戻り値　　　　：なし
 */
void AOdaBase::WorldTimeReturn()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

/*
 * 関数名　　　　：BossHPRock()
 * 処理内容　　　：ボスをノックバックさせないようにする処理
 * 戻り値　　　　：なし
 */
void AOdaBase::BossHPRock()
{
	isBossHPRock = !isBossHPRock;
}



/*
 * 関数名　　　　：BacktoStayCase()
 * 処理内容　　　：ボスの行動を待機に戻す処理
 * 戻り値　　　　：なし
 */
void AOdaBase::BacktoStayCase()
{
	//待機に戻す
	OdaMoveEnum = ECPPOdaEnum::Stay1;
	//切り替えるにあたって変数を初期化する
	WaitTime = 0;
	//もしHPロックが外れていたら
	if (isBossHPRock != true)
	{
		//ダメージを受けた時ノックバックするように
		BossHPRock();
	}
}

void AOdaBase::BossKnockback()
{
	//
	if (!isBossHPRock)
	{
		PlayAnimMontage(AnimMontage_BossBlowAway);
	}
}

/*
 * 関数名　　　　：HPLock()
 * 処理内容　　　：HPロック用のスイッチを切り替える処理(プレイヤー側)
 * 戻り値　　　　：なし
 */
void AOdaBase::HPLock()
{
	//HPロック用のスイッチを切り替える
	isHPLock = !isHPLock;
}

//動き関連

/*
 * 関数名　　　　：BossMove()
 * 処理内容　　　：正面方向に歩く処理
 * 戻り値　　　　：なし
 */
void AOdaBase::BossMove(float Speed, FVector MoveSize)
{
	//正面方向に歩く処理
	AddMovementInput(MoveSize, 1.f);
}

/*
 * 関数名　　　　：GetPlayer()
 * 処理内容　　　：プレイヤーを取得する処理
 * 戻り値　　　　：AActor(プレイヤーの情報)
 */
AActor* AOdaBase::GetPlayer()
{
	PlayerChara = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return PlayerChara;
}

/*
 * 関数名　　　　：toPlayerAttacked()
 * 処理内容　　　：プレイヤーに対してのHPロックするための関数
 * 戻り値　　　　：AActor(プレイヤーの情報)
 */
void AOdaBase::toPlayerAttacked()
{
	if (bIsAttacked)
	{
		bIsAttacked = false;
	}
}

/*
 * 関数名　　　　：Is1Combo()
 * 処理内容　　　：近距離攻撃のコンボをするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::Is1Combo()
{
	if (!bIsAttacked || Combo1Counter >= 3)
	{
		//アニメーションを止める
		GetMesh()->GetAnimInstance()->StopSlotAnimation();
		//変数をリセット
		isAttack1Waiting = true;
		Combo1Counter = 0;
		HoldDamageAdd = 0;
	}
	else
	{
		Combo1Counter++;
	}
}
/*
 * 関数名　　　　：Is2Combo()
 * 処理内容　　　：遠距離攻撃のコンボをするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::Is2Combo()
{
	if (Combo2Counter == 0)
	{
		//HPが2/3以下じゃなかったら以下の処理
		if (Health > MaxHealth * (2.f / 3.f))
		{
			//アニメーションを止める
			GetMesh()->GetAnimInstance()->StopSlotAnimation();
			//変数をリセット
			Combo2Counter = 0;

		}
		else
		{
			Combo2Counter++;
		}
	}
	else if (Combo2Counter == 1)
	{
		//HPが1/3以下じゃなかったら以下の処理
		if (Health > MaxHealth * (1.f / 3.f))
		{
			//アニメーションを止める
			GetMesh()->GetAnimInstance()->StopSlotAnimation();
			//変数をリセット
			Combo2Counter = 0;
		}
		else
		{
			Combo2Counter++;
		}
	}
	else if (Combo2Counter)
	{
		//変数をリセット
		Combo2Counter = 0;
	}
	//UKismetSystemLibrary::PrintString(this, LastAttack() ? TEXT("true") : TEXT("false"), true, true, FColor::Cyan, 2.f, TEXT("None"));
}
/*
 * 関数名　　　　：LastAttack()
 * 処理内容　　　：プレイヤーをノックバックさせるために最後の攻撃かどうかをとる関数
 * 戻り値　　　　：bool(最後の攻撃ならtrueをかえす)
 */
bool AOdaBase::LastAttack()
{
	if (isMove)
	{
		if (Combo1Counter >= 3)
		{
			return true;
		}

		else if (Combo2Counter == 2)
		{
			return true;
		}
	}
	return false;
}

//下のDeathを呼ぶための関数
void AOdaBase::DeathMotion()
{
	Death();
}

/*
 * 関数名　　　　：Death()
 * 処理内容　　　：体力が0以下になったときに動く関数
 * 戻り値　　　　：なし
 */
void AOdaBase::Death()
{
	//ゲームモードにてこのアクタを消す処理
	ATGS_GameMode* GameMode = Cast<ATGS_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		if (this != NULL)
		{
			//現在エラーで落ちるので普通に消す
			//ゲームモードを用いて消す
			GameMode->DestroyEnemy(this, IsAreaEnemy);
			GameMode->SetState_GameClear(0);
		}
	}
}

/*
 * 関数名　　　　：CheckOverlap()
 * 処理内容　　　：ボックスコリジョンに当たっているかチェック
 * 戻り値　　　　：なし
 */
void AOdaBase::CheckOverlap()
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FVector Start = BoxComp->GetComponentLocation();
	FVector End = Start;// + GetActorForwardVector() * 100.0f;
	FQuat Rot = BoxComp->GetComponentQuat();			// 
	FCollisionShape CollisionBox = FCollisionShape::MakeBox(BoxComp->GetUnscaledBoxExtent());

	bool isHit = GetWorld()->SweepMultiByChannel(HitResults, Start, End, Rot, ECollisionChannel::ECC_GameTraceChannel1, CollisionBox, CollisionParams);

	if (isHit != true) { return; }

	if (false) {
		DrawDebugCapsule(GetWorld(), (Start + End) / 2,
			CollisionBox.GetCapsuleHalfHeight(),
			CollisionBox.GetCapsuleRadius(), Rot, FColor::Red, true, -1.0f, 0, 1.0f);
	}

	for (FHitResult HitResult : HitResults) {
		PlayerOnOverlap(HitResult);
	}
}

/*
 * 関数名　　　　：CheckOverlap()
 * 処理内容　　　：プレイヤーが当たったらの処理
 * 戻り値　　　　：なし
 */
void AOdaBase::PlayerOnOverlap(FHitResult& _HitResult)
{
	//Cast
	APlayerBase* Player = Cast<APlayerBase>(_HitResult.GetActor());
	if (Player) {
		//プレイヤーがHPをロックしたらこの処理を通る
		if (bIsAttacked) {
			return;
		}

		if (Combo1Counter == 0)
		{
			Player->TakedDamage(SwordFirstDamage, LastAttack());						//プレイヤーにダメージを与える初段
			UKismetSystemLibrary::PrintString(this, LastAttack() ? TEXT("true") : TEXT("false"), true, true, FColor::Cyan, 2.f, TEXT("None"));

		}
		else
		{
			//追加ダメージ値確認用
			//UKismetSystemLibrary::PrintString(this, FString::FromInt(SwordFirstDamage + ((float)SwordFirstDamage*(float)(Combo1Counter/4.f))), true, true, FColor::Cyan, 2.f, TEXT("None"));

			Player->TakedDamage(SwordFirstDamage + ((float)SwordFirstDamage * (float)(Combo1Counter / 4.f)), LastAttack());						//プレイヤーにダメージを与える(後半の処理はコンボ時の追加ダメージ)
			UKismetSystemLibrary::PrintString(this, LastAttack() ? TEXT("true") : TEXT("false"), true, true, FColor::Cyan, 2.f, TEXT("None"));
		}
		//ヒットストップをかける
		ActionAssistComp->HitStop(1.f, .2f);

		//リセット
		bIsAttacked = true;
	}
}
