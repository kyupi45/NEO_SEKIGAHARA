// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySword.h"
AEnemySword::AEnemySword()
{
	Sword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	RootComponent = Sword;
}


