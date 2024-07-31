#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/spherecomponent.h"
#include "PaperCharacter.h"
#include "CombatInterface.h"
#include "WTPlayer.h"
#include "Sound/SoundBase.h"
#include "WTEnemy.generated.h"

class AWTPlayer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeath);

UCLASS()
class WIZARDTASK0_API AWTEnemy : public AActor ,public ICombatInterface //�̳�����������
{
	GENERATED_BODY()
	
public:	
	// ���캯��
	AWTEnemy();

	//������ҽ�ɫ��ײ�������壬������������۳���ҽ�ɫ����ֵ���Ǹ����
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USphereComponent* damageCollision;
	//����2D�������
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UPaperFlipbookComponent * enemyMesh;
	//�����˺�ֵ
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		float damageVal= 0.2f;
	//����ֵ(�������
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		float life = 2.0f;

	//�ܻ���Ч
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USoundBase* SFX_Hit;

protected:
	// ����ִ��һ��
	virtual void BeginPlay() override;

	//�̳нӿڳ���������к���������ᱨ����ʹ������Ϊ�գ�Ҳ��Ҫʵ�֣�
	virtual void INT_HitHandle(AActor* damageCauser, float baseDamage) override;
	virtual void INT_Killings(int num) override;

public:	


	// ÿִ֡��
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
		void HitReaction(AActor* causer, float baseDamage);

	//��������У��ص�����������UFUNCTION()
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

private:
	//��Ҳο�
	AWTPlayer* playerRef = nullptr;

	//�����ٶ�
	float realSpeed = 0.0f;

	//�����߼�
	void MoveToPoint(float detaTime);

	//�ж��Ƿ�ִ�Ŀ��
	//bool bReached = false;
	//bool bSerPoint = false;
};
