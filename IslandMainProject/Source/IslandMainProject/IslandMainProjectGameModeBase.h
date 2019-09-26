// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IslandMainProjectGameModeBase.generated.h"

UENUM(BlueprintType)
enum class EModeEnum : uint8
{
	ME_Exploration 	UMETA(DisplayName = "Exploration"),
	ME_Building 	UMETA(DisplayName = "Building")
};

UCLASS(minimalapi)
class  AIslandMainProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AIslandMainProjectGameModeBase();

	class UDataTable* GetItemDB() const { return m_InventoryItemDB; }
	class UDataTable* GetBuildingResourceDB() const { return m_buildingResourceDB; }
	class UDataTable* GetToolDB() const { return m_toolDB; }
	/* ----Useful HEAD
	TArray<TSubclassOf<class AAPToolBase>> GetSpawnToolList() const { return m_ToolClassesToSpawn; }
	*/


protected:
	/* ----Useful HEAD
	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
		TArray<TSubclassOf<class AAPToolBase>> m_ToolClassesToSpawn;
		*/
	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
		class UDataTable* m_InventoryItemDB;
	UPROPERTY(EditDefaultsOnly, Category = "BuildingSystem")
		class UDataTable* m_buildingResourceDB;
	UPROPERTY(EditDefaultsOnly, Category = "Tool")
		class UDataTable* m_toolDB;
	

public:
	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
	FORCEINLINE bool InBuildingMode() const { return m_inBuildingMode; }

	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
	void GoToExploreMode();

	UFUNCTION(BlueprintCallable, Category = "BuildingSystem")
	void GoToBuildingMode(class ABuilding* targetBuilding);

	UPROPERTY(VisibleAnywhere, Category = "Game Mode State")
	EModeEnum m_CurrentMode;
		
	UFUNCTION(BlueprintCallable, Category = "Building System")
	TArray<int> GetAmountsOfRequiredResource(FName sourceID);
	
	UFUNCTION(BlueprintCallable, Category = "Building System")
	bool CostResources(FName sourceID);

	UFUNCTION(BlueprintCallable, Category = "Builiding System")
	bool CheckIfResourceEnough(FName sourceID);

	UFUNCTION(BlueprintCallable, Category = "Building System")
	class UMaterialInterface* GetBuildPlaceableMaterial();

	UFUNCTION(BlueprintCallable, Category = "Building System")
	class UMaterialInterface* GetBuildNotPlaceableMaterial();

protected:
	bool m_inBuildingMode;
	
	UPROPERTY(EditDefaultsOnly, Category = "Exploring System")
	TSubclassOf<class AVenturePawn> m_venturePawnClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Building System")
	TSubclassOf<class ABuildingPawn> m_buildingPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Building System")
	class UMaterialInterface* m_materialBuildPlaceable;
	
	UPROPERTY(EditDefaultsOnly, Category = "Building System")
	class UMaterialInterface* m_materialBuildNotPlaceable;
	
	class AVenturePawn* m_cachedVenturePawn;
	class ABuildingPawn* m_cachedBuildingPawn;

	virtual void BeginPlay() override;


};
