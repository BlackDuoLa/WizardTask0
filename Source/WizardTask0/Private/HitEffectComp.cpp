#include "HitEffectComp.h"
#include "PaperFlipbookComponent.h"

// Sets default values for this component's properties
UHitEffectComp::UHitEffectComp()
{
	//不需要Tick
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UHitEffectComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UHitEffectComp::HitEffects(AActor* causer, float KnockDistance, UPaperFlipbookComponent* fp)
{
	//判断传入参数的有效性，以及该组价是否加载在有效对象上
	if (causer && fp && GetOwner())
	{
		//具体被击退的方向，和击退的距离，计算后的位置
		//子弹距离
		FVector causerPos = causer->GetActorLocation();
		//受击者位置
		FVector receiverPos = GetOwner()->GetActorLocation();
		//受击方向
		FVector dir = receiverPos - causerPos;
		//单位向量化,精度0.1即可
		dir.Normalize(0.1f);
		FVector targetPos = FVector(
			receiverPos.X + KnockDistance * dir.X,
			receiverPos.Y + KnockDistance * dir.Y,
			receiverPos.Z);
		//将受击者传送到此位置
		GetOwner()->SetActorLocation(targetPos, true);

		//被击中的瞬间，显示红色，持续0.1秒后恢复原色
		KnockRef = fp;
		fp->SetSpriteColor(FColor(255.0f,0.0f,0.0f));

		//设置计时器
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ChangeColor,this,
			&UHitEffectComp::NormalColor, 0.1f);
	}
}

void UHitEffectComp::NormalColor()
{
	//返回原色
	KnockRef->SetSpriteColor(FColor(255.0f, 255.0f, 255.0f));
}
