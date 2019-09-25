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
#pragma region BuildingSystem
public:
	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		FORCEINLINE bool InBuildingMode() const { return m_inBuildingMode; }

	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		void SwitchToMode(EModeEnum mode);

	UPROPERTY(VisibleAnywhere, Category = "BuilidingSystem")
		EModeEnum m_CurrentMode;

	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		bool CheckIfResourceEnough(FName sourceID);
	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		TArray<int> GetAmountsOfRequiredResource(FName sourceID);
	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		bool CostResources(FName sourceID);
protected:
	bool m_inBuildingMode;

	bool FindBuildingManager();

	UPROPERTY(EditDefaultsOnly, Category = "BuilidingSystem")
		TSubclassOf<class AVenturePawn> m_venturePawnClass;
	/* ----Useful HEAD
	UPROPERTY(EditDefaultsOnly, Category = "BuilidingSystem")
		TSubclassOf<class ABuildingSystemPawn> m_buildingPawnClass;
		*/

	class AVenturePawn* m_cachedVenturePawn;
	/* ----Useful HEAD
	class ABuildingSynchronization* m_buildingManager;
	class ABuildingSystemPawn* m_cachedBuildingPawn;
*/
	virtual void BeginPlay() override;

#pragma endregion

};
