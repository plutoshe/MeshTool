// Fill out your copyright notice in the Description page of Project Settings.


#include "IslandMainProjectGameModeBase.h"
#include "Public/VenturePawn.h"
#include "Public/InventoryComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
	/* ----Useful HEAD
#include "AncientWorldPlayerController.h"
#include "BuildingSystemPawn.h"
#include "BuildingSynchronization.h"
*/
AIslandMainProjectGameModeBase::AIslandMainProjectGameModeBase()
{
	m_inBuildingMode = false;
}

void AIslandMainProjectGameModeBase::SwitchToMode(EModeEnum mode)
{
	switch (mode)
	{
	case EModeEnum::ME_Exploration:
		if (mode != m_CurrentMode) {
			m_CurrentMode = mode;
			// Switch to exploration mode

			if (m_cachedVenturePawn == nullptr) {
				m_cachedVenturePawn = GetWorld()->SpawnActor<AVenturePawn>(m_venturePawnClass);
			}

			APlayerController* controller = GetWorld()->GetFirstPlayerController();
			controller->UnPossess();
			controller->Possess(m_cachedVenturePawn);
			UE_LOG(LogTemp, Warning, TEXT("Go to Exploration Mode"));
		}
		break;
	case EModeEnum::ME_Building:
		if (mode != m_CurrentMode) {
			m_CurrentMode = mode;
			// Switch to building mode

/* ----Useful HEAD
			// if it is a nullptr
			if (m_cachedBuildingPawn == nullptr) {
				m_cachedBuildingPawn = GetWorld()->SpawnActor<ABuildingSystemPawn>(m_buildingPawnClass);
			}

			APlayerController* controller = GetWorld()->GetFirstPlayerController();
			controller->UnPossess();
			controller->Possess(m_cachedBuildingPawn);
			m_cachedBuildingPawn->m_BuildingSystem = m_buildingManager;

			m_cachedBuildingPawn->SetActorLocation(m_buildingManager->GetActorLocation());
			UE_LOG(LogTemp, Warning, TEXT("Go to Building Mode"));
			*/
		}
		break;
	default:
		break;
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

bool AIslandMainProjectGameModeBase::FindBuildingManager()
{
	/* ----Useful HEAD
	if (m_buildingManager == nullptr) {
		for (TActorIterator<ABuildingSynchronization> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ABuildingSynchronization* buildingMan = Cast<ABuildingSynchronization>(*ActorItr);
			if (buildingMan) {
				m_buildingManager = buildingMan;
				return true;
			}
		}
		return false;
	}
	else */return true;
}

void AIslandMainProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	m_cachedVenturePawn = Cast<AVenturePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!FindBuildingManager()) UE_LOG(LogTemp, Warning, TEXT("No Building Manager Found"));

}
