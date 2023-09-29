// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"
/**
 * 
 */
 UINTERFACE(Blueprintable)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class IEnemyInterface
{
	 GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetAction(const FName& ActionName);
};