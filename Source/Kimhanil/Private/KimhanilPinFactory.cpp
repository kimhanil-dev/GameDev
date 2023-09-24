// Fill out your copyright notice in the Description page of Project Settings.


#include "KimhanilPinFactory.h"

#include "Engine/DataTable.h"
#include "SGraphPinDataTableRowName.h"
#include "K2Node_SetEnemyAction.h"

TSharedPtr<class SGraphPin> FKimhanilPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
	{
		UObject* Outer = InPin->GetOuter();

		const UEdGraphPin* DataTablePin = nullptr;
		if (Outer->IsA(UK2Node_SetEnemyAction::StaticClass()))
		{
			const UK2Node_SetEnemyAction* SetEnemyActionNode = CastChecked<UK2Node_SetEnemyAction>(Outer);
			DataTablePin = SetEnemyActionNode->GetDataTablePin();
		}

		if (DataTablePin)
		{
			if (DataTablePin->DefaultObject != nullptr && DataTablePin->LinkedTo.Num() == 0)
			{
				if (auto DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
				{
					return SNew(SGraphPinDataTableRowName, InPin, DataTable);
				}
				if (DataTablePin->DefaultObject->IsA(UCurveTable::StaticClass()))
				{
					UCurveTable* CurveTable = (UCurveTable*)DataTablePin->DefaultObject;
					if (CurveTable)
					{
						TArray<TSharedPtr<FName>> RowNames;
						/** Extract all the row names from the RowMap */
						for (TMap<FName, FRealCurve*>::TConstIterator Iterator(CurveTable->GetRowMap()); Iterator; ++Iterator)
						{
							/** Create a simple array of the row names */
							TSharedPtr<FName> RowNameItem = MakeShareable(new FName(Iterator.Key()));
							RowNames.Add(RowNameItem);
						}
						return SNew(SGraphPinNameList, InPin, RowNames);
					}
				}
			}
		}
	}

	
	return nullptr;
}
