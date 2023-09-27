// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_SetEnemyAction.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Containers/EnumAsByte.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "DataTableEditorUtils.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "Engine/DataTable.h"
#include "Engine/MemberReference.h"
#include "HAL/PlatformMath.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"
#include "KismetCompiler.h"
#include "Math/Color.h"
#include "Misc/AssertionMacros.h"
#include "Styling/AppStyle.h"
#include "Templates/Casts.h"
#include "Templates/ChooseClass.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectBaseUtility.h"
#include "UObject/UnrealNames.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "EnemyInterface.h"
#include "GameFramework/Actor.h"

class UBlueprint;

DEFINE_LOG_CATEGORY(LogKimhanil);


#define LOCTEXT_NAMESPACE "K2Node_SetEnemyAction"

namespace GetDataTableRowHelper
{
	const FName ActorPinName = "Actor";
	const FName DataTablePinName = "DataTable";
	const FName RowNamePinName = "RowName";
	const FName ActionNamePinName = "ActionName";
	const FName ActionNameOutPinName = "ActionNameOut";
}

UK2Node_SetEnemyAction::UK2Node_SetEnemyAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Attempts to retrieve a TableRow from a DataTable via it's RowName");
}

void UK2Node_SetEnemyAction::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add execution pins
//CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
//UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
//RowFoundPin->PinFriendlyName = LOCTEXT("GetDataTableRow Row Found Exec pin", "Row Found");

	// Add DataTable pin
	UEdGraphPin* DataTablePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), GetDataTableRowHelper::DataTablePinName);
	SetPinToolTip(*DataTablePin, LOCTEXT("DataTablePinDescription", "The DataTable you want to retreive a row from"));

	// Row Name pin
	UEdGraphPin* RowNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, GetDataTableRowHelper::RowNamePinName);
	SetPinToolTip(*RowNamePin, LOCTEXT("RowNamePinDescription", "The name of the row to retrieve from the DataTable"));

	// Action Name Pin
	UEdGraphPin* ActionNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, GetDataTableRowHelper::ActionNamePinName);
	SetPinToolTip(*ActionNamePin, LOCTEXT("ActionNamePinDescription", "The name of the action to tretrieve from the DataTable"));

	UEdGraphPin* ActionNameOutPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Name, GetDataTableRowHelper::ActionNameOutPinName);
	SetPinToolTip(*ActionNameOutPin, LOCTEXT("ActionNameOutPinDescription", "The name of the action to retrieve from the DataTable"));


	Super::AllocateDefaultPins();
}

void UK2Node_SetEnemyAction::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
{
	MutatablePin.PinToolTip = UEdGraphSchema_K2::TypeToText(MutatablePin.PinType).ToString();

	UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	if (K2Schema != nullptr)
	{
		MutatablePin.PinToolTip += TEXT(" ");
		MutatablePin.PinToolTip += K2Schema->GetPinDisplayName(&MutatablePin).ToString();
	}

	MutatablePin.PinToolTip += FString(TEXT("\n")) + PinDescription.ToString();
}

void UK2Node_SetEnemyAction::RefreshRowNameOptions()
{
	// When the DataTable pin gets a new value assigned, we need to update the Slate UI so that SGraphNodeCallParameterCollectionFunction will update the ParameterName drop down
	UEdGraph* Graph = GetGraph();
	Graph->NotifyGraphChanged();
}

void UK2Node_SetEnemyAction::OnDataTableRowListChanged(const UDataTable* DataTable)
{
	UEdGraphPin* DataTablePin = GetDataTablePin();
	if (DataTable && DataTablePin && DataTable == DataTablePin->DefaultObject)
	{
		UEdGraphPin* RowNamePin = GetRowNamePin();
		const bool TryRefresh = RowNamePin && !RowNamePin->LinkedTo.Num();
		const FName CurrentName = RowNamePin ? FName(*RowNamePin->GetDefaultAsString()) : NAME_None;
		if (TryRefresh && RowNamePin && !DataTable->GetRowNames().Contains(CurrentName))
		{
			if (UBlueprint* BP = GetBlueprint())
			{
				FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
			}
		}
	}
}

void UK2Node_SetEnemyAction::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	if (UEdGraphPin* DataTablePin = GetDataTablePin(&OldPins))
	{
		if (UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
		{
			// make sure to properly load the data-table object so that we can 
			// farm the "RowStruct" property from it (below, in GetDataTableRowStructType)
			PreloadObject(DataTable);
		}
	}
}

void UK2Node_SetEnemyAction::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_SetEnemyAction::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Utilities);
}

void UK2Node_SetEnemyAction::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	if(!ChangedPin)
		return;

	if (ChangedPin->PinName == GetDataTableRowHelper::DataTablePinName)
	{
		UEdGraphPin* RowNamePin = GetRowNamePin();
		UDataTable* DataTable = Cast<UDataTable>(ChangedPin->DefaultObject);
		if (RowNamePin)
		{
			if (DataTable && (RowNamePin->DefaultValue.IsEmpty() || !DataTable->GetRowMap().Contains(*RowNamePin->DefaultValue)))
			{
				if (auto Iterator = DataTable->GetRowMap().CreateConstIterator())
				{
					RowNamePin->DefaultValue = Iterator.Key().ToString();
				}
			}

			RefreshRowNameOptions();
		}
	}
	else if (ChangedPin->PinName == GetDataTableRowHelper::ActionNamePinName)
	{
		const UEdGraphSchema* Schema = GetActionNameOutPin()->GetSchema();
		Schema->TrySetDefaultValue(*GetActionNameOutPin(), GetActionNamePin()->GetDefaultAsString());

		UE_LOG(LogKimhanil,Warning, TEXT("ActionNamePinName Changed : %s"), *GetActionNameOutPin()->GetDefaultAsString());
	}
}

FText UK2Node_SetEnemyAction::GetTooltipText() const
{
	return NodeTooltip;
}

UEdGraphPin* UK2Node_SetEnemyAction::GetThenPin()const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_SetEnemyAction::GetDataTablePin(const TArray<UEdGraphPin*>* InPinsToSearch /*= NULL*/) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == GetDataTableRowHelper::DataTablePinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SetEnemyAction::GetRowNamePin() const
{
	UEdGraphPin* Pin = FindPinChecked(GetDataTableRowHelper::RowNamePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SetEnemyAction::GetActionNamePin() const
{
	UEdGraphPin* Pin = FindPinChecked(GetDataTableRowHelper::ActionNamePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SetEnemyAction::GetActionNameOutPin() const
{
	UEdGraphPin* Pin = FindPinChecked(GetDataTableRowHelper::ActionNameOutPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

FText UK2Node_SetEnemyAction::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Set Enemy Action");
	}
	else if (UEdGraphPin* DataTablePin = GetDataTablePin())
	{
		if (DataTablePin->LinkedTo.Num() > 0)
		{
			return NSLOCTEXT("K2Node", "DataTable_Title_Unknown", "Set Enemy Action");
		}
		else if (DataTablePin->DefaultObject == nullptr)
		{
			return NSLOCTEXT("K2Node", "DataTable_Title_None", "Set Enemy(NONE) Action");
		}
		else if (CachedNodeTitle.IsOutOfDate(this))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("DataTableName"), FText::FromString(GetActionNameOutPin()->DefaultValue));

			FText LocFormat = NSLOCTEXT("K2Node", "DataTable", "Set Enemy({DataTableName}) Action");
			// FText::Format() is slow, so we cache this to save on performance
			CachedNodeTitle.SetCachedText(FText::Format(LocFormat, Args), this);
		}
	}
	else
	{
		return NSLOCTEXT("K2Node", "DataTable_Title_None", "Set Enemy(NONE) Action");
	}
	return CachedNodeTitle;
}

void UK2Node_SetEnemyAction::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
	UDataTable* Table = (OriginalDataTableInPin != NULL) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : NULL;
	if ((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("GetDataTableRowNoDataTable_Error", "GetDataTableRow must have a DataTable specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	//UK2Node_CallFunction* SetActionPin = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	//if (SetActionPin)
	//{
	//	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(IEnemyInterface, SetAction);
	//	SetActionPin->FunctionReference.SetExternalMember(FunctionName, UEnemyInterface::StaticClass());
	//	SetActionPin->AllocateDefaultPins();
	//	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(SetActionPin->GetExecPin()));
	//	UEdGraphPin* SetActionInputPin = SetActionPin->FindPinChecked(TEXT("ActionName"));
	//	SetActionInputPin->DefaultValue = GetActionNamePin()->DefaultValue;
	//}
	//	
	
	CompilerContext.MovePinLinksToIntermediate(*GetActionNamePin(), *GetActionNameOutPin());

	
	BreakAllNodeLinks();
}

FSlateIcon UK2Node_SetEnemyAction::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_SetEnemyAction::EarlyValidation(class FCompilerResultsLog& MessageLog) const
{
	Super::EarlyValidation(MessageLog);

	const UEdGraphPin* DataTablePin = GetDataTablePin();
	const UEdGraphPin* RowNamePin = GetRowNamePin();
	if (!DataTablePin || !RowNamePin)
	{
		MessageLog.Error(*LOCTEXT("MissingPins", "Missing pins in @@").ToString(), this);
		return;
	}

	if (DataTablePin->LinkedTo.Num() == 0)
	{
		const UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject);
		if (!DataTable)
		{
			MessageLog.Error(*LOCTEXT("NoDataTable", "No DataTable in @@").ToString(), this);
			return;
		}

		if (!RowNamePin->LinkedTo.Num())
		{
			const FName CurrentName = FName(*RowNamePin->GetDefaultAsString());
			if (!DataTable->GetRowNames().Contains(CurrentName))
			{
				const FString Msg = FText::Format(
					LOCTEXT("WrongRowNameFmt", "'{0}' row name is not stored in '{1}'. @@"),
					FText::FromString(CurrentName.ToString()),
					FText::FromString(GetFullNameSafe(DataTable))
				).ToString();
				MessageLog.Error(*Msg, this);
				return;
			}
		}
	}
}

void UK2Node_SetEnemyAction::PreloadRequiredAssets()
{
	if (UEdGraphPin* DataTablePin = GetDataTablePin())
	{
		if (UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
		{
			// make sure to properly load the data-table object so that we can 
			// farm the "RowStruct" property from it (below, in GetDataTableRowStructType)
			PreloadObject(DataTable);
		}
	}
	return Super::PreloadRequiredAssets();
}

void UK2Node_SetEnemyAction::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (Pin == GetDataTablePin())
	{
		const bool bConnectionAdded = Pin->LinkedTo.Num() > 0;
		if (bConnectionAdded)
		{
			// if a connection was made, then we may need to rid ourselves of the row dropdown
			RefreshRowNameOptions();
		}
	}
}

#undef LOCTEXT_NAMESPACE
