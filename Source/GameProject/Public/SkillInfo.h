// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSkillInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SkillName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* SkillIcon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UClass* SkillType;
};

USTRUCT(BlueprintType)
struct GAMEPROJECT_API FSkillData
{
	GENERATED_USTRUCT_BODY()

	FSkillData() {}
	FSkillData(const int32 number, const FSkillInfo& tableData)
		: Number(number), SkillName(tableData.SkillName), SkillIcon(tableData.SkillIcon), SkillType(tableData.SkillType) {
		
		UE_LOG(LogInit, Error,TEXT("DataLoaded"));
		}

	~FSkillData() {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Number;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SkillName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* SkillIcon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UClass* SkillType;
};
