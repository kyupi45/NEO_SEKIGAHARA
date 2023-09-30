// 更新日：2023/7/7		更新者：董		CharaInfoAPIの作成
// 
// ↓
// 使用方法：CharaInfoAPIを継承して、GetCharacterInfoをオーバーライドする
// GetCharacterInfoの返り値は、キャラクター名、HP、MPの順で返す

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterInfo_UI_API.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API UCharacterInfo_UI_API : public UObject
{
	GENERATED_BODY()

public:
	virtual	 TTuple<FName, int, int> GetCharacterInfo();	//First is character name, second is character Hp
	
	UFUNCTION(BlueprintCallable)
	void SetUI_Enemy();
};
