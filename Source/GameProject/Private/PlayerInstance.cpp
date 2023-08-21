// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInstance.h"

void UPlayerInstance::GetPlayerSkillSlotSize(int32& size)
{
	size = this->SkillSlotSize;
}

void UPlayerInstance::SetPlayerSkillSlot(const int32 skillNumber, const int32 slotNumber, bool& isSucceed)
{
	if ((skillNumber > (SkillDatas.Num() -1)) || (slotNumber >= SkillSlotSize))
	{
		isSucceed = false;
	}
	else
	{
		SkillList[slotNumber] = skillNumber;

		isSucceed = true;
	}
}

void UPlayerInstance::GetPlayerSkillSlot(TArray<int32>& slot)
{
	slot = this->SkillList;
}

void UPlayerInstance::Init()
{
	Super::Init();

	if (SkillDataTable == nullptr)
	{
		UE_LOG(LogInit, Error, TEXT("PlayerInstance::Init() : DataTable has no specified (%s)."), *GetPathName());
	}
	else
	{
		TArray<FSkillInfo*> rows;
		SkillDataTable->GetAllRows<FSkillInfo>(TEXT("SkillDataTable"), rows);
		if (rows.Num() > 0)
		{
			SkillDatas.Reserve(rows.Num());
			int32 num = 0;
			for (auto row : rows)
			{
				SkillDatas.Add({ num++ ,*row});
			}
		}
	}

	// TODO :
	// 나중에 Json이나 기타 설정으로 뺄것 (하드코딩 X)
	SkillSlotSize = 3;
	SkillList.SetNum(SkillSlotSize);
}

const TArray<FSkillData>& UPlayerInstance::GetAllSkillData() const
{
	return SkillDatas;
}

const FSkillData& UPlayerInstance::GetSkillData(const int32& num) const
{
	return SkillDatas[num];
}
