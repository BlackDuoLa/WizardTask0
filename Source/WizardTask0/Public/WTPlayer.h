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
class WIZARDTASK0_API AWTPlayer : public APawn,public ICombatInterface//���ؽӿ�
{
	GENERATED_BODY()

public:
	//���캯��
	AWTPlayer();

protected:
	// ����ʱִ��һ�Σ��麯��
	virtual void BeginPlay() override;

	//�̳нӿڳ���������к���������ᱨ����ʹ������Ϊ�գ�Ҳ��Ҫʵ�֣�
	virtual void INT_HitHandle(AActor* damageCauser, float baseDamage) override;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AWTEnemy> EnemyClass;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void SpawnAWTEnemy();

public:	
	// ִ����ÿִ֡��
	virtual void Tick(float DeltaTime) override;

	//�ɰ���ư�����
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void INT_Killings(int num) override;

	//���Ʒ����ƶ�
	void Movement(const FInputActionValue& val);
	void ReseMovement();

	//���ƿ���
	void FirePressed();
	void FireReleased();

	void Fire();

	//������Ϸ�Ƿ�ʼ
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void HitEffects(AActor* caueser, float Damage);
	

	//����
	//������ҽ�ɫ��ײ�������壬������������۳���ҽ�ɫ����ֵ���Ǹ����
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USphereComponent* damageCollision;

	//���岥��Flipbook���Ͷ��������
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UPaperFlipbookComponent* wizardmesh;

	//���巽����������ͼ��ѡ����������ʲ���Flipbook������������C++�м���
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UPaperFlipbook* wizardmation;

	//��������������
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		USpringArmComponent* cameraBoom;

	//������������
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay")
		UCameraComponent* followCamera;

	//������ǿ����ӳ��IMC��Input Mapping Context)
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputMappingContext* IMC;
	//������ǿ�����¼�IA��Input Action��
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputAction* IA_Shoot;

	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputAction* IA_Start;

	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "Input")
		UInputAction* IE_Pressed;

	//����ʵ��������ҿ�����
	APlayerController* PlayerController;

	//���ý�ɫ������ٶ�
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		float maxSpeed = 2.0f;

	//���������ӵ����࣬���������������ʽ����ͼ��ѡ��
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		TSubclassOf<AProjectile>ProjectileClass;

	//�����ӵ����ƶ��ٶ�
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		float ProjectileSpeed = 2.0f;
	//���ó�ʼ�Ĵ�С
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		float ProjectileSize = 10.0f;
	//�����ӵ�����ʱ����
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		float ProjectileInterval = 0.2f;
	//������Ϸ��UI����
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		TSubclassOf<UWTLayout> InGameUIClass;

	//���忪ʼ��Ϸ��UI����
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "ProPerties")
		TSubclassOf<UWTMenu> menuUIClass;

	//����������Ѫ��
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "PlayerHP")
		float maxLife = 1.0f;

	//�ܾ���Ч������Ч
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay ")
		USoundBase* SFX_Fire;
	//������Ч
	UPROPERTY(EditAnywhere, BluePrintReadwrite, Category = "GamePlay ")
		USoundBase* SFX_Hit;

	

private:
	//��Ϸ��ʼ��ʶ
	bool bstartGame= false;

	//ͳ�Ƶ�������
	int KillingNums = 0;

	//ֵ
	float currentSpeedX = 0.0f;
	float currentSpeedY = 0.0f;

	//��ҿ�����ʵ��������ǿ�������
	APlayerController* Pcontroller;

	//�Ƿ��¿����
	bool bIsFiringPressed = false;
	//�Ƿ񿪻�����ȴCD��
	bool bCanFire = false;

	//���λ�ã���ʶ�ӵ�����ķ���
	FVector cursorPosition = FVector(0.0f, 0.0f, 0.0f);
	//������ʱ�����
	FTimerHandle TimerHandle_FireTimerExpired;
	//��������ȥ�󣬴˺���bCanFire�ٴ�����ΪTrue,�������ӵ�
	void FireTimerExpired();

	//�˵�UIָ�����
	UWTLayout* InGameUI = nullptr;

	//�˵�UIָ�����
	UWTMenu* menuUI = nullptr;

	//�������������ִ�Ѫ������
	float curLife = 0.0;

	public:
		//bstarGame��Getter����ȷ���������ⲿ����
		inline bool GetStarGame() const { return bstartGame; };

};
