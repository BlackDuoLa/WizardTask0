#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WTLayout.generated.h"

/**
 * 
 */
UCLASS()
class WIZARDTASK0_API UWTLayout : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintImplementableEvent)
		//����Ѫ��������������ͼ
	void UpdateLife(float curLife, float maxLife);

	UFUNCTION(BlueprintImplementableEvent)
		//����ɱ������������������ͼ
	void Updatekillings(int killNums);

	UFUNCTION(BlueprintImplementableEvent)
	//�����ҽ�ɫ�������л�UI����ʾ����
	void DeathHandle();

};
