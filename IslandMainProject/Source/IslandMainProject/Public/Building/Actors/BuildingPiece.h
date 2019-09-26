// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPiece.generated.h"

class UStaticMeshComponent;
class UBuildingBlockComponent;
class UMaterialInterface;

UENUM( BlueprintType, Category ="Building Piece Material")
enum EBuildingPieceMaterialState
{
	UNPLACEABLE,
	PLACEABLE,
	STANDARD
};


UCLASS()
class ISLANDMAINPROJECT_API ABuildingPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingPiece();

	UFUNCTION(BlueprintCallable, Category = "Building Piece")
	TArray<UBuildingBlockComponent*> GetBuildingBlocks() const;

	UFUNCTION(BlueprintCallable, Category = "Building Piece")
	FIntVector GetRootBuildingIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Building Piece")
	void RotateBuildingBlock(bool clockwise);

	UFUNCTION(BlueprintCallable, Category = "Building Piece")
	void SetMaterial(EBuildingPieceMaterialState state);
protected:
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UBuildingBlockComponent* m_rootBlock;
	TArray<UBuildingBlockComponent*> m_blocks;
	TArray<UStaticMeshComponent*> m_meshes;
	TMap<UStaticMeshComponent*, TArray<UMaterialInterface*>> m_originalMaterialMap;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Parent", meta = (AllowPrivateAccess = "true"))
		USceneComponent* MainParent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Parent", meta = (AllowPrivateAccess = "true"))
		USceneComponent* MeshParent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Functional Parent", meta = (AllowPrivateAccess = "true"))
		USceneComponent* FunctionalParent;

	// Material Setting
	void SetMaterialOnAllMeshes(UMaterialInterface* material);
	void ResetMaterialOnAllMeshes();
};
