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

	//�������ʵ��
	damageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Collision"));
	//Ĭ����ʾ�����߿�
	damageCollision->SetHiddenInGame(false);
	//Ĭ�ϰ뾶
	damageCollision->SetSphereRadius(40.0f);
	//��̬��ί�д���Ч������ײ������ָ��ͨ�������κ����壬���̻�ȡ������Ĳ�����Ϣ
	damageCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);


	//��ʾ�ӵ���2DͼƬ
	ProjectileSpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Projectile Sprrite"));
	//������damageCollision��
	ProjectileSpriteComp->SetupAttachment(damageCollision);
	//ͼƬ���������ײ
	ProjectileSpriteComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//��Ӱ��
	ProjectileSpriteComp->CastShadow = false;
	//�������Ŵ�С�ͽǶ�
	ProjectileSpriteComp->SetRelativeRotation(FRotator(0.0, 90.0f, -90.0f));
	ProjectileSpriteComp->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));


}

// ����ʱִ��һ�Σ��麯��
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

//ÿִ֡��
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//ÿ֡���λ�ƣ�Ĭ��X��ǰ��
	AddActorLocalOffset(FVector(internalSpeed,0,0));
	//ÿ֡��С
	//DecreaseProjectileSize(DeltaTime);

}


void AProjectile::InitializeProjectile(float Speed, float Size)
{
	//�����ӵ��Ĵ�С
	SetActorScale3D(FVector(Size, Size, Size));
	//�ӵ������ٶȣ���WTPlayer������
	internalSpeed = Speed;

	//�����������ڣ�Ĭ��4s������
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
	//��������
	Destroy();

}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//����Ƿ�������Ч���壬�Լ��ų�����,�Լ��������Ƿ�Ϊָ����ǩ������������ִ��

	if ((OtherActor != nullptr) && (OtherActor->ActorHasTag("enemy")))
	{
		
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue, TEXT("Overlapped!!"));
		
		//���Ե�����ײ����otherActor,ʹ��ǿ������ת��ģ��Cast
		ICombatInterface* interface = Cast<ICombatInterface>(OtherActor);

		//�����û�м��ظýӿڣ����򲻻ᱨ��ʹ�ø���ȫ
		if (interface)
		{
			interface ->INT_HitHandle(this, damageVal);
			//һ���ӵ���һ�����ˣ���֮��������ײ
			damageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//��������
			TimerDestoryEvent();
		}
			
	}
}
