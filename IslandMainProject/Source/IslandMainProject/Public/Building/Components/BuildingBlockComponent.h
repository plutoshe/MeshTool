// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingBlockComponent.generated.h"



UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UBuildingBlockComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Properties", meta = (AllowPrivateAccess = "true"))
		bool Attachable = true;

	// Sets default values for this component's properties
	UBuildingBlockComponent();

	// Getters
	FIntVector GetLocalIndexPosition();
	FIntVector GetBuildingIndexPosition();

	// Setters
	void SetLocalIndexPosition(FIntVector position);
	void SetBuildingIndexPosition(FIntVector position);
protected:

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Properties", meta = (AllowPrivateAccess = "true"))
		FIntVector LocalIndexPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block Properties", meta = (AllowPrivateAccess = "true"))
		FIntVector BuildingIndexPosition;
};
