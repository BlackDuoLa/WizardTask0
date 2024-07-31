#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class WIZARDTASK0_API ICombatInterface
{
	GENERATED_BODY()

	// �����÷������崿�����������庯����ʵ��
	//�ӿ��ļ��Ĺ���Ӧ���ֵ�һ�ԣ�����������ӹ��ܲ�ͬ�Ľӿں���������ά��
public:

	//����ʩ���˺��Ķ������ƣ����˺�ֵ
	virtual void INT_HitHandle(AActor* damageCauser, float beseDamage) = 0;

	//�����������֣�����������ʱ��������Ϣ��player

	virtual void INT_Killings(int num) = 0;

};
