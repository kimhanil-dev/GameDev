// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_GetEnemyAction.generated.h"

class FBlueprintActionDatabaseRegistrar;
class UEdGraph;
/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UK2Node_GetEnemyAction : public UK2Node
{
	GENERATED_UCLASS_BODY()

	static UINT CountOfNode;
	
	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph);
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	//~ End UK2Node Interface

	/** Get the then output pin */
	UEdGraphPin* GetThenPin() const;
	/** Get the Data Table input pin */
	UEdGraphPin* GetDataTablePin(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const;
	/** Get the spawn transform input pin */
	UEdGraphPin* GetEnemyNamePin() const;
	/** Get the spawn transform input pin */
	UEdGraphPin* GetActionNamePin() const;
	/** Get the exec output pin for when the row was not found */
	UEdGraphPin* GetActionNotFoundPin() const;
	/** Get the result output pin */
	UEdGraphPin* GetResultPin() const;

private:
	/**
		 * Takes the specified "MutatablePin" and sets its 'PinToolTip' field (according
		 * to the specified description)
		 *
		 * @param   MutatablePin	The pin you want to set tool-tip text on
		 * @param   PinDescription	A string describing the pin's purpose
		 */
	void SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const;
};
