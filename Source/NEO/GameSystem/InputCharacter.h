// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"

#include "InputCharacter.generated.h"

//PlayerInput
UENUM(BlueprintType)
enum class ESubAction : uint8
{
	ESubAction_None UMETA(DisplayName = "SubAction_None"),

	ESubAction_Up UMETA(DisplayName = "SubAction_Up"),
	ESubAction_Down UMETA(DisplayName = "SubAction_Down"),
	ESubAction_Left UMETA(DisplayName = "SubAction_Left"),
	ESubAction_Right UMETA(DisplayName = "SubAction_Right"),
	ESubAction_Enter UMETA(DisplayName = "SubAction_Enter"),
	ESubAction_Back UMETA(DisplayName = "SubAction_Back"),
	ESubAction_Menu UMETA(DisplayName = "SubAction_Menu"),
};

UCLASS()
class NEO_API AInputCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AInputCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		class UInputMappingContext* SubMappingContext;

	//上キー
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		class UInputAction* UpAction;

	//下キー
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		class UInputAction* DownAction;

	//左キー
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		class UInputAction* LeftAction;

	//右キー
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		class UInputAction* RightAction;

	//確認
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		class UInputAction* EnterAction;

	//キャンセル
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		class UInputAction* BackAction;

	//メニュー
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameInput")
		class UInputAction* MenuAction;


	//インプット関数
	void ActionUp();
	void ActionDown();
	void ActionLeft();
	void ActionRight();
	void ActionEnter();
	void ActionBack();
	void ActionMenu();

	//ゲームモード取得
	class ATGS_GameMode* GetGamemode();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
