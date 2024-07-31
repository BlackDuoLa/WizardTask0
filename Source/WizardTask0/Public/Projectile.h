#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/spherecomponent.h"
#include "Projectile.generated.h"

class UPaperSpriteComponent;
class USphereComponent;

UCLASS()
class WIZARDTASK0_API AProjectile : public AActor
{

	GENERATED_BODY()
	
public:	
	//构造函数
	AProjectile();

protected:
	// 激活时执行一次，虚函数
	virtual void BeginPlay() override;

public:	
	// 每帧执行
	virtual void Tick(float DeltaTime) override;

public:
		//自定义变量
		UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USphereComponent* damageCollision;

		UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UPaperSpriteComponent* ProjectileSpriteComp;
		//定义函数，用于初始化子弹速度和初始大小，无需蓝图调用
		void InitializeProjectile(float Speed, float Size);

		//子弹距离玩家越远，体积越小
		//void DecreaseProjectileSize(float deltatime);


private:
	float internalSpeed ;
	float internaISize;
	float decreaseSpeed;
	float damageVal;

	//计时器固定句柄，所需的固定参数，每个计时器唯一
	FTimerHandle TimerHandle_WaitToDestroy;
	void TimerDestoryEvent();

	//虚幻引擎中，回调函数必须是UFUNCTION()
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
