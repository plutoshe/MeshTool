// Fill out your copyright notice in the Description page of Project Settings.


#include "IslandMainProjectGameModeBase.h"
#include "Public/VenturePawn.h"
#include "Public/CameraControlComponent.h"
#include "Public/InventoryComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Public/Building/Pawns/BuildingPawn.h"
#include "Public/Building/Actors/Building.h"

	/* ----Useful HEAD
#include "AncientWorldPlayerController.h"

#include "BuildingSynchronization.h"
*/
AIslandMainProjectGameModeBase::AIslandMainProjectGameModeBase()
{
	m_inBuildingMode = false;
}

void AIslandMainProjectGameModeBase::GoToExploreMode()
{
	if (this->m_CurrentMode != EModeEnum::ME_Exploration) {
		// Switch to exploration mode
		this->m_CurrentMode = EModeEnum::ME_Exploration;
		
		if (this->m_cachedVenturePawn == nullptr) {
			this->m_cachedVenturePawn = GetWorld()->SpawnActor<AVenturePawn>(m_venturePawnClass);
		}

		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		controller->SetViewTargetWithBlend(this->m_cachedVenturePawn, 2.0f, EViewTargetBlendFunction::VTBlend_Linear, 0, false);
		controller->UnPossess();
		//controller->SetViewTargetWithBlend(this->m_cachedVenturePawn, 2.0f, EViewTargetBlendFunction::VTBlend_Linear, 0, false);
		controller->Possess(this->m_cachedVenturePawn);

		UE_LOG(LogTemp, Log, TEXT("Go to Exploration Mode"));
	}
}

void AIslandMainProjectGameModeBase::GoToBuildingMode(ABuilding * targetBuilding)
{
	if (this->m_CurrentMode != EModeEnum::ME_Building)
	{

		if (!targetBuilding)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not switch to Building Mode. Building Target Null"));
			return;
		}

		if (this->m_cachedBuildingPawn == nullptr)
		{
			FVector location = targetBuilding->GetActorLocation();
			FActorSpawnParameters spawnInfo;
			this->m_cachedBuildingPawn = GetWorld()->SpawnActor<ABuildingPawn>(m_buildingPawnClass, location, FRotator::ZeroRotator, spawnInfo);
		}

		if (!this->m_cachedBuildingPawn)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not switch to Building Mode. Building Pawn NULL"));
			return;
		}

		if (this->m_cachedBuildingPawn && targetBuilding)
		{
			APlayerController* controller = GetWorld()->GetFirstPlayerController();
			controller->UnPossess();
			//controller->SetViewTargetWithBlend(this->m_cachedBuildingPawn, 2.0f, EViewTargetBlendFunction::VTBlend_Linear, 0, false);
			controller->Possess(this->m_cachedBuildingPawn);

			this->m_cachedBuildingPawn->SetTargetBuildingAndInitialize(targetBuilding);

			//m_cachedBuildingPawn->m_BuildingSystem = m_buildingManager;
			this->m_cachedBuildingPawn->SetActorLocation(targetBuilding->GetActorLocation());

			// Switch to building mode
			this->m_CurrentMode = EModeEnum::ME_Building;
			UE_LOG(LogTemp, Log, TEXT("Go to Building Mode"));
		}
	}
}

bool AIslandMainProjectGameModeBase::CheckIfResourceEnough(FName sourceID)
{
	if (!m_cachedVenturePawn) return false;
	
	UInventoryComponent* playerInventoryComp = m_cachedVenturePawn->GetInventoryComponent();
	// Find the source ID
	FCostable* thisCostable = m_buildingResourceDB->FindRow<FCostable>(sourceID, "");
	if(thisCostable)
	return playerInventoryComp->HasEnoughItem(*thisCostable);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No data found in FCostable Data table"));
	}
	return false;
}

TArray<int> AIslandMainProjectGameModeBase::GetAmountsOfRequiredResource(FName sourceID)
{
	TArray<int> listOfAmount;
	if (!m_cachedVenturePawn) return listOfAmount;

	UInventoryComponent* playerInventoryComp = m_cachedVenturePawn->GetInventoryComponent();
	// Find the source ID
	FCostable* thisCostable = m_buildingResourceDB->FindRow<FCostable>(sourceID, "");
	if (thisCostable) {
		for (auto it = thisCostable->RequireResourceList.begin(); it != thisCostable->RequireResourceList.end(); ++it)
		{
			listOfAmount.Add(playerInventoryComp->GetItemAmount((*it).ItemID));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No data found in FCostable Data table"));
	}
	return listOfAmount;
}

bool AIslandMainProjectGameModeBase::CostResources(FName sourceID)
{
	if (!m_cachedVenturePawn) return false;

	UInventoryComponent* playerInventoryComp = m_cachedVenturePawn->GetInventoryComponent();
	// Find the source ID
	FCostable* thisCostable = m_buildingResourceDB->FindRow<FCostable>(sourceID, "");
	if (thisCostable) {
		for (auto it = thisCostable->RequireResourceList.begin(); it != thisCostable->RequireResourceList.end(); ++it)
		{
			int removedAmount = playerInventoryComp->ReduceItemAmount((*it).ItemID, (*it).Amount);
			if (removedAmount < (*it).Amount) UE_LOG(LogTemp, Warning, TEXT("Query Amount is more than the amout player has, didn't check call [CheckIfResourceEnouth(FName)] before to make sure there is enough item in player's inventory"));
		}
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No data found in FCostable Data table"));
	}
	return false;
}

class UMaterialInterface* AIslandMainProjectGameModeBase::GetBuildPlaceableMaterial()
{
	return this->m_materialBuildPlaceable;
}

class UMaterialInterface* AIslandMainProjectGameModeBase::GetBuildNotPlaceableMaterial()
{
	return this->m_materialBuildNotPlaceable;
}

void AIslandMainProjectGameModeBase::RegisterCachedBuildingPawn(ABuildingPawn * pawn)
{
	m_cachedBuildingPawn = pawn;
}

void AIslandMainProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	m_CurrentMode = EModeEnum::ME_Exploration;

	m_cachedVenturePawn = Cast<AVenturePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// TODO Remove?
	//if (!FindBuildingManager()) UE_LOG(LogTemp, Warning, TEXT("No Building Manager Found"));
}
