#include "WTEnemy.h"
#include "PaperFlipbookComponent.h"
#include "WTPlayer.h"
#include"Sound/SoundBase.h"
#include "Kismet/GamePlayStatics.h"

// Sets default values
AWTEnemy::AWTEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//加载球形碰撞体;在蓝图中显示为Damage Collision
	damageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Collision"));
	//设置球形默认半径大小
	damageCollision->SetSphereRadius(120.0f);
	//设置显示外框线
	damageCollision->SetHiddenInGame(false);
	//设置碰撞体颜色为绿色
	damageCollision->ShapeColor = FColor(0.0f, 255.0f, 0.0f);
	//加载回调函数
	damageCollision->OnComponentBeginOverlap.AddDynamic(this, &AWTEnemy::OnOverlapBegin);

	//加载敌人的动画组件
	enemyMesh = CreateDefaultSubobject<UPaperFlipbookComponent >(TEXT("Enemy Mesh"));
	//挂载在碰撞球上
	enemyMesh->SetupAttachment(damageCollision);
	//设置和其他物体均不发生碰撞（仅碰撞球负责检测碰撞）
	enemyMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	enemyMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, -90.0f));
	enemyMesh->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));

}

// Called when the game starts or when spawned
void AWTEnemy::BeginPlay()
{
	Super::BeginPlay();

	//在游戏运行中获取内容建议写在BeginPlay(),写在构造函数中可能获取不到
	playerRef = Cast<AWTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//行走速度范围，随机取值
	realSpeed = FMath::RandRange(1.0f, 3.0f);

}

void AWTEnemy::INT_HitHandle(AActor* damageCauser, float baseDamage)
{
	//测试用
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::SanitizeFloat(baseDamage));

	//受机反馈（在蓝图中调用C++的类
	HitReaction(damageCauser, baseDamage);
	//播放受击音效
	UGameplayStatics::SpawnSoundAtLocation(this, SFX_Hit, GetActorLocation());
	
	//扣血逻辑
	life = life - baseDamage;
	if (life <= 0.0f)
	{
		ICombatInterface* interface = Cast<ICombatInterface>(playerRef);
		if (interface)
		{
			interface->INT_Killings(1);
		}
		//销毁自身
		Destroy();
	}
}

void AWTEnemy::INT_Killings(int num)
{
}

// Called every frame 每帧执行
void AWTEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//若玩家生命大于0，且bstarGame为真，敌人才能向玩家移动\

	if (playerRef && life > 0.0f&&playerRef->GetStarGame())

	{
		MoveToPoint(DeltaTime);
	}

}

void AWTEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor->ActorHasTag("Player"))
	{
		//尝试调用碰撞对象otherActor,使用强制类型转换模板Cast
		ICombatInterface* interface = Cast<ICombatInterface>(OtherActor);
		if(interface)
		{
			interface->INT_HitHandle(this, damageVal);
			
		}
	}
}

void AWTEnemy::MoveToPoint(float detaTime)
{
	//获得从敌人自身朝向玩家的单位向量
	FVector orientRotation = playerRef->GetActorLocation() - GetActorLocation();
	orientRotation.Normalize(0.1f);

	//移动自身
	AddActorLocalOffset(FVector(
		orientRotation.X * realSpeed,
		orientRotation.Y * realSpeed,
		0
	));

	//以玩家角色为中心，在其左边就面朝右，反之亦然
	bool bRight = GetActorLocation().Y < playerRef->GetActorLocation().Y;
	if (bRight)
	{
		enemyMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, -90.0f));
	}
	else
	{
		enemyMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0f));
	}

}


