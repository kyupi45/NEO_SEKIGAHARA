// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraSpline.generated.h"

class USplineComponent;
class USceneComponent;

UCLASS()
class NEO_API ACameraSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//ƒXƒvƒ‰ƒCƒ“
		/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spline)
		class USplineComponent* SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spline)
		class USceneComponent* SceneComp;



};
