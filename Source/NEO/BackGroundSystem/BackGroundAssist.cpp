// Fill out your copyright notice in the Description page of Project Settings.


#include "BackGroundAssist.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "NEO/PlayerSystem/CharacterCamera.h"
#include "NEO/GameSystem/TGS_GameMode.h"

#define DIRECTION (90.0)

// Sets default values for this component's properties
UBackGroundAssist::UBackGroundAssist()
	: bUseBillboard(true)
{
	// Tick�𖈃t���[���Ăяo�����ǂ���
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UBackGroundAssist::BeginPlay()
{
	Super::BeginPlay();


}


// ���t���[���Ăяo������
void UBackGroundAssist::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ��ɃJ����������
	ToFaceCamera();
}


/*
 * �֐����@�@�@�@�FToFaceCamera()
 * �������e�@�@�@�F��ɃJ�����Ɍ����鏈��
 * �߂�l�@�@�@�@�F�Ȃ�
 */
void UBackGroundAssist::ToFaceCamera()
{
	// �@�\�̃I���E�I�t
	if (!bUseBillboard)
	{
		// �r���{�[�h�������s��Ȃ��ꍇTick()���Ă΂Ȃ��悤�ɂ���
		PrimaryComponentTick.bCanEverTick = false;

		return; 
	}

	// �Q�[�����[�h�쐬
	ATGS_GameMode* GameMode = Cast<ATGS_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) { return; }


	// �J�����̌��݈ʒu�Ɗp�x���擾
	const FVector CameraLocation = GameMode->GetCameraLocation();


	// �J�����̈ʒu���ς���Ă���ΐV�����������v�Z
	if (BeforeCameraPos != CameraLocation)
	{
		// �J�����Ɍ�����v�Z
		FVector ToCamera = GetOwner()->GetActorLocation() - GameMode->GetCameraLocation();
		ToCamera.Y = 0;
		ToCamera.Z = 0;

		FRotator LookAtRotation = ToCamera.Rotation();
		GetOwner()->SetActorRotation(FRotator(LookAtRotation.Pitch, LookAtRotation.Yaw, LookAtRotation.Roll));
	}
}