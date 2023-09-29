// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
/**
 * 
 */
class KIMHANIL_API FKimhanilPinFactory : public FGraphPanelPinFactory
{

	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};
