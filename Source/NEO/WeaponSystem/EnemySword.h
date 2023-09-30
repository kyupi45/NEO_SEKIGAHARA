// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "EnemySword.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API AEnemySword : public AWeaponBase
{
	GENERATED_BODY()
public:
	AEnemySword();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		UStaticMeshComponent* Sword;
};
