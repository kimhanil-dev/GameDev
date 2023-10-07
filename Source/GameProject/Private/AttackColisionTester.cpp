// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackColisionTester.h"

DEFINE_LOG_CATEGORY_STATIC(AttackCollisionTester, Error, Warning)

UAttackColisionTester::UAttackColisionTester(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UAttackColisionTester::BeginPlay()
{
	Super::BeginPlay();
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called every frame
void UAttackColisionTester::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (NowWorkingTester)
	{
		NowWorkingTester->Execute();
	}
}

void UAttackColisionTester::AddCollisionTester(const FName& Key, const FCollisionTester& CollisionTester)
{
	CollisionTesters.Add(Key, CollisionTester);
}

void UAttackColisionTester::StartCollisionTester(const FName& Key, bool& IsTesterNotFounded)
{
	IsTesterNotFounded = true;

	auto tester = CollisionTesters.Find(Key);
	if (tester)
	{
		NowWorkingTester = tester;
		IsTesterNotFounded = false;

		SetComponentTickEnabled(true);

		return;
	}

	UE_LOG(AttackCollisionTester, Error, TEXT("%s collision tester not founded"), *Key.ToString());
}

void UAttackColisionTester::StopCollisionTester()
{
	NowWorkingTester = nullptr;
	SetComponentTickEnabled(false);
}
