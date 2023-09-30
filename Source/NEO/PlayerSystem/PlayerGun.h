// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "PlayerGun.generated.h"

UCLASS()
class NEO_API APlayerGun : public APlayerBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �v���C���[�̃f�[�^��������
	virtual void SetupPlayerData() override;


	//// �U��
	//virtual void ComboAttack(int AttackNum = 0) override;


	//// ��ڂ̃R���{
	//virtual void Combo1() override;

	//// ��ڂ̃R���{
	//virtual void Combo2()override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Mesh", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponSkeletalMesh;
};