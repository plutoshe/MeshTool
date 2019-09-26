// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BuildingPawn.generated.h"

class ABuilding;
class ABuildingPiece;

UCLASS()
class ISLANDMAINPROJECT_API ABuildingPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABuildingPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PrimaryCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* PrimaryCameraBoom;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Input
	virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Target Building")
	void SetTargetBuildingAndInitialize(ABuilding* targetBuilding);
	
private:
	// Build Target
	UPROPERTY(EditInstanceOnly, Category = "Building", meta = (AllowPrivateAccess = "true"))
	ABuilding* m_targetBuilding;

	// Current Piece
	UPROPERTY(VisibleAnywhere , Category = "Building")
	ABuildingPiece* m_currentPiece;

	UPROPERTY(VisibleAnywhere, Category = "Building")
	int m_currentSelectedPieceIndex;

	// Piece manipulation
	void CreateCurrentPiece(TSubclassOf<ABuildingPiece> pieceToBuild);
	void MoveCurrentPiece(FIntVector position);
	void LoadPiece(int pieceIndex);
	void CycleCurrentPiece(bool forward);
	void UpdatePlacementIndicator();

	// Piece Input functions
	void ConfirmCurrentPiece();
	void MovePieceXNeg();
	void MovePieceXPos();
	void MovePieceYNeg();
	void MovePieceYPos();
	void MovePieceZNeg();
	void MovePieceZPos();
	void MovePieceRotateNeg();
	void MovePieceRotatePos();
	void CyclePieceLeft();
	void CyclePieceRight();
};
