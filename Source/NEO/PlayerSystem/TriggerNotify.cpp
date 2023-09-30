// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerNotify.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATriggerNotify::ATriggerNotify()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	pTriggerBoxComp = CreateDefaultSubobject<UBoxComponent>("TriggerBoxComp");

	RootComponent = pTriggerBoxComp;

	if (pTriggerBoxComp != NULL)
	{
		pTriggerBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ATriggerNotify::OnOverlap);
	}
}

// Called when the game starts or when spawned
void ATriggerNotify::BeginPlay()
{
	Super::BeginPlay();

	// オーバーラップ接触し始めた時に呼ばれるイベント関数を登録
}

// Called every frame
void ATriggerNotify::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



// オーバーラップし始めた時の処理（オーバーライド）
void ATriggerNotify::OnOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// オーバーラップした際に実行したいイベント
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// オーバーラップしたのがプレイヤーの時のみ反応させたい
		// PlayerCharaBP -> Actor -> Tagsに設定したタグ「Player」で、プレイヤーを識別する
		if (OtherActor->ActorHasTag("Player"))
		{
			//// デバッグ確認用
			// UE_LOG(LogTemp,Warning,TEXT("Goal"));

			// 接触フラグを上げる
			IsEnterBox = true;
		}
	}
}