#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaperCharacter.h"
#include "HitEffectComp.generated.h"

class UPaperFlipbookComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WIZARDTASK0_API UHitEffectComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// ���캯��
	UHitEffectComp();

protected:
	// 
	virtual void BeginPlay() override;

public:	
	// ��Tick(float Delta)������
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
		void HitEffects(AActor* causer, float KnockDistance, UPaperFlipbookComponent* fp);
		
	//����ԭɫ
	FTimerHandle TimerHandle_ChangeColor;

	void NormalColor();

	UPaperFlipbookComponent* KnockRef;



};
