// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_SetEnemyAction.generated.h"

class FBlueprintActionDatabaseRegistrar;
class FString;
class UDataTable;
class UEdGraph;
class UEdGraphPin;
class UObject;
class UScriptStruct;
struct FLinearColor;

DECLARE_LOG_CATEGORY_EXTERN(LogKimhanil, Log, All)

/**
 * 
 */
UCLASS()
class KIMHANIL_API UK2Node_SetEnemyAction : public UK2Node
{
	GENERATED_UCLASS_BODY()
	
	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual FText GetTooltipText() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void EarlyValidation(class FCompilerResultsLog& MessageLog) const override;
	virtual void PreloadRequiredAssets() override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	//~ End UK2Node Interface

	virtual bool IsNodePure() const override { return true; }

	/** Get the then output pin */
	UEdGraphPin* GetThenPin() const;
	/** Get the Data Table input pin */
	UEdGraphPin* GetDataTablePin(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const;
	/** Get the spawn transform input pin */
	UEdGraphPin* GetRowNamePin() const;
	/** Get the spawn transform input pin */
	UEdGraphPin* GetActionNamePin() const;
	/** Get the spawn transform output pin */
	UEdGraphPin* GetActionNameOutPin() const;

	void OnDataTableRowListChanged(const UDataTable* DataTable);
private:
	/**
	 * Takes the specified "MutatablePin" and sets its 'PinToolTip' field (according
	 * to the specified description)
	 *
	 * @param   MutatablePin	The pin you want to set tool-tip text on
	 * @param   PinDescription	A string describing the pin's purpose
	 */
	void SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const;

	/** Triggers a refresh which will update the node's widget; aimed at updating the dropdown menu for the RowName input */
	void RefreshRowNameOptions();

	/** Tooltip text for this node. */
	FText NodeTooltip;

	/** Constructing FText strings can be costly, so we cache the node's title */
	FNodeTextCache CachedNodeTitle;
};
