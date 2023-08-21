// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "SkillInfo.h"
#include "PlayerInstance.generated.h"
/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UPlayerInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:


#pragma region  PlayerData Management
private:
	UPROPERTY()
	int32 CristalCount = 0;
	UPROPERTY()
	int32 SkillSlotSize = 3;
	UPROPERTY()
	TArray<int32> SkillList;

public:

	UFUNCTION(BlueprintCallable, Category = "PlayerInformation")
	void GetPlayerSkillSlotSize(int32& size);

	UFUNCTION(BlueprintCallable, Category = "PlayerInformation", meta = (ToolTip = "if the skill slot size is smaller than 'int32 : slotNumber', or No skills found on SkillNumber, the value of 'isSucceed' becomes false"))
	void SetPlayerSkillSlot(const int32 skillNumber, const int32 slotNumber, bool& isSucceed);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerInformation")
	void GetPlayerSkillSlot(TArray<int32>& slot);

	//UFUNCTION(BlueprintCallable, category = "PlayerInformation")
	//void GetPlayerInformation()

#pragma endregion

protected:

	virtual void Init();

#pragma region DataTable Data Management
private:
	UPROPERTY()
	TArray<FSkillData> SkillDatas;

public:
	UPROPERTY(BlueprintReadWrite)
	UDataTable* SkillDataTable = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Skill DataTable")
	const TArray<FSkillData>& GetAllSkillData() const;

	UFUNCTION(BlueprintCallable, Category = "Skill DataTable")
	const FSkillData& GetSkillData(const int32& num) const;
#pragma endregion
};

