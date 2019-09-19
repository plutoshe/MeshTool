// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase {

	GENERATED_BODY()

public:
	FInventoryItem() {
		Name = FText::FromString("Item");
		Action = FText::FromString("Use");
		Describtion = FText::FromString("Add Description");
		Value = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class APickUpBase> ItemPickUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32  Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* Thumbnail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Describtion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanBeUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanStack;

	void ClearItem() {
		ItemID = FName(TEXT("None"));
		Name = FText::FromString("Item");
		Action = FText::FromString("Use");
		Describtion = FText::FromString("Add Description");
		Thumbnail = nullptr;
		Value = 0;
	}
	bool IsNone() { return ItemID.IsEqual(FName(TEXT("None"))); }

	bool operator==(const FInventoryItem& other) const {
		return ItemID == other.ItemID;
	}
};

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

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool HasItem(FName itemID);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool HasEnoughItem(FName itemID, int amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FORCEINLINE TArray<FInventoryItem>& GetInventory() { return m_inventory; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FORCEINLINE int GetNumOfSlots() { return m_slotCount; }
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
