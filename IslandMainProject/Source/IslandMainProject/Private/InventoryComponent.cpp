// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/InventoryComponent.h"
#include "IslandMainProjectGameModeBase.h"
#include "Engine/World.h"
#include "Public/PickUpBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_slotCount = 10;
	InitlizeInventory();
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::InitlizeInventory()
{
	// Initialize a certain size of list
	LoadInventoryData();

}


bool UInventoryComponent::AddItem(FName itemID, int amount)
{
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		// Check if it is existing item
		if ((*it).ItemID.IsEqual(itemID) && (*it).bCanStack) {
			(*it).Value += amount;
			UE_LOG(LogTemp, Log, TEXT("Existing Item: %s, Amount: %d"), *itemID.ToString(), (*it).Value);

			// Update the HUD in BP
			OnUpdateHUD.Broadcast();

			return true;
		}
	}

	// if it is over capacity return false
	if (m_inventory.Num() >= m_slotCount) return false;

	AIslandMainProjectGameModeBase* GM = Cast <AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GM) {
		UDataTable* table = GM->GetItemDB();
		if (table) {
			FInventoryItem* ItemToAdd = table->FindRow<FInventoryItem>(itemID, "");
			if (ItemToAdd) {
				FInventoryItem newItem(*ItemToAdd);
				newItem.Value = amount;
				UE_LOG(LogTemp, Log, TEXT("New Item: %s, Amount: %d"), *newItem.ItemID.ToString(), amount);
				bool bNoneBefore;
				for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
				{
					if ((*it).ItemID.IsEqual(FName("None"))) {
						bNoneBefore = true;
						(*it) = newItem;
						break;
					}
				}
				if (!bNoneBefore)
					m_inventory.Add(newItem);
				OnUpdateHUD.Broadcast();
				return true;
			}
		}
		return false;
	}
	return false;
}

int UInventoryComponent::ReduceItemAmount(FName itemID, int amount)
{
	UE_LOG(LogTemp, Log, TEXT("Reduce %s Amount: %d"), *itemID.ToString(), amount);

	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		// Check if it is existing item
		if ((*it).ItemID.IsEqual(itemID)) {
			int beforeCount = (*it).Value;
			(*it).Value -= amount;
			if ((*it).Value <= 0) {
				RemoveItem(itemID);
				return beforeCount;
			}
			// Update the HUD in BP
			OnUpdateHUD.Broadcast();
			return amount;
		}
	}
	return 0;
}

void UInventoryComponent::RemoveItem(FName itemID)
{
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		if ((*it).ItemID.IsEqual(itemID)) {
			(*it).ClearItem();
			break;
		}
	}
	OnUpdateHUD.Broadcast();
}

void UInventoryComponent::ThrowItem(FName itemID, int amount)
{
	if (!itemID.IsEqual(TEXT("None"))) {
		int actualRecudeAmount = amount;
		if (actualRecudeAmount != 0) {
			FInventoryItem reducedItem;
			for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
			{
				if ((*it).ItemID.IsEqual(itemID)) {
					reducedItem = (*it);
				}
			}

			TSubclassOf<APickUpBase> spawnedClass = reducedItem.ItemPickUp;
			if (spawnedClass) {
				FActorSpawnParameters spawnPara;
				spawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				APickUpBase* spawnActor = GetWorld()->SpawnActor<APickUpBase>(spawnedClass, GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 30.f, FRotator::ZeroRotator, spawnPara);

				spawnActor->AddImpulseToOverlapComp((GetOwner()->GetActorForwardVector() + GetOwner()->GetActorUpVector()).GetSafeNormal(), 500.f);
				spawnActor->Amount = ReduceItemAmount(itemID, amount);
			
			}
	
		}
	}

}

void UInventoryComponent::ClearAllItems()
{

}



void UInventoryComponent::TransformItemTo(UInventoryComponent* Other)
{

}


void UInventoryComponent::LoadInventoryData()
{
	//... for future loading data
}

bool UInventoryComponent::HasItem(FName itemID)
{
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		if ((*it).ItemID.IsEqual(itemID)) {
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::HasEnoughItem(FName itemID, int amount)
{
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		if ((*it).ItemID.IsEqual(itemID) && (*it).Value >= amount) {
			return true;
		}
	}
	return false;
}
