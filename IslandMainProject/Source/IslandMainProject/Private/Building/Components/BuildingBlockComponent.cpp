// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Building/Components/BuildingBlockComponent.h"

// Sets default values for this component's properties
UBuildingBlockComponent::UBuildingBlockComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

FIntVector UBuildingBlockComponent::GetLocalIndexPosition()
{
	return this->LocalIndexPosition;
}

FIntVector UBuildingBlockComponent::GetBuildingIndexPosition()
{
	return this->BuildingIndexPosition;
}

void UBuildingBlockComponent::SetLocalIndexPosition(FIntVector position)
{
	this->LocalIndexPosition = position;
}

void UBuildingBlockComponent::SetBuildingIndexPosition(FIntVector position)
{
	this->BuildingIndexPosition = position;
}

// Called when the game starts
void UBuildingBlockComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

