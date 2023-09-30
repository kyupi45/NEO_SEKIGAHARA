//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "BossBase.h"
//#include "BossAIController.h"
//
//
//
//// Sets default values
//ABossBase::ABossBase()
//{
// 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//}
//
//// Called when the game starts or when spawned
//void ABossBase::BeginPlay()
//{
//	Super::BeginPlay();
//
//
//	
//	PlayerInput();
//}
//
//// Called every frame
//void ABossBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}
//
//// Called to bind functionality to input
//void ABossBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}
//
//void ABossBase::PlayerInput()
//{
//
//	ABossAIController* AIController = Cast<ABossAIController>(GetController());
//
//	//ÉvÉåÉCÉÑÅ[
//	APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
//
//	if (AIController != NULL)
//	{
//		AIController->SetPlayerKey(player);
//	}
//	
//	UKismetSystemLibrary::PrintString(this, UKismetSystemLibrary::GetDisplayName(player), true, true, FColor::Blue, 2.f);
//}
//
//
