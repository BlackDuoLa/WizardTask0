#include "WTPlayer.h"
#include "PaperFlipbookComponent.h"
#include "Engine.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "Projectile.h"
#include "WTLayout.h"
#include "WTMenu.h"
#include "WTEnemy.h"


// 构造函数
AWTPlayer::AWTPlayer()
{
 	// 是否打开每帧Tick功能
	PrimaryActorTick.bCanEverTick = true;

	//加载球形碰撞体;在蓝图中显示为Damage Collision
	damageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Collision"));
	//设置球形默认半径大小
	damageCollision->SetSphereRadius(120.0f);
	//设置显示外框线
	damageCollision->SetHiddenInGame(false);
	//设置碰撞体颜色为绿色
	damageCollision->ShapeColor = FColor(0.0f, 255.0f, 0.0f);

	//加载巫师的动画组件
	wizardmesh = CreateDefaultSubobject<UPaperFlipbookComponent >(TEXT("wizard Mesh"));
	//设置动画资产
	wizardmesh->SetFlipbook(wizardmation);
	//挂载在碰撞球上
	wizardmesh->SetupAttachment(damageCollision);
	//设置和其他物体均不发生碰撞（仅碰撞球负责检测碰撞）
	wizardmesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	//加载摄像机臂
	cameraBoom = CreateDefaultSubobject<USpringArmComponent >(TEXT("Camera Boom"));
	//设置臂长
	cameraBoom->TargetArmLength = 1200.0f;
	//挂载在碰撞球上
	cameraBoom->SetupAttachment(damageCollision);
	//垂直90°，从上空俯视
	cameraBoom->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	cameraBoom->bEnableCameraLag = true;
	cameraBoom->CameraLagSpeed = 30.0f;

	//加载摄像机
	followCamera = CreateDefaultSubobject<UCameraComponent >(TEXT(" Follow Camera"));
	//挂载在摄像机臂
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);

}

// 激活时执行一次
void AWTPlayer::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(Controller);
	//从玩家控制器组价中获取增强型输入系统
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>
			(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}

	//控制焦点自动聚焦到游戏，无需在点击一次鼠标激活
	PlayerController->SetInputMode(FInputModeGameOnly());

	//显示开始提示
	menuUI = CreateWidget<UWTMenu>(GetWorld(), menuUIClass);
	if (menuUI)menuUI->AddToViewport(1);
}

void AWTPlayer::INT_HitHandle(AActor* damageCauser, float baseDamage)
{
	//击退效果用蓝图连接C++的组件
	HitEffects(damageCauser, baseDamage);

	//受击音效
	UGameplayStatics::SpawnSoundAtLocation(this, SFX_Hit, GetActorLocation());

	//扣除血量
	curLife = curLife - baseDamage;

	//更新UI
	InGameUI->UpdateLife(curLife, maxLife);
	//判断是否死亡
	if (curLife <= 0)
	{

		InGameUI->DeathHandle();

	}
}

// 执行中每帧执行
void AWTPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Pcontroller != nullptr && curLife >0.0f && bstartGame) 
	{
		//开火
		Fire();
	}

}

// 旧版控制按键绑定
void AWTPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//使用新版控制按键绑定
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//绑定方向移动按键
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AWTPlayer::Movement);
		EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &AWTPlayer::ReseMovement);
		//绑定开火按键。Started按下，Competed松开
		EIC->BindAction(IA_Shoot, ETriggerEvent::Started, this, &AWTPlayer::FirePressed);
		EIC->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &AWTPlayer::FireReleased);
		//绑定游戏开始按键，无需Tick
		EIC->BindAction(IA_Start, ETriggerEvent::Started, this, &AWTPlayer::StartGame);
		
		// 绑定生成敌人的按键
		EIC->BindAction(IE_Pressed, ETriggerEvent::Started,this, &AWTPlayer::SpawnAWTEnemy);
}
	
}

void AWTPlayer::INT_Killings(int num)
{
	//每次调用一次，直加1
	KillingNums++;
	InGameUI->Updatekillings(KillingNums);

}

//控制器的操作原理
void AWTPlayer::Movement(const FInputActionValue& val)
{
	if (bstartGame)
	{
		FVector2D MovementVector = val.Get<FVector2D>();

		float targetSpeedX = 0.0f;
		float targetSpeedY = 0.0f;

		//垂直方向
		if (MovementVector.X == 0.0f)
		{
			targetSpeedX = 0.0f;
		}
		else if (MovementVector.X > 0.0f)
		{
			targetSpeedX = maxSpeed;
		}
		else
		{
			targetSpeedX = -maxSpeed;
		}

		//使用插值的方式计算移动距离，保证流畅速度
		currentSpeedX = FMath::FInterpTo(
			currentSpeedX, targetSpeedX, GetWorld()->GetDeltaSeconds(), 2.0f);

		//水平方向
		if (MovementVector.Y == 0.0f)
		{
			targetSpeedY = 0.0f;
		}
		else if (MovementVector.Y > 0.0f)
		{
			targetSpeedY = maxSpeed;
			//朝向右边，注意x,y,z和蓝图中的顺序
			//FRotator中（Pitch.Y,Yaw.Z,Roll.X)
			//蓝图中，Rotator：(Roll.X，Pitch.Y,Yaw.Z）

			wizardmesh->SetRelativeRotation(FRotator(0.0, 90.0f, -90.0f));

		}
		else
		{
			targetSpeedY = -maxSpeed;
			//朝向左边，注意x,y,z和蓝图中的顺序
			//FRotator中（Pitch.Y,Yaw.Z,Roll.X)
			//蓝图中，Rotator：(Roll.X，Pitch.Y,Yaw.Z）
			wizardmesh->SetRelativeRotation(FRotator(0.0, -90.0f, 90.0f));
		}

		currentSpeedY = FMath::FInterpTo(
			currentSpeedY, targetSpeedY, GetWorld()->GetDeltaSeconds(), 2.0f);

		//移动碰撞球的位移
		damageCollision->AddWorldOffset(FVector(currentSpeedX, currentSpeedY, 0.0f));

	}
	
}


void AWTPlayer::ReseMovement()
{
	currentSpeedX = 0.0f;
	currentSpeedY = 0.0f;
}

//控制连发
void AWTPlayer::FirePressed()
{
	
	//用Started模式+Tick功能
	bIsFiringPressed = true;
	//允许连发
	bCanFire = true;
}


void AWTPlayer::FireReleased()
{
	bIsFiringPressed = false;
}

void AWTPlayer::StartGame()
{

	if (!bstartGame)
	{
		//清除menu UI实例
		menuUI->RemoveFromParent();
		
		//主要存入玩家控制器实例，方便调用
		Pcontroller = GetWorld()->GetFirstPlayerController();
		//显示鼠标光标
		Pcontroller->bShowMouseCursor = true;
		//切换控制模式至UI，and Gmae,使鼠标位置在游戏中有效
		Pcontroller->SetInputMode(FInputModeGameAndUI());

		//显示游戏内UI
		InGameUI = CreateWidget<UWTLayout>(GetWorld(), InGameUIClass);

		//如果UI创建成功就显示到屏幕上，1代表优先级
		if (InGameUI)
		{
			InGameUI->AddToViewport(1);
		}

		//初始化血量
		curLife = maxLife;
		//更新UI显示
		InGameUI->UpdateLife(curLife, maxLife);

		//初始化杀敌数
		InGameUI->Updatekillings(0);

		//开始游戏
		bstartGame = true;
	}
	
}

void AWTPlayer::Fire()
{
	//连发条件
	//按下鼠标左键为准，刷新cursorPosition位置
	ECollisionChannel trace = ECC_Visibility;
	//定义结构体用于存放指定函数的返回值,引用传递
	FHitResult hit;
	Pcontroller->GetHitResultUnderCursor(trace, false, hit);

	//刷新cursorPosition的数值位置
	cursorPosition = hit.ImpactPoint;

	if (bIsFiringPressed)
	{
		
		if (bCanFire)
		{
			//立刻将bCanFire设置为False
			bCanFire = false;

			//经过一定间隔后，才允许继续发射，故使用计时器控制bCanFire的值
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle_FireTimerExpired, this, 
				&AWTPlayer::FireTimerExpired, ProjectileInterval);

			//定义子弹发射方向：从角色本身的位置向当前鼠标所有的位置方向发射
			float ProjectYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), cursorPosition).Yaw;
			FRotator ProjectileRotation = FRotator(0.0f, ProjectYaw, 0.0f);

			//测试生成子弹表达式，含安全检查
			AProjectile* newProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, GetActorLocation(), ProjectileRotation);
			//调用Projectile.h类中定义的函数，对子弹参数初始化
			newProjectile->InitializeProjectile(ProjectileSpeed, ProjectileSize);

			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue,TEXT("Firing!!"));
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue,FString::Printf(TEXT("%d"), cursorPosition));
			//播放音效
			UGameplayStatics::SpawnSoundAtLocation(this, SFX_Fire, GetActorLocation());
		}
	}
}

void AWTPlayer::SpawnAWTEnemy()
{
	
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,TEXT("YEASS!!"));
		// 获取当前角色的位置
		FVector PlayerLocation = GetActorLocation();

		// 定义生成位置的偏移量
		//FVector SpawnLocation = PlayerLocation + FVector(500.0f, 0.0f, 0.0f); // 偏移500单位距离
		FVector SpawnLocation = FVector(2100.0f, -250.0f, 70.0f);


		// 定义生成旋转角度
		FRotator SpawnRotation = FRotator(0.0f,0.0f,0.0f);

		// 确保世界存在
		UWorld* World = GetWorld();
		if (World)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("sehngc!!"));
			// 生成敌人
			AWTEnemy* SpawnedEnemy = World->SpawnActor<AWTEnemy>(EnemyClass, SpawnLocation, SpawnRotation);
			//World->SpawnActor<AWTEnemy>(EnemyClass, SpawnLocation, SpawnRotation);
		}
	
}

void AWTPlayer::FireTimerExpired()
{
	bCanFire = true;
}

