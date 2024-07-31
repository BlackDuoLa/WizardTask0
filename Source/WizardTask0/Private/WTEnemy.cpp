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

	//����������ײ��;����ͼ����ʾΪDamage Collision
	damageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Collision"));
	//��������Ĭ�ϰ뾶��С
	damageCollision->SetSphereRadius(120.0f);
	//������ʾ�����
	damageCollision->SetHiddenInGame(false);
	//������ײ����ɫΪ��ɫ
	damageCollision->ShapeColor = FColor(0.0f, 255.0f, 0.0f);
	//���ػص�����
	damageCollision->OnComponentBeginOverlap.AddDynamic(this, &AWTEnemy::OnOverlapBegin);

	//���ص��˵Ķ������
	enemyMesh = CreateDefaultSubobject<UPaperFlipbookComponent >(TEXT("Enemy Mesh"));
	//��������ײ����
	enemyMesh->SetupAttachment(damageCollision);
	//���ú������������������ײ������ײ��������ײ��
	enemyMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	enemyMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, -90.0f));
	enemyMesh->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));

}

// Called when the game starts or when spawned
void AWTEnemy::BeginPlay()
{
	Super::BeginPlay();

	//����Ϸ�����л�ȡ���ݽ���д��BeginPlay(),д�ڹ��캯���п��ܻ�ȡ����
	playerRef = Cast<AWTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//�����ٶȷ�Χ�����ȡֵ
	realSpeed = FMath::RandRange(1.0f, 3.0f);

}

void AWTEnemy::INT_HitHandle(AActor* damageCauser, float baseDamage)
{
	//������
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::SanitizeFloat(baseDamage));

	//�ܻ�����������ͼ�е���C++����
	HitReaction(damageCauser, baseDamage);
	//�����ܻ���Ч
	UGameplayStatics::SpawnSoundAtLocation(this, SFX_Hit, GetActorLocation());
	
	//��Ѫ�߼�
	life = life - baseDamage;
	if (life <= 0.0f)
	{
		ICombatInterface* interface = Cast<ICombatInterface>(playerRef);
		if (interface)
		{
			interface->INT_Killings(1);
		}
		//��������
		Destroy();
	}
}

void AWTEnemy::INT_Killings(int num)
{
}

// Called every frame ÿִ֡��
void AWTEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�������������0����bstarGameΪ�棬���˲���������ƶ�\

	if (playerRef && life > 0.0f&&playerRef->GetStarGame())

	{
		MoveToPoint(DeltaTime);
	}

}

void AWTEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor->ActorHasTag("Player"))
	{
		//���Ե�����ײ����otherActor,ʹ��ǿ������ת��ģ��Cast
		ICombatInterface* interface = Cast<ICombatInterface>(OtherActor);
		if(interface)
		{
			interface->INT_HitHandle(this, damageVal);
			
		}
	}
}

void AWTEnemy::MoveToPoint(float detaTime)
{
	//��ôӵ�����������ҵĵ�λ����
	FVector orientRotation = playerRef->GetActorLocation() - GetActorLocation();
	orientRotation.Normalize(0.1f);

	//�ƶ�����
	AddActorLocalOffset(FVector(
		orientRotation.X * realSpeed,
		orientRotation.Y * realSpeed,
		0
	));

	//����ҽ�ɫΪ���ģ�������߾��泯�ң���֮��Ȼ
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


