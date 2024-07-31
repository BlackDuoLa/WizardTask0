#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WTLayout.generated.h"

/**
 * 
 */
UCLASS()
class WIZARDTASK0_API UWTLayout : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintImplementableEvent)
		//更新血量，传递数据蓝图
	void UpdateLife(float curLife, float maxLife);

	UFUNCTION(BlueprintImplementableEvent)
		//更新杀敌数，传递数据至蓝图
	void Updatekillings(int killNums);

	UFUNCTION(BlueprintImplementableEvent)
	//如果玩家角色死亡，切换UI的显示界面
	void DeathHandle();

};
