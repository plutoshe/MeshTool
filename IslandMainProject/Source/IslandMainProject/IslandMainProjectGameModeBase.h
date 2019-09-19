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

#pragma region BuildingSystem
public:
	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		FORCEINLINE bool InBuildingMode() const { return m_inBuildingMode; }

	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		void SwitchToMode(EModeEnum mode);

	UPROPERTY(VisibleAnywhere, Category = "BuilidingSystem")
		EModeEnum m_CurrentMode;

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
