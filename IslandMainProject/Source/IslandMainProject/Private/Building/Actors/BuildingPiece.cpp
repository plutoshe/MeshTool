// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Building/Actors/BuildingPiece.h"
#include "IslandMainProjectGameModeBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Scene.h"
#include "Building/Components/BuildingBlockComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"

// Sets default values
ABuildingPiece::ABuildingPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Main Parent
	this->MainParent = CreateDefaultSubobject<USceneComponent>(TEXT("Main Parent"));
	this->MainParent->SetupAttachment(RootComponent);

	// Mesh Parent
	this->MeshParent = CreateDefaultSubobject<USceneComponent>(TEXT("Mesh Parent"));
	this->MeshParent->SetupAttachment(this->MainParent);

	// Functional Parent
	this->FunctionalParent = CreateDefaultSubobject<USceneComponent>(TEXT("Functional Parent"));
	this->FunctionalParent->SetupAttachment(this->MainParent);
}

TArray<UBuildingBlockComponent*> ABuildingPiece::GetBuildingBlocks() const
{
	return this->m_blocks;
}

FIntVector ABuildingPiece::GetRootBuildingIndex() const
{
	if (this->m_rootBlock == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Building piece has null root block"));
		return FIntVector::ZeroValue;
	}
	return this->m_rootBlock->GetBuildingIndexPosition();
}

void ABuildingPiece::RotateBuildingBlock(bool clockwise)
{
	TArray<UBuildingBlockComponent*> blocks;
	this->GetComponents<UBuildingBlockComponent>(blocks);
	for (size_t i = 0; i < blocks.Num(); i++)
	{
		//TODO Unfinished
	}
}

void ABuildingPiece::SetMaterial(EBuildingPieceMaterialState state)
{
	AIslandMainProjectGameModeBase* gameMode = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (gameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not set material due to null game mode"));
		return;
	}

	UMaterialInterface* material = nullptr;
	switch (state)
	{
	default:
		break;
	case UNPLACEABLE:
		material = gameMode->GetBuildNotPlaceableMaterial();
		break;
	case PLACEABLE:
		material = gameMode->GetBuildPlaceableMaterial();
		break;
	case STANDARD:
		this->ResetMaterialOnAllMeshes();
		return;
	}

	if (material == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not set material due to invalid material state"));
		return;
	}

	this->SetMaterialOnAllMeshes(material);
}

void ABuildingPiece::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Assign Root Block and Blocks. Assumes one with Local Index of 0 , 0. Otherwise picks first position
	TArray<UActorComponent*> aBlocks = this->GetComponentsByClass(UBuildingBlockComponent::StaticClass());
	this->m_blocks = TArray<UBuildingBlockComponent*>();
	for (size_t i = 0; i < aBlocks.Num(); i++)
	{
		UBuildingBlockComponent* block = Cast<UBuildingBlockComponent>(aBlocks[i]);

		// Add to list of blocks
		this->m_blocks.Add(block);

		// Check for root block
		FIntVector localPosition = block->GetLocalIndexPosition();
		if (localPosition.X == 0 && localPosition.Y == 0)
		{
			this->m_rootBlock = block;
		}
	}
	if (this->m_rootBlock == nullptr && m_blocks.Num() > 0)
		this->m_rootBlock = m_blocks[0];

	// Assign Meshes
	TArray<UActorComponent*> aMeshes = this->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	for (size_t i = 0; i < aMeshes.Num(); i++)
	{
		UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(aMeshes[i]);

		// Add to list of meshes
		this->m_meshes.Add(mesh);

		// Map Materials
		this->m_originalMaterialMap.Add(mesh, mesh->GetMaterials());
	}
}

// Called when the game starts or when spawned
void ABuildingPiece::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABuildingPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuildingPiece::SetMaterialOnAllMeshes(UMaterialInterface* material)
{
	for (size_t i = 0; i < m_meshes.Num(); i++)
	{
		if (m_meshes[i] == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Material could not be set due to null Referenced Mesh"));
			continue;
		}

		TArray<UMaterialInterface*> originalMaterials = this->m_originalMaterialMap[m_meshes[i]];
		for (size_t j = 0; j < originalMaterials.Num(); j++)
		{
			m_meshes[i]->SetMaterial(j, material);
		}
	}
}

void ABuildingPiece::ResetMaterialOnAllMeshes()
{
	for (size_t i = 0; i < m_meshes.Num(); i++)
	{
		if (m_meshes[i] == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Material could not be reset due to null Referenced Mesh"));
			continue;
		}

		TArray<UMaterialInterface*> originalMaterials = this->m_originalMaterialMap[m_meshes[i]];
		for (size_t j = 0; j < originalMaterials.Num(); j++)
		{
			m_meshes[i]->SetMaterial(j, originalMaterials[j]);
		}
	}
}