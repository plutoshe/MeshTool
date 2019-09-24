// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Public/StaticLibrary.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateHUD);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
		TArray<FInventoryItem> m_inventory;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool AddItem(FName itemID, int amount);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int ReduceItemAmount(FName itemID, int amount);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RemoveItem(FName itemID);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ThrowItem(FName itemID, int amount);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void TransformItemTo(UInventoryComponent* Other);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ClearAllItems();
#pragma region Query
	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		bool HasItem(FName itemID);
	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		bool HasEnoughItem(const FCostable& costable);
	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		int GetItemAmount(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		FORCEINLINE TArray<FInventoryItem>& GetInventory() { return m_inventory; }

	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		FORCEINLINE int GetNumOfSlots() { return m_slotCount; }
#pragma endregion


	void LoadInventoryData();

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnUpdateHUD"))
		FOnUpdateHUD OnUpdateHUD;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		uint16 m_slotCount;

	void InitlizeInventory();

private:

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
