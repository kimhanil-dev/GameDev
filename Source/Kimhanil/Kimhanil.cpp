// Fill out your copyright notice in the Description page of Project Settings.

#include "Kimhanil.h"
#include "EdGraphUtilities.h"
#include "Templates/SharedPointer.h"
#include "SGraphPinDataTableRowName.h"
#include "K2Node_SetEnemyAction.h"
#include "BlueprintGraphPanelPinFactory.h"
#include "KimhanilPinFactory.h"

IMPLEMENT_MODULE( FKimhanilModuleImpl, Kimhanil );

void FKimhanilModuleImpl::StartupModule()
{
	FEdGraphUtilities::RegisterVisualPinFactory(MakeShareable(new FKimhanilPinFactory()));
	FEdGraphUtilities::RegisterVisualPinFactory(MakeShareable(new FBlueprintGraphPanelPinFactory()));
}
