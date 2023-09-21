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

// 각종 핀들에 대한 할당
// 노드가 생성될때, 노드가 그래프에 배치될때마다 호출된다. 때때로 그래프가 열릴때도 호출됨
// UPROPERTY는 AllocateDefaultPins가 호출되기전에 Serialized 된다고함, (UPROPERTY는 일종의 설정 같은 느낌)
// Pin들은 이 함수를 통해 항상 Fresh하게 유지됨
// Personally : 굳이 이름에 DefaultPins를 넣은 이유는 임의의 핀을 재 할당 가능할수도 있을 것 같다
void UK2Node_GetEnemyAction::AllocateDefaultPins()
{
	//const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	//핀을 만드는 함수 CreatePin()

	//실행핀
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* ActionFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	//에디터에서 보여지는 핀의 이름, 기본값은 PinName의 값으로 되어있다.
	ActionFoundPin->PinFriendlyName = LOCTEXT("GetEnemyAction Action Found Exec pin", "Action Found");
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetEnemyActionHelper::ActionNotFoundPinName);

	//액션 데이터테이블 핀
	// Pin Category가 object면 어떤 object인지 알려주는 인자를 넣어줘야 한다.
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

// 노드의 이름을 설정하는 부분
// 
FText UK2Node_GetEnemyAction::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	// MenuAction에 노출되는 이름. // Q : MenuTitle은 한번만 호출되는가?, GetMenuActions에서 NodeSpawner가 BlueprintActionDatabase에 등록될때로 예상
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		//로컬라이징되는 텍스트를 생성한다. LOCTEXT 매크로의 기능, LOCTEXT_NAMESPACE로 만들어진 로컬라이징 네임스페이스로 영역 설정됨
		return LOCTEXT("ListViewTitle", "Get Enemy Action");
	}
	else
	{
		// 세팅된 Enemy에 따라 이름이 바뀌게 만들기
	}
	
	// NSLOCTEXT는 LOCTEXT_NAMESPACE의 네임스페이스를 따르지 않고, 첫번째 인수로 별도로 네임스페이스를 설정한다.
	return LOCTEXT("Enemy","Get Enemy Action");
}

// guess : k2node의 nodespawner를 상위 오브젝트에 전달해, 블루프린트 액션리스트에 올린뒤 사용하는 듯.
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


// 노드의 확장
// 노드의 각종 기능을 넣는 곳인듯
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

	// 왜 Node로 CallFunction기능을 가져와, DataTableFunctionLibrary를 실행시켰는가.
	// 그냥 DataTableFunctionLibrary를 실행시키면 되는것 아닌가?
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

	// function node 핀들과, 이 핀의 리턴 핀 가져오기
	UEdGraphPin* OriginalOutActionPin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionOutRowPin = GetDataTableRowFunction->FindPinChecked(TEXT("OutRow"));
	UEdGraphPin* FunctionReturnPin = GetDataTableRowFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionThenPin = GetDataTableRowFunction->GetThenPin();

	// OutRow핀의 타입을 오리지널과 맞추기
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