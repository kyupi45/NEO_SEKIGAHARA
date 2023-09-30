// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase_WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "EnemyBase_UMG.h"
#include "TGS_GameMode.h"
#include "Kismet/GameplayStatics.h"

void UEnemyBase_WidgetComponent::BeginPlay()
{
	Super::BeginPlay();


	SetWidgetSpace(EWidgetSpace::World);
	if (WidgetClass != nullptr) {
		SetWidgetClass(WidgetClass);
	}
}

void UEnemyBase_WidgetComponent::LookAtPlayer()
{
	ATGS_GameMode* GameMode = Cast<ATGS_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		//Test
		FVector PLayerViewLocation;
		FRotator PlayerViewRotation;

		UGameplayStatics::GetPlayerController(GetWorld(),0)->GetPlayerViewPoint(PLayerViewLocation, PlayerViewRotation);

		SetWorldRotation( FRotator(0, PlayerViewRotation.Yaw + 180, 0) );

		return;

		//FVector LookAtDirection = PLayerViewLocation - GetOwner()->GetActorLocation();
		//FRotator LookAtRotation = LookAtDirection.Rotation();


		////-----------------------------------------------------------------------

		//FVector CameraPos = GameMode->GetCameraLocation();
		//FVector EnemyPos = GetOwner()->GetActorLocation();

		//FVector tmpPos = CameraPos;
		//tmpPos.Normalize();

		////EnemyPos.X = tmpPos.X;
		////EnemyPos.Y = tmpPos.Y;

		//// Camera To Enemy
		//FVector CameraToEnemy = CameraPos - EnemyPos;

		//CameraToEnemy.Z = 0;

		////CameraPos.Normalize();
		//LookAtRotation = CameraToEnemy.Rotation();
		//SetWorldRotation(LookAtRotation);

		return;
	}
}

void UEnemyBase_WidgetComponent::SetHPInfo(float HP, float MaxHP)
{
	if (WidgetClass != nullptr) {
		//Get Character Widget
		UEnemyBase_UMG* CharacterWidget = Cast<UEnemyBase_UMG>(GetWidget());
		if (CharacterWidget != nullptr) {
			//Set HP
			CharacterWidget->SetPlayerInfo(HP, MaxHP);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("CharacterWidget is nullptr"));
		}
	}
}

void UEnemyBase_WidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LookAtPlayer();
}
