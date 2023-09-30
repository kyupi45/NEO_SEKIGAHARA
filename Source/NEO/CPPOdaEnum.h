// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPOdaEnum.generated.h"


UENUM(BlueprintType)
enum class ECPPOdaEnum : uint8
{
	Stay1,
	Stop,
	Moveflont,
	MoveBack,
	Attack1,
	Attack2,
	Ultimate
};