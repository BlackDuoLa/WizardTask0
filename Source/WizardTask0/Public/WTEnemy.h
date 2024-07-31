#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/spherecomponent.h"
#include "PaperCharacter.h"
#include "CombatInterface.h"
#include "WTPlayer.h"
#include "Sound/SoundBase.h"
#include "WTEnemy.generated.h"

class AWTPlayer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeath);

UCLASS()
class WIZARDTASK0_API AWTEnemy : public AActor ,public ICombatInterface //继承自两个父类
{
	GENERATED_BODY()
	
public:	
	// 构造函数
	AWTEnemy();

	//定义玩家角色碰撞区的球体，若敌人碰到侧扣除玩家角色生命值，是根组件
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USphereComponent* damageCollision;
	//敌人2D动画组件
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UPaperFlipbookComponent * enemyMesh;
	//设置伤害值
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		float damageVal= 0.2f;
	//生命值(无需最大）
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		float life = 2.0f;

	//受击音效
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USoundBase* SFX_Hit;

protected:
	// 初次执行一次
	virtual void BeginPlay() override;

	//继承接口抽象类的所有函数，否则会报错（即使函数提为空，也需要实现）
	virtual void INT_HitHandle(AActor* damageCauser, float baseDamage) override;
	virtual void INT_Killings(int num) override;

public:	


	// 每帧执行
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
		void HitReaction(AActor* causer, float baseDamage);

	//虚幻引擎中，回调函数必须是UFUNCTION()
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

private:
	//玩家参考
	AWTPlayer* playerRef = nullptr;

	//行走速度
	float realSpeed = 0.0f;

	//行走逻辑
	void MoveToPoint(float detaTime);

	//判断是否抵达目标
	//bool bReached = false;
	//bool bSerPoint = false;
};
