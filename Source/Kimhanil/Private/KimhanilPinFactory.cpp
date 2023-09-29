// Fill out your copyright notice in the Description page of Project Settings.


#include "KimhanilPinFactory.h"

#include "Engine/DataTable.h"
#include "SGraphPinDataTableRowName.h"
#include "EnemyType.h"
#include "K2Node_SetEnemyAction.h"

TSharedPtr<class SGraphPin> FKimhanilPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
	{
		UObject* Outer = InPin->GetOuter();

		const UEdGraphPin* DataTablePin = nullptr;
		const UEdGraphPin* RowNamePin = nullptr;
		if (Outer->IsA(UK2Node_SetEnemyAction::StaticClass()))
		{
			const UK2Node_SetEnemyAction* SetEnemyActionNode = CastChecked<UK2Node_SetEnemyAction>(Outer);
			DataTablePin = SetEnemyActionNode->GetDataTablePin();
			RowNamePin	 = SetEnemyActionNode->GetRowNamePin();
		}

		UDataTable* DataTable = nullptr;
		if (DataTablePin)
		{
			if (DataTablePin->DefaultObject != nullptr && DataTablePin->LinkedTo.Num() == 0)
			{
				DataTable = Cast<UDataTable>(DataTablePin->DefaultObject);
			}
		}

		if (DataTable)
		{
			if (InPin->PinName == TEXT("RowName"))
			{
				return SNew(SGraphPinDataTableRowName, InPin, DataTable);
			}
			else if(InPin->PinName == TEXT("ActionName"))
			{
				FEnemyType* RowData = DataTable->FindRow<FEnemyType>(FName(*RowNamePin->GetDefaultAsString()), TEXT("EnemyAction"));
				if (RowData != nullptr)
				{
					TArray<TSharedPtr<FName>> NameList;
					for (FName Iter : RowData->Actions)
					{
						TSharedPtr<FName> ActionName = MakeShareable(new FName(Iter));
						NameList.Add(ActionName);
					}

					return SNew(SGraphPinNameList, InPin, NameList);
				}
			}
		}

		return nullptr;
	}

	
	return nullptr;
}
