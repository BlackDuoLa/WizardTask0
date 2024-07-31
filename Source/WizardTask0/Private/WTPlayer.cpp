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


// ���캯��
AWTPlayer::AWTPlayer()
{
 	// �Ƿ��ÿ֡Tick����
	PrimaryActorTick.bCanEverTick = true;

	//����������ײ��;����ͼ����ʾΪDamage Collision
	damageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Collision"));
	//��������Ĭ�ϰ뾶��С
	damageCollision->SetSphereRadius(120.0f);
	//������ʾ�����
	damageCollision->SetHiddenInGame(false);
	//������ײ����ɫΪ��ɫ
	damageCollision->ShapeColor = FColor(0.0f, 255.0f, 0.0f);

	//������ʦ�Ķ������
	wizardmesh = CreateDefaultSubobject<UPaperFlipbookComponent >(TEXT("wizard Mesh"));
	//���ö����ʲ�
	wizardmesh->SetFlipbook(wizardmation);
	//��������ײ����
	wizardmesh->SetupAttachment(damageCollision);
	//���ú������������������ײ������ײ��������ײ��
	wizardmesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	//�����������
	cameraBoom = CreateDefaultSubobject<USpringArmComponent >(TEXT("Camera Boom"));
	//���ñ۳�
	cameraBoom->TargetArmLength = 1200.0f;
	//��������ײ����
	cameraBoom->SetupAttachment(damageCollision);
	//��ֱ90�㣬���Ͽո���
	cameraBoom->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	cameraBoom->bEnableCameraLag = true;
	cameraBoom->CameraLagSpeed = 30.0f;

	//���������
	followCamera = CreateDefaultSubobject<UCameraComponent >(TEXT(" Follow Camera"));
	//�������������
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);

}

// ����ʱִ��һ��
void AWTPlayer::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(Controller);
	//����ҿ���������л�ȡ��ǿ������ϵͳ
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>
			(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}

	//���ƽ����Զ��۽�����Ϸ�������ڵ��һ����꼤��
	PlayerController->SetInputMode(FInputModeGameOnly());

	//��ʾ��ʼ��ʾ
	menuUI = CreateWidget<UWTMenu>(GetWorld(), menuUIClass);
	if (menuUI)menuUI->AddToViewport(1);
}

void AWTPlayer::INT_HitHandle(AActor* damageCauser, float baseDamage)
{
	//����Ч������ͼ����C++�����
	HitEffects(damageCauser, baseDamage);

	//�ܻ���Ч
	UGameplayStatics::SpawnSoundAtLocation(this, SFX_Hit, GetActorLocation());

	//�۳�Ѫ��
	curLife = curLife - baseDamage;

	//����UI
	InGameUI->UpdateLife(curLife, maxLife);
	//�ж��Ƿ�����
	if (curLife <= 0)
	{

		InGameUI->DeathHandle();

	}
}

// ִ����ÿִ֡��
void AWTPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Pcontroller != nullptr && curLife >0.0f && bstartGame) 
	{
		//����
		Fire();
	}

}

// �ɰ���ư�����
void AWTPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//ʹ���°���ư�����
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//�󶨷����ƶ�����
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AWTPlayer::Movement);
		EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &AWTPlayer::ReseMovement);
		//�󶨿��𰴼���Started���£�Competed�ɿ�
		EIC->BindAction(IA_Shoot, ETriggerEvent::Started, this, &AWTPlayer::FirePressed);
		EIC->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &AWTPlayer::FireReleased);
		//����Ϸ��ʼ����������Tick
		EIC->BindAction(IA_Start, ETriggerEvent::Started, this, &AWTPlayer::StartGame);
		
		// �����ɵ��˵İ���
		EIC->BindAction(IE_Pressed, ETriggerEvent::Started,this, &AWTPlayer::SpawnAWTEnemy);
}
	
}

void AWTPlayer::INT_Killings(int num)
{
	//ÿ�ε���һ�Σ�ֱ��1
	KillingNums++;
	InGameUI->Updatekillings(KillingNums);

}

//�������Ĳ���ԭ��
void AWTPlayer::Movement(const FInputActionValue& val)
{
	if (bstartGame)
	{
		FVector2D MovementVector = val.Get<FVector2D>();

		float targetSpeedX = 0.0f;
		float targetSpeedY = 0.0f;

		//��ֱ����
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

		//ʹ�ò�ֵ�ķ�ʽ�����ƶ����룬��֤�����ٶ�
		currentSpeedX = FMath::FInterpTo(
			currentSpeedX, targetSpeedX, GetWorld()->GetDeltaSeconds(), 2.0f);

		//ˮƽ����
		if (MovementVector.Y == 0.0f)
		{
			targetSpeedY = 0.0f;
		}
		else if (MovementVector.Y > 0.0f)
		{
			targetSpeedY = maxSpeed;
			//�����ұߣ�ע��x,y,z����ͼ�е�˳��
			//FRotator�У�Pitch.Y,Yaw.Z,Roll.X)
			//��ͼ�У�Rotator��(Roll.X��Pitch.Y,Yaw.Z��

			wizardmesh->SetRelativeRotation(FRotator(0.0, 90.0f, -90.0f));

		}
		else
		{
			targetSpeedY = -maxSpeed;
			//������ߣ�ע��x,y,z����ͼ�е�˳��
			//FRotator�У�Pitch.Y,Yaw.Z,Roll.X)
			//��ͼ�У�Rotator��(Roll.X��Pitch.Y,Yaw.Z��
			wizardmesh->SetRelativeRotation(FRotator(0.0, -90.0f, 90.0f));
		}

		currentSpeedY = FMath::FInterpTo(
			currentSpeedY, targetSpeedY, GetWorld()->GetDeltaSeconds(), 2.0f);

		//�ƶ���ײ���λ��
		damageCollision->AddWorldOffset(FVector(currentSpeedX, currentSpeedY, 0.0f));

	}
	
}


void AWTPlayer::ReseMovement()
{
	currentSpeedX = 0.0f;
	currentSpeedY = 0.0f;
}

//��������
void AWTPlayer::FirePressed()
{
	
	//��Startedģʽ+Tick����
	bIsFiringPressed = true;
	//��������
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
		//���menu UIʵ��
		menuUI->RemoveFromParent();
		
		//��Ҫ������ҿ�����ʵ�����������
		Pcontroller = GetWorld()->GetFirstPlayerController();
		//��ʾ�����
		Pcontroller->bShowMouseCursor = true;
		//�л�����ģʽ��UI��and Gmae,ʹ���λ������Ϸ����Ч
		Pcontroller->SetInputMode(FInputModeGameAndUI());

		//��ʾ��Ϸ��UI
		InGameUI = CreateWidget<UWTLayout>(GetWorld(), InGameUIClass);

		//���UI�����ɹ�����ʾ����Ļ�ϣ�1�������ȼ�
		if (InGameUI)
		{
			InGameUI->AddToViewport(1);
		}

		//��ʼ��Ѫ��
		curLife = maxLife;
		//����UI��ʾ
		InGameUI->UpdateLife(curLife, maxLife);

		//��ʼ��ɱ����
		InGameUI->Updatekillings(0);

		//��ʼ��Ϸ
		bstartGame = true;
	}
	
}

void AWTPlayer::Fire()
{
	//��������
	//����������Ϊ׼��ˢ��cursorPositionλ��
	ECollisionChannel trace = ECC_Visibility;
	//����ṹ�����ڴ��ָ�������ķ���ֵ,���ô���
	FHitResult hit;
	Pcontroller->GetHitResultUnderCursor(trace, false, hit);

	//ˢ��cursorPosition����ֵλ��
	cursorPosition = hit.ImpactPoint;

	if (bIsFiringPressed)
	{
		
		if (bCanFire)
		{
			//���̽�bCanFire����ΪFalse
			bCanFire = false;

			//����һ������󣬲�����������䣬��ʹ�ü�ʱ������bCanFire��ֵ
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle_FireTimerExpired, this, 
				&AWTPlayer::FireTimerExpired, ProjectileInterval);

			//�����ӵ����䷽�򣺴ӽ�ɫ�����λ����ǰ������е�λ�÷�����
			float ProjectYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), cursorPosition).Yaw;
			FRotator ProjectileRotation = FRotator(0.0f, ProjectYaw, 0.0f);

			//���������ӵ����ʽ������ȫ���
			AProjectile* newProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, GetActorLocation(), ProjectileRotation);
			//����Projectile.h���ж���ĺ��������ӵ�������ʼ��
			newProjectile->InitializeProjectile(ProjectileSpeed, ProjectileSize);

			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue,TEXT("Firing!!"));
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue,FString::Printf(TEXT("%d"), cursorPosition));
			//������Ч
			UGameplayStatics::SpawnSoundAtLocation(this, SFX_Fire, GetActorLocation());
		}
	}
}

void AWTPlayer::SpawnAWTEnemy()
{
	
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,TEXT("YEASS!!"));
		// ��ȡ��ǰ��ɫ��λ��
		FVector PlayerLocation = GetActorLocation();

		// ��������λ�õ�ƫ����
		//FVector SpawnLocation = PlayerLocation + FVector(500.0f, 0.0f, 0.0f); // ƫ��500��λ����
		FVector SpawnLocation = FVector(2100.0f, -250.0f, 70.0f);


		// ����������ת�Ƕ�
		FRotator SpawnRotation = FRotator(0.0f,0.0f,0.0f);

		// ȷ���������
		UWorld* World = GetWorld();
		if (World)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("sehngc!!"));
			// ���ɵ���
			AWTEnemy* SpawnedEnemy = World->SpawnActor<AWTEnemy>(EnemyClass, SpawnLocation, SpawnRotation);
			//World->SpawnActor<AWTEnemy>(EnemyClass, SpawnLocation, SpawnRotation);
		}
	
}

void AWTPlayer::FireTimerExpired()
{
	bCanFire = true;
}

