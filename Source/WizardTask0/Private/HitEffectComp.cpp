#include "HitEffectComp.h"
#include "PaperFlipbookComponent.h"

// Sets default values for this component's properties
UHitEffectComp::UHitEffectComp()
{
	//����ҪTick
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
	//�жϴ����������Ч�ԣ��Լ�������Ƿ��������Ч������
	if (causer && fp && GetOwner())
	{
		//���屻���˵ķ��򣬺ͻ��˵ľ��룬������λ��
		//�ӵ�����
		FVector causerPos = causer->GetActorLocation();
		//�ܻ���λ��
		FVector receiverPos = GetOwner()->GetActorLocation();
		//�ܻ�����
		FVector dir = receiverPos - causerPos;
		//��λ������,����0.1����
		dir.Normalize(0.1f);
		FVector targetPos = FVector(
			receiverPos.X + KnockDistance * dir.X,
			receiverPos.Y + KnockDistance * dir.Y,
			receiverPos.Z);
		//���ܻ��ߴ��͵���λ��
		GetOwner()->SetActorLocation(targetPos, true);

		//�����е�˲�䣬��ʾ��ɫ������0.1���ָ�ԭɫ
		KnockRef = fp;
		fp->SetSpriteColor(FColor(255.0f,0.0f,0.0f));

		//���ü�ʱ��
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ChangeColor,this,
			&UHitEffectComp::NormalColor, 0.1f);
	}
}

void UHitEffectComp::NormalColor()
{
	//����ԭɫ
	KnockRef->SetSpriteColor(FColor(255.0f, 255.0f, 255.0f));
}
