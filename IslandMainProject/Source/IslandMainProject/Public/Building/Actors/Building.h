// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

class ABuildingPiece;

UCLASS()
class ISLANDMAINPROJECT_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Properties")
	FIntVector BuildBoundaries = FIntVector(3,3,6);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Properties")
	float GridSize = 100.0f;

	// Sets default values for this actor's properties
	ABuilding();

	UFUNCTION(BlueprintCallable, Category = "Building")
	const TArray<TSubclassOf<ABuildingPiece>> GetPossibleBuildingPieces() const;

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool AttachPiece(ABuildingPiece* buildingPiece);

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool CanAttachPiece(ABuildingPiece* buildingPiece);

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool IsOccupied(FIntVector index);
	
	UFUNCTION(BlueprintCallable, Category = "Building")
	bool IsInBoundaries(FIntVector index);

	UFUNCTION(BlueprintCallable, Category = "Building")
	FVector GetWorldPositionFromIndex(FIntVector buildingIndexPosition);

	UFUNCTION(BlueprintCallable, Category = "Building")
	FIntVector GetBuildingIndexSpawn(bool positiveZ);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Parent", meta = (AllowPrivateAccess = "true"))
	USceneComponent* PiecesParent;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Blocks", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<ABuildingPiece>> m_possibleBuildingBlocks;

	// The spacial pieces of the map. Null pts mean a piece isn't there
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Blocks", meta = (AllowPrivateAccess = "true"))
	TMap<FIntVector, class UBuildingBlockComponent*> m_buildingMap;
};
