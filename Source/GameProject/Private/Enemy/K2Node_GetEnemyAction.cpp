// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/K2Node_GetEnemyAction.h"

#include "Kismet/DataTableFunctionLibrary.h"
#include "KismetCompiler.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "Engine/DataTable.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"

#define LOCTEXT_NAMESPACE "K2Node_GetEnemyAction"

UINT UK2Node_GetEnemyAction::CountOfNode = 0;
namespace GetEnemyActionHelper
{
	const FName EnemyDataTablePinName	= "DataTable";
	const FName ActionNotFoundPinName	= "ActionNotFound";
	const FName EnemyNamePinName		= "EnemyName";
	const FName ActionNamePinName		= "ActionName";
}


UK2Node_GetEnemyAction::UK2Node_GetEnemyAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CountOfNode++;
	UE_LOG(LogConfig,Warning,TEXT("%d Node created"),CountOfNode);
}

// ���� �ɵ鿡 ���� �Ҵ�
// ��尡 �����ɶ�, ��尡 �׷����� ��ġ�ɶ����� ȣ��ȴ�. ������ �׷����� �������� ȣ���
// UPROPERTY�� AllocateDefaultPins�� ȣ��Ǳ����� Serialized �ȴٰ���, (UPROPERTY�� ������ ���� ���� ����)
// Pin���� �� �Լ��� ���� �׻� Fresh�ϰ� ������
// Personally : ���� �̸��� DefaultPins�� ���� ������ ������ ���� �� �Ҵ� �����Ҽ��� ���� �� ����
void UK2Node_GetEnemyAction::AllocateDefaultPins()
{
	//const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	//���� ����� �Լ� CreatePin()

	//������
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* ActionFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	//�����Ϳ��� �������� ���� �̸�, �⺻���� PinName�� ������ �Ǿ��ִ�.
	ActionFoundPin->PinFriendlyName = LOCTEXT("GetEnemyAction Action Found Exec pin", "Action Found");
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetEnemyActionHelper::ActionNotFoundPinName);

	//�׼� ���������̺� ��
	// Pin Category�� object�� � object���� �˷��ִ� ���ڸ� �־���� �Ѵ�.
	UEdGraphPin* DataTablePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(),GetEnemyActionHelper::EnemyDataTablePinName);
	SetPinToolTip(*DataTablePin, LOCTEXT("DataTablePinDescription", "The Enemy_DataTable you want to retreive a action from"));

	//Enemy Name Pin
	UEdGraphPin* EnemyNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, GetEnemyActionHelper::EnemyNamePinName);
	SetPinToolTip(*DataTablePin, LOCTEXT("EnemyNamePinDescription", "The name of the enemy to retrieve from the Enemy_DataTable"));

	// Action Name Pin
	UEdGraphPin* ActionNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, GetEnemyActionHelper::ActionNamePinName);
	SetPinToolTip(*DataTablePin, LOCTEXT("ActionNamePinDescription", "The name of action of the enemy to retrieve from the Enemy_DataTable"));

	Super::AllocateDefaultPins();
}

void UK2Node_GetEnemyAction::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
{
	MutatablePin.PinToolTip  = UEdGraphSchema_K2::TypeToText(MutatablePin.PinType).ToString();

	UEdGraphSchema_K2 const * const K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	if (K2Schema != nullptr)
	{
		MutatablePin.PinToolTip += TEXT(" ");
		MutatablePin.PinToolTip += K2Schema->GetPinDisplayName(&MutatablePin).ToString();
	}

	MutatablePin.PinToolTip += FString(TEXT("\n")) + PinDescription.ToString();
}

// ����� �̸��� �����ϴ� �κ�
// 
FText UK2Node_GetEnemyAction::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	// MenuAction�� ����Ǵ� �̸�. // Q : MenuTitle�� �ѹ��� ȣ��Ǵ°�?, GetMenuActions���� NodeSpawner�� BlueprintActionDatabase�� ��ϵɶ��� ����
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		//���ö���¡�Ǵ� �ؽ�Ʈ�� �����Ѵ�. LOCTEXT ��ũ���� ���, LOCTEXT_NAMESPACE�� ������� ���ö���¡ ���ӽ����̽��� ���� ������
		return LOCTEXT("ListViewTitle", "Get Enemy Action");
	}
	else
	{
		// ���õ� Enemy�� ���� �̸��� �ٲ�� �����
	}
	
	// NSLOCTEXT�� LOCTEXT_NAMESPACE�� ���ӽ����̽��� ������ �ʰ�, ù��° �μ��� ������ ���ӽ����̽��� �����Ѵ�.
	return LOCTEXT("Enemy","Get Enemy Action");
}

// guess : k2node�� nodespawner�� ���� ������Ʈ�� ������, �������Ʈ �׼Ǹ���Ʈ�� �ø��� ����ϴ� ��.
void UK2Node_GetEnemyAction::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(NodeSpawner);
	}
}

FText UK2Node_GetEnemyAction::GetMenuCategory() const
{
	return LOCTEXT("Category","Enemy");
}

UEdGraphPin* UK2Node_GetEnemyAction::GetThenPin() const
{
	//const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_GetEnemyAction::GetDataTablePin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == GetEnemyActionHelper::EnemyDataTablePinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_GetEnemyAction::GetEnemyNamePin() const
{
	//const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(GetEnemyActionHelper::EnemyNamePinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_GetEnemyAction::GetActionNamePin() const
{
	//const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(GetEnemyActionHelper::ActionNamePinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_GetEnemyAction::GetActionNotFoundPin() const
{
	//const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(GetEnemyActionHelper::ActionNotFoundPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_GetEnemyAction::GetResultPin() const
{
	//const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}


FText UK2Node_GetEnemyAction::GetTooltipText() const
{
	return LOCTEXT("ToolTip","Test tooltip");
}


// ����� Ȯ��
// ����� ���� ����� �ִ� ���ε�
void UK2Node_GetEnemyAction::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
	UDataTable* Table = (OriginalDataTableInPin != NULL) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : NULL;
	if ((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num()) && nullptr == Table)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("GetEnemyActionDataTable_Error", "GetEnemyAction must have a DataTable specified.").ToString(), this);
		BreakAllNodeLinks();
		return;
	}

	// �� Node�� CallFunction����� ������, DataTableFunctionLibrary�� ������״°�.
	// �׳� DataTableFunctionLibrary�� �����Ű�� �Ǵ°� �ƴѰ�?
	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UDataTableFunctionLibrary, GetDataTableRowFromName);
	UK2Node_CallFunction* GetDataTableRowFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	GetDataTableRowFunction->FunctionReference.SetExternalMember(FunctionName, UDataTableFunctionLibrary::StaticClass());
	GetDataTableRowFunction->AllocateDefaultPins();
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(GetDataTableRowFunction->GetExecPin()));

	// Connect the input of our GetEnemyAction to the Input of our Function pin
	UEdGraphPin* DataTableInPin = GetDataTableRowFunction->FindPinChecked(TEXT("Table"));
	if (OriginalDataTableInPin->LinkedTo.Num() > 0)
	{
		// Copy the connection
		CompilerContext.MovePinLinksToIntermediate(*OriginalDataTableInPin, *DataTableInPin);
	}
	else
	{
		// Copy literal
		DataTableInPin->DefaultObject = OriginalDataTableInPin->DefaultObject;
	}
	UEdGraphPin* RowNameInPin = GetDataTableRowFunction->FindPinChecked(TEXT("RowName"));
	CompilerContext.MovePinLinksToIntermediate(*GetEnemyNamePin(), *RowNameInPin);

	// function node �ɵ��, �� ���� ���� �� ��������
	UEdGraphPin* OriginalOutActionPin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionOutRowPin = GetDataTableRowFunction->FindPinChecked(TEXT("OutRow"));
	UEdGraphPin* FunctionReturnPin = GetDataTableRowFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionThenPin = GetDataTableRowFunction->GetThenPin();

	// OutRow���� Ÿ���� �������ΰ� ���߱�
	FunctionOutRowPin->PinType = OriginalOutActionPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutActionPin->PinType.PinSubCategoryObject;

	//BRANCH NODE
	UK2Node_IfThenElse* BranchNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	BranchNode->AllocateDefaultPins();
	// Hook up inputs to branch
	FunctionThenPin->MakeLinkTo(BranchNode->GetExecPin());
	FunctionReturnPin->MakeLinkTo(BranchNode->GetConditionPin());

	CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *(BranchNode->GetThenPin()));
	CompilerContext.MovePinLinksToIntermediate(*GetActionNotFoundPin(), *(BranchNode->GetElsePin()));
	CompilerContext.MovePinLinksToIntermediate(*OriginalOutActionPin, *FunctionOutRowPin);

	BreakAllNodeLinks()
}

#undef LOCTEXT_NAMESPACE