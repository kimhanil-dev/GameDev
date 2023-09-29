// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyType.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct KIMHANIL_API FEnemyType : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TArray<FName> Actions;
};
