// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCamera.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/SplineComponent.h"
#include "PlayerBase.h"
#include "NEO/GameSystem/GameSystem_BattleArea.h"

#include "NEO/GameSystem/TGS_GameMode.h"

// Sets default values
ACharacterCamera::ACharacterCamera()
	:m_splineTagName("")
	, m_pSplineActor(NULL)
	, m_localLength(0.0f)
	, m_moveDistance(0.0f)
	, m_defaultSpeed(100.0f)
	, m_pPlayer(NULL)
	, m_CanMove(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bInheritPitch = false;
	CameraBoom->bDoCollisionTest = false;

	// Create a follow camera
	Follow_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Follow_Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Follow_Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void ACharacterCamera::BeginPlay()
{
	Super::BeginPlay();


	PlayerInfo = GetPlayer();
	m_pPlayer = Cast<APlayerBase>(PlayerInfo);

	ATGS_GameMode* GameMode = Cast<ATGS_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	//AGameSystem_BattleArea BattleArea;

	
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);

	
	if (PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(this, 0.f);
	}

	//// プレイヤーの取得
	//m_pCharaOwner = Cast<APlayerBase>(PawnOwner);

	/*if (GameMode)
	{
		GameMode->SetViewTargetWithBlend(this, 0.5f);
	}*/

	//PlayerInfo = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	//PlayerInfo->SetOwner(this);

	// 初期位置設定
	/*StartPos = PlayerInfo->GetActorLocation();
	PlayerToViewPointDistance = FVector(200.f,400.f,200.f);
	SetActorLocation(StartPos);*/

	//SetActorRotation(FRotator(-25.0, 0.0, 0.0));

	//------------------------スプライン------------------------
	m_pSplineActor = SplineActorInitialize(this, m_splineTagName);
}

// Called every frame
void ACharacterCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//ACharacter* tmp_PlayerInfo = NULL;
	//if (tmp_PlayerInfo) return;
	//tmp_PlayerInfo = GetPlayer();
	if (PlayerInfo == nullptr)
	{

		PlayerInfo = GetPlayer();

		m_pPlayer = Cast<APlayerBase>(PlayerInfo);

		ATGS_GameMode* GameMode = Cast<ATGS_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		//AGameSystem_BattleArea BattleArea;


		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 1);


		if (PlayerController)
		{
			PlayerController->SetViewTargetWithBlend(this, 0.f);
		}


	}
	FVector PlayerPos = m_pPlayer->GetActorLocation();
	
	// 現在座標を取得する
	FVector nowpos = GetActorLocation();
	FVector newpos = nowpos;
	
	// 新しい座標を算出する
	newpos = FMath::VInterpTo(nowpos, PlayerPos, DeltaTime, m_defaultSpeed);

	//SetActorLocation(FVector(StartPos.X - PlayerToViewPointDistance.Z, PlayerPos.Y + PlayerToViewPointDistance.X,StartPos.Z + PlayerToViewPointDistance.Y));

	//------------------------スプライン------------------------
	//SplineActorが存在する場合
	if (m_pSplineActor != NULL)
	{
		////移動距離の更新
		//if(PlayerPos.Y >= GetActorLocation().Y - 520.0f)			//プレイヤーの方が右にいる場合
		//	m_moveDistance = m_moveDistance + (m_defaultSpeed * DeltaTime);
		//else if (PlayerPos.Y < GetActorLocation().Y - 480.0f)	//プレイヤーの方が左にいる場合
		//	m_moveDistance = m_moveDistance - (m_defaultSpeed * DeltaTime);

		//移動距離の更新
		//if (PlayerPos.Y > GetActorLocation().Y - 520.0f && PlayerPos.Y < GetActorLocation().Y - 480.0f || m_CanMove == false)
		//	return;
		//else if (PlayerPos.Y > GetActorLocation().Y - 510.0f)	//プレイヤーの方が右にいる場合
		//	m_moveDistance = m_moveDistance + (m_defaultSpeed * DeltaTime);
		//else if (PlayerPos.Y < GetActorLocation().Y - 490.0f)	//プレイヤーの方が左にいる場合
		//	m_moveDistance = m_moveDistance - (m_defaultSpeed * DeltaTime);


		//if (PlayerPos.Y > GetActorLocation().Y && PlayerPos.Y < GetActorLocation().Y || m_CanMove == false)
		//	return;
		//else if (PlayerPos.Y > GetActorLocation().Y)	//プレイヤーの方が右にいる場合
		//	m_moveDistance = m_moveDistance + (m_defaultSpeed * DeltaTime);
		//else if (PlayerPos.Y < GetActorLocation().Y)	//プレイヤーの方が左にいる場合
		//	m_moveDistance = m_moveDistance - (m_defaultSpeed * DeltaTime);


		
		

		//更新後の新しい座標・回転情報を入れるローカル変数
		FVector newLocation;
		FRotator newRotation;

		//現在のスプライン上の距離から座標、回転を算出
		//GetCurrentInfo0nSpline(m_pPlayer->DistanceAdvanced * m_defaultSpeed * m_pPlayer->deltaTime, newLocation, newRotation);
		GetCurrentInfo0nSpline(m_moveDistance, newLocation, newRotation);


		//if (m_CanMove)
		//	GetCurrentInfo0nSpline(m_pPlayer->DistanceAdvanced * m_defaultSpeed * m_pPlayer->deltaTime, newLocation, newRotation);
		//else
		//	GetCurrentInfo0nSpline(m_pPlayer->DistanceAdvanced, newLocation, newRotation);
		//	SetActorLocation(newLocation);



		newRotation.Roll = -25.0;
		
		if (!m_CanMove)
			return;

		//更新後の座標・回転情報を反映
		//SetActorLocationAndRotation(newpos, m_pPlayer->SplineYawRotation);
	}
}


AActor* ACharacterCamera::SplineActorInitialize(AActor* _pOwnerActor, const FName& _tag)
{
	if (_pOwnerActor == NULL) { return NULL; }

	//ゲーム全体に対するActorの検索コストが高いため、BeginPlayで一回保存しておくだけにする
	//検索対象は全てのActor
	TSubclassOf<AActor> findClass;
	findClass = AActor::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(_pOwnerActor->GetWorld(), findClass, actors);


	//検索結果、Actorがあれば
	if (actors.Num() > 0)
	{
		//そのActorの中を順番に検索
		for (int idx = 0; idx < actors.Num(); idx++)
		{
			AActor* pActor = Cast<AActor>(actors[idx]);

			//タグ名で判断する
			//名前はエディタ上で重複した場合は内部の名前が変わるため、あてにならない
			if (pActor->ActorHasTag(_tag))
			{
				//デバッグ確認
				FString message = FString("Founded Actor	:") + pActor->GetName();
				UE_LOG(LogTemp, Warning, TEXT("%s"), *message);

				return pActor;

			}
		}
	}
	return NULL;

}

void ACharacterCamera::GetCurrentInfo0nSpline(float _length, FVector& _location, FRotator& _rotation)
{
	//Splineがなければ無効
	if (!m_pSplineActor) { return; }

	//検索して最初に見つけたSplineComponentオブジェクトを一つ取得
	UActorComponent* pComponent =
		m_pSplineActor->GetComponentByClass(USplineComponent::StaticClass());

	//SplineComponentに型変換
	USplineComponent* pSplineComp = Cast<USplineComponent>(pComponent);

	//SplineComponentが無ければ無効
	if (!pSplineComp) { return; }

	////ループしない(一周だけ回る)
	//if (_loop)
	//{
	//	m_localLength = _length;
	//}
	////ループする
	//else
	//{
	//	//スプライン全体の長さに合わせた比率を求め、位置を更新させる
	//	float overallLength = pSplineComp->GetSplineLength();
	//	m_localLength = (float)((int)_length % (int)overallLength);

	//}

	//スプライン全体の長さに合わせた比率を求め、位置を更新させる
	float overallLength = pSplineComp->GetSplineLength();
	m_localLength = (float)((int)_length % (int)overallLength);


	//現在のスプラインの位置に合わせた座標・回転情報の値を参照で返す
	_location = pSplineComp->GetLocationAtDistanceAlongSpline(m_localLength,
		ESplineCoordinateSpace::World);
	_rotation = pSplineComp->GetRotationAtDistanceAlongSpline(m_localLength,
		ESplineCoordinateSpace::World);
	m_splineloccation = _location;
}
ACharacter* ACharacterCamera::GetPlayer()
{
	ACharacter*  tmp_PlayerInfo = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (tmp_PlayerInfo == PlayerInfo) {
		return PlayerInfo;
	}
	else {
		tmp_PlayerInfo->SetOwner(this);
		return tmp_PlayerInfo;
	}
}

//// キャラクターをセットする関数
//void ACharacterCamera::GetCharacterOwner(APlayerBase* Chara)
//{
//	m_pCharaOwner = Chara;
//}
