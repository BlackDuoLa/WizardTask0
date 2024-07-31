#include "Projectile.h"
#include "PaperSpriteComponent.h"
#include "WTPlayer.h"
#include "CombatInterface.h"
#include"Sound/SoundBase.h"
#include "Kismet/GamePlayStatics.h"

// Sets default values
AProjectile::AProjectile() :
	internalSpeed{ 1.0f },
	internaISize{ 0.2f },
	decreaseSpeed{2.0f},
	damageVal{2.0f}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//创建组件实例
	damageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Collision"));
	//默认显示测试线框
	damageCollision->SetHiddenInGame(false);
	//默认半径
	damageCollision->SetSphereRadius(40.0f);
	//动态绑定委托代理，效果即碰撞体若在指定通道遇到任何物体，立刻获取该物体的部分信息
	damageCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);


	//显示子弹的2D图片
	ProjectileSpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Projectile Sprrite"));
	//挂载在damageCollision下
	ProjectileSpriteComp->SetupAttachment(damageCollision);
	//图片本身不造成碰撞
	ProjectileSpriteComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//无影子
	ProjectileSpriteComp->CastShadow = false;
	//设置缩放大小和角度
	ProjectileSpriteComp->SetRelativeRotation(FRotator(0.0, 90.0f, -90.0f));
	ProjectileSpriteComp->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));


}

// 激活时执行一次，虚函数
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

//每帧执行
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//每帧添加位移，默认X正前方
	AddActorLocalOffset(FVector(internalSpeed,0,0));
	//每帧缩小
	//DecreaseProjectileSize(DeltaTime);

}


void AProjectile::InitializeProjectile(float Speed, float Size)
{
	//设置子弹的大小
	SetActorScale3D(FVector(Size, Size, Size));
	//子弹飞行速度，从WTPlayer传过来
	internalSpeed = Speed;

	//设置生命周期，默认4s后销毁
	UWorld* World = GetWorld();
	if(World)
	{
		World->GetTimerManager().SetTimer(TimerHandle_WaitToDestroy, this,
			&AProjectile::TimerDestoryEvent, 4.0f);
	}

}
/*
void AProjectile::DecreaseProjectileSize(float deltaTime)
{
	SetActorScale3D(FVector(
		FMath::FInterpTo(GetActorScale3D().X, internaISize, deltaTime, decreaseSpeed),
		FMath::FInterpTo(GetActorScale3D().Y, internaISize, deltaTime, decreaseSpeed),
		FMath::FInterpTo(GetActorScale3D().Z, internaISize, deltaTime, decreaseSpeed)
	));
}
*/
void AProjectile::TimerDestoryEvent()
{
	//销毁自身
	Destroy();

}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//检查是否碰到有效物体，以及排除自身,以及检查对象是否为指定标签，如果有则继续执行

	if ((OtherActor != nullptr) && (OtherActor->ActorHasTag("enemy")))
	{
		
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue, TEXT("Overlapped!!"));
		
		//尝试调用碰撞对象otherActor,使用强制类型转换模板Cast
		ICombatInterface* interface = Cast<ICombatInterface>(OtherActor);

		//如对象没有加载该接口，程序不会报错，使用更安全
		if (interface)
		{
			interface ->INT_HitHandle(this, damageVal);
			//一发子弹打一个敌人，打到之后消除碰撞
			damageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//立刻销毁
			TimerDestoryEvent();
		}
			
	}
}
