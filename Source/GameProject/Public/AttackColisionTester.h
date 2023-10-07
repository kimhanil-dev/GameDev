// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AttackColisionTester.generated.h"

DECLARE_DYNAMIC_DELEGATE(FCollisionTester);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPROJECT_API UAttackColisionTester : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackColisionTester(const FObjectInitializer& ObjectInitializer);

protected:

	TMap<FName, FCollisionTester> CollisionTesters;

	FCollisionTester* NowWorkingTester = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void AddCollisionTester(const FName& Key, const FCollisionTester& CollisionTester);
		
	UFUNCTION(BlueprintCallable)
	void StartCollisionTester(const FName& Key, bool& IsTesterNotFounded);

	UFUNCTION(BlueprintCallable)
	void StopCollisionTester();
};
