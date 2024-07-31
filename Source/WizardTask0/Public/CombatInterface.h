#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class WIZARDTASK0_API ICombatInterface
{
	GENERATED_BODY()

	// 典型用法，定义纯函数，供具体函数体实现
	//接口文件的功能应保持单一性，避免任意添加功能不同的接口函数，不便维护
public:

	//传递施加伤害的对象名称，和伤害值
	virtual void INT_HitHandle(AActor* damageCauser, float beseDamage) = 0;

	//传递死亡数字，当敌人死亡时，传递信息至player

	virtual void INT_Killings(int num) = 0;

};
