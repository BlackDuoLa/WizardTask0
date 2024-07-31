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
	//���캯��
	AProjectile();

protected:
	// ����ʱִ��һ�Σ��麯��
	virtual void BeginPlay() override;

public:	
	// ÿִ֡��
	virtual void Tick(float DeltaTime) override;

public:
		//�Զ������
		UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USphereComponent* damageCollision;

		UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UPaperSpriteComponent* ProjectileSpriteComp;
		//���庯�������ڳ�ʼ���ӵ��ٶȺͳ�ʼ��С��������ͼ����
		void InitializeProjectile(float Speed, float Size);

		//�ӵ��������ԽԶ�����ԽС
		//void DecreaseProjectileSize(float deltatime);


private:
	float internalSpeed ;
	float internaISize;
	float decreaseSpeed;
	float damageVal;

	//��ʱ���̶����������Ĺ̶�������ÿ����ʱ��Ψһ
	FTimerHandle TimerHandle_WaitToDestroy;
	void TimerDestoryEvent();

	//��������У��ص�����������UFUNCTION()
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
