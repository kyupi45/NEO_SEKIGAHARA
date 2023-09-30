// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraSpline.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"

#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACameraSpline::ACameraSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultScene"));
	//RootComponent = SceneComp;

	Tags.Add("CameraSpline");


	SplineComp = CreateDefaultSubobject<USplineComponent>("SplineComp");

	//SplineComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACameraSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

