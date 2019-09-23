// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"

#include "StaticLibrary.generated.h"


USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase {

	GENERATED_USTRUCT_BODY()

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
USTRUCT(BlueprintType)
struct FCostPair{
	GENERATED_USTRUCT_BODY()
public:
	FCostPair() {
		ItemID = FName("None");
		Amount = 0;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Amount;
};

USTRUCT(BlueprintType)
struct FCostable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FCostable() {
		SourceID = FName("None");
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName SourceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCostPair> RequireResourceList;
};


/**
 * A class to hold static my functions
 */
UCLASS()
class ISLANDMAINPROJECT_API UStaticLibrary : public UObject
{
	GENERATED_BODY()
	
		// Add more static functions here later on

};
