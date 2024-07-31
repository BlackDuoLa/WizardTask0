#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/spherecomponent.h"
#include "PaperCharacter.h"
#include "InputActionValue.h"
#include "CombatInterface.h"
#include "Sound/SoundBase.h"
#include "WTPlayer.generated.h"

class USphereComponent;
class UPaperFlipbookComponent;
class UPaperFlipbook;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AProjectile;
class UWTLayout;
class UWTMenu;
class AWTEnemy;

UCLASS()
class WIZARDTASK0_API AWTPlayer : public APawn,public ICombatInterface//加载接口
{
	GENERATED_BODY()

public:
	//构造函数
	AWTPlayer();

protected:
	// 激活时执行一次，虚函数
	virtual void BeginPlay() override;

	//继承接口抽象类的所有函数，否则会报错（即使函数提为空，也需要实现）
	virtual void INT_HitHandle(AActor* damageCauser, float baseDamage) override;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AWTEnemy> EnemyClass;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void SpawnAWTEnemy();

public:	
	// 执行中每帧执行
	virtual void Tick(float DeltaTime) override;

	//旧版控制按键绑定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void INT_Killings(int num) override;

	//控制方向移动
	void Movement(const FInputActionValue& val);
	void ReseMovement();

	//控制开火
	void FirePressed();
	void FireReleased();

	void Fire();

	//控制游戏是否开始
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void HitEffects(AActor* caueser, float Damage);
	

	//变量
	//定义玩家角色碰撞区的球体，若敌人碰到侧扣除玩家角色生命值，是根组件
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USphereComponent* damageCollision;

	//定义播放Flipbook类型动画的组件
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UPaperFlipbookComponent* wizardmesh;

	//定义方便用于在蓝图中选择具体美术资产的Flipbook变量，用于在C++中加载
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UPaperFlipbook* wizardmation;

	//定义摄像机臂组件
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USpringArmComponent* cameraBoom;

	//定义摄像机组件
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UCameraComponent* followCamera;

	//定义增强输入映射IMC（Input Mapping Context)
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputMappingContext* IMC;
	//定义增强输入事件IA（Input Action）
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputAction* IA_Shoot;

	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputAction* IA_Start;

	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputAction* IE_Pressed;

	//保持实例化的玩家控制器
	APlayerController* PlayerController;

	//设置角色的最大速度
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		float maxSpeed = 2.0f;

	//定义生产子弹的类，方便以下拉框的形式在蓝图中选择
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		TSubclassOf<AProjectile>ProjectileClass;

	//设置子弹的移动速度
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		float ProjectileSpeed = 2.0f;
	//设置初始的大小
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		float ProjectileSize = 10.0f;
	//定义子弹发射时间间隔
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		float ProjectileInterval = 0.2f;
	//定义游戏内UI的类
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		TSubclassOf<UWTLayout> InGameUIClass;

	//定义开始游戏内UI的类
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		TSubclassOf<UWTMenu> menuUIClass;

	//定义玩家最大血量
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "PlayerHP")
		float maxLife = 1.0f;

	//受尽音效开火音效
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay ")
		USoundBase* SFX_Fire;
	//开火音效
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay ")
		USoundBase* SFX_Hit;

	

private:
	//游戏开始标识
	bool bstartGame= false;

	//统计敌人数量
	int KillingNums = 0;

	//值
	float currentSpeedX = 0.0f;
	float currentSpeedY = 0.0f;

	//玩家控制器实例（绑定增强型输入后）
	APlayerController* Pcontroller;

	//是否按下开火键
	bool bIsFiringPressed = false;
	//是否开火在冷却CD中
	bool bCanFire = false;

	//光标位置，标识子弹发射的方向
	FVector cursorPosition = FVector(0.0f, 0.0f, 0.0f);
	//连发计时器句柄
	FTimerHandle TimerHandle_FireTimerExpired;
	//发射间隔过去后，此函数bCanFire再次设置为True,允许发射子弹
	void FireTimerExpired();

	//菜单UI指针变量
	UWTLayout* InGameUI = nullptr;

	//菜单UI指针变量
	UWTMenu* menuUI = nullptr;

	//定于运行中在现存血量变量
	float curLife = 0.0;

	public:
		//bstarGame的Getter，明确内联，供外部调用
		inline bool GetStarGame() const { return bstartGame; };

};
