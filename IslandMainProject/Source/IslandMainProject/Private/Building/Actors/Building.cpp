// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Building/Actors/Building.h"
#include "Public/Building/Actors/BuildingPiece.h"
#include "Building/Components/BuildingBlockComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* mainParent = CreateDefaultSubobject<USceneComponent>(TEXT("Main Parent"));
	mainParent->SetupAttachment(RootComponent);

	this->PiecesParent = CreateDefaultSubobject<USceneComponent>(TEXT("Pieces Parent"));
	this->PiecesParent->SetupAttachment(mainParent);
	
	this->m_buildingMap = TMap<FIntVector, UBuildingBlockComponent*>();
}

const TArray<TSubclassOf<ABuildingPiece>> ABuilding::GetPossibleBuildingPieces() const
{
	return this->m_possibleBuildingBlocks;
}

bool ABuilding::AttachPiece(ABuildingPiece * buildingPiece)
{
	if (!this->CanAttachPiece(buildingPiece))
	{
		return false;
	}

	TArray<UBuildingBlockComponent*> blocks = buildingPiece->GetBuildingBlocks();
	for (size_t i = 0; i < blocks.Num(); i++)
	{
		FIntVector blockIndex = blocks[i]->GetBuildingIndexPosition();
		this->m_buildingMap.Add(blockIndex,blocks[i]);
	}
	return true;
}

bool ABuilding::CanAttachPiece(ABuildingPiece* buildingPiece)
{
	if (buildingPiece == nullptr)
		return false;

	// Check space is available and within boundaries
	TArray<UBuildingBlockComponent*> blocks = buildingPiece->GetBuildingBlocks();
	for (size_t i = 0; i < blocks.Num(); i++)
	{
		FIntVector blockIndex = blocks[i]->GetBuildingIndexPosition();
		if (!this->IsInBoundaries(blockIndex) || this->IsOccupied(blockIndex))
			return false;
	}
	return true;
}

bool ABuilding::IsOccupied(FIntVector index)
{
	return this->m_buildingMap.Contains(index);
}

bool ABuilding::IsInBoundaries(FIntVector index)
{
	return (index.X <= this->BuildBoundaries.X && index.X >= this->BuildBoundaries.X* -1.0f &&
		index.Y <= this->BuildBoundaries.Y && index.Y >= this->BuildBoundaries.Y* -1.0f &&
		index.Z <= this->BuildBoundaries.Z && index.Z >= this->BuildBoundaries.Z* -1.0f);
}

FVector ABuilding::GetWorldPositionFromIndex(FIntVector buildingIndexPosition)
{
	FVector indexConverted = FVector(buildingIndexPosition.X, buildingIndexPosition.Y, buildingIndexPosition.Z);
	FVector buildingPosition = GetActorLocation();
	return buildingPosition + (indexConverted * this->GridSize);
}

FIntVector ABuilding::GetBuildingIndexSpawn(bool positiveZ)
{
	FIntVector startPosition = FIntVector(0, 0, 0);
	
	FIntVector offset = FIntVector(0,0,1);
	if (!positiveZ)
		offset*-1;

	while (true)
	{
		if (!this->IsOccupied(startPosition))
			return startPosition;

		if (!this->IsInBoundaries(startPosition + offset))
			break;

		startPosition += offset;
	}

	return startPosition;
} 

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
