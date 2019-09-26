// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Building/Pawns/BuildingPawn.h"
#include "Public/Building/Actors/Building.h"
#include "Public/Building/Actors/BuildingPiece.h"
#include "Public/Building/Components/BuildingBlockComponent.h"
#include "Engine/Classes/GameFramework/Pawn.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Public/CameraControlComponent.h"
#include "GameFramework/PlayerController.h"
#include "Public/Constants/GameInputConstants.h"
#include "IslandMainProjectGameModeBase.h"
#include "Engine/World.h"

// Sets default values
ABuildingPawn::ABuildingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CreateDefaultSubobject<USceneComponent>(TEXT("OriginPoint"))->SetupAttachment(RootComponent);

	
	// Primary Camera Boom
	this->PrimaryCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("PrimaryCameraBoom"));
	this->PrimaryCameraBoom->SetupAttachment(RootComponent);
	this->PrimaryCameraBoom->bDoCollisionTest = true;
	this->PrimaryCameraBoom->bInheritPitch = false;
	this->PrimaryCameraBoom->bInheritRoll = false;
	this->PrimaryCameraBoom->bInheritYaw = false;
	
	// Primary Camera
	this->PrimaryCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PrimaryCameraComponent"));
	this->PrimaryCameraComponent->SetupAttachment(this->PrimaryCameraBoom);

	// Camera Control Comp
	this->CameraControlComp = CreateDefaultSubobject<UCameraControlComponent>(TEXT("CameraControlComp"));
}

// Called when the game starts or when spawned
void ABuildingPawn::BeginPlay()
{
	Super::BeginPlay();

	if (this->m_currentPiece == nullptr && this->m_targetBuilding != nullptr)
		SetTargetBuildingAndInitialize(this->m_targetBuilding);
}

// Called every frame
void ABuildingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABuildingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(GameInputConstants::CONFIRM, IE_Pressed, this, &ABuildingPawn::ConfirmCurrentPiece);

	// Movement Action Inputs
	PlayerInputComponent->BindAction(GameInputConstants::MOVE_LEFT, IE_Pressed, this, &ABuildingPawn::MovePieceXNeg);
	PlayerInputComponent->BindAction(GameInputConstants::MOVE_RIGHT, IE_Pressed, this, &ABuildingPawn::MovePieceXPos);

	PlayerInputComponent->BindAction(GameInputConstants::MOVE_UP, IE_Pressed, this, &ABuildingPawn::MovePieceYNeg);
	PlayerInputComponent->BindAction(GameInputConstants::MOVE_DOWN, IE_Pressed, this, &ABuildingPawn::MovePieceYPos);

	PlayerInputComponent->BindAction(GameInputConstants::MOVE_DESCEND, IE_Pressed, this, &ABuildingPawn::MovePieceZNeg);
	PlayerInputComponent->BindAction(GameInputConstants::MOVE_ASCEND, IE_Pressed, this, &ABuildingPawn::MovePieceZPos);

	PlayerInputComponent->BindAction(GameInputConstants::CYCLE_PIECE_LEFT, IE_Pressed, this, &ABuildingPawn::CyclePieceLeft);
	PlayerInputComponent->BindAction(GameInputConstants::CYCLE_PIECE_RIGHT, IE_Pressed, this, &ABuildingPawn::CyclePieceRight);

	CameraControlComp->InitializeCameraComponent();

	if (Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
		m_gameMode = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	PlayerInputComponent->BindAction(GameInputConstants::CHANGE_GAME_MODE, IE_Pressed, m_gameMode, &AIslandMainProjectGameModeBase::GoToExploreMode);
}

void ABuildingPawn::SetTargetBuildingAndInitialize(ABuilding* targetBuilding)
{
	if (targetBuilding == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant initialize with null target building"));
		return;
	}
		

	this->m_targetBuilding = targetBuilding;
	this->SetActorLocation(this->m_targetBuilding->GetActorLocation());

	this->m_currentSelectedPieceIndex = 0;
	this->LoadPiece(this->m_currentSelectedPieceIndex);
}

void ABuildingPawn::CreateCurrentPiece(TSubclassOf<ABuildingPiece> pieceToBuild)
{
	if (!this->m_targetBuilding)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Targeted Building Found To Create Piece With"));
		return;
	}

	if (this->m_currentPiece != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant create another piece, another one is currently being made"));
		return;
	}

	if(!pieceToBuild)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid input piece class to create piece"));
		return;
	}

	// Create and spawn piece
	FActorSpawnParameters spawnInfo;
	spawnInfo.Owner = this;
	spawnInfo.Instigator = Instigator;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FIntVector indexSpawnPosition = this->m_targetBuilding->GetBuildingIndexSpawn(true);
	FVector worldSpawnPosition = this->m_targetBuilding->GetWorldPositionFromIndex(indexSpawnPosition);

	this->m_currentPiece = GetWorld()->SpawnActor<ABuildingPiece>(pieceToBuild, worldSpawnPosition, FRotator::ZeroRotator, spawnInfo);

	MoveCurrentPiece(indexSpawnPosition);
}

void ABuildingPawn::MoveCurrentPiece(FIntVector position)
{
	// Check piece
	if (this->m_currentPiece == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No set building piece to move."));
		return;
	}

	// Check Boundaries
	if (!this->m_targetBuilding->IsInBoundaries(position))
	{
		return;
	}

	// Move Index Position
	TArray<UBuildingBlockComponent*> blocks = this->m_currentPiece->GetBuildingBlocks();
	for (size_t i = 0; i < blocks.Num(); i++)
	{
		UBuildingBlockComponent* block = blocks[i];
		block->SetBuildingIndexPosition(position + block->GetLocalIndexPosition());
	}

	// Move World Position
	this->m_currentPiece->SetActorLocation(m_targetBuilding->GetWorldPositionFromIndex(position));

	this->UpdatePlacementIndicator();
}

void ABuildingPawn::LoadPiece(int pieceIndex)
{
	// Set initial piece
	// Get list of spawnable pieces
	TArray<TSubclassOf<ABuildingPiece>> buildingPieces = this->m_targetBuilding->GetPossibleBuildingPieces();
	if (pieceIndex < 0 || pieceIndex >= buildingPieces.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Piece index out of range"));
		return;
	}

	// For now just pick first one
	this->CreateCurrentPiece(buildingPieces[pieceIndex]);
}

void ABuildingPawn::CycleCurrentPiece(bool forward)
{
	TArray<TSubclassOf<ABuildingPiece>> buildingPieces = this->m_targetBuilding->GetPossibleBuildingPieces();
	if (buildingPieces.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No pieces to cycle"));
		return;
	}

	int nextIndex = m_currentSelectedPieceIndex;
	if (forward)
	{
		nextIndex = (nextIndex + 1) % buildingPieces.Num();
	}
	else
	{
		nextIndex = nextIndex - 1;
		if (nextIndex < 0)
			nextIndex = buildingPieces.Num() - 1;
	}

	this->m_currentPiece->Destroy();
	this->m_currentPiece = nullptr;
	LoadPiece(nextIndex);
}

void ABuildingPawn::UpdatePlacementIndicator() 
{
	if (!this->m_targetBuilding)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid target building to update placement indicator with"));
		return;
	}

	if (!this->m_currentPiece)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid current piece class to update placement indicator"));
		return;
	}

	if (this->m_targetBuilding->CanAttachPiece(this->m_currentPiece))
		this->m_currentPiece->SetMaterial(PLACEABLE);
	else
		this->m_currentPiece->SetMaterial(UNPLACEABLE);

	return;
}

void ABuildingPawn::ConfirmCurrentPiece()
{
	if (!this->m_targetBuilding)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Targeted Building Found To Confirm"));
		return;
	}

	if (!this->m_currentPiece)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid current piece class to confirm piece"));
		return;
	}

	if (this->m_targetBuilding->AttachPiece(this->m_currentPiece))
	{
		this->m_currentPiece->SetMaterial(STANDARD);
		this->m_currentPiece = nullptr;
		this->LoadPiece(this->m_currentSelectedPieceIndex);
	}
	else
	{
		// Dont do shit if couldnt confirm the current piece
		UE_LOG(LogTemp, Display, TEXT("Couldn't confirm piece"));
	}

	return;
}

void ABuildingPawn::MovePieceXNeg()
{
	if(m_currentPiece!= nullptr)
		this->MoveCurrentPiece(m_currentPiece->GetRootBuildingIndex() + FIntVector(-1, 0, 0));
}

void ABuildingPawn::MovePieceXPos()
{
	if (m_currentPiece != nullptr)
		this->MoveCurrentPiece(m_currentPiece->GetRootBuildingIndex() + FIntVector(1, 0, 0));
}

void ABuildingPawn::MovePieceYNeg()
{
	if (m_currentPiece != nullptr)
		this->MoveCurrentPiece(m_currentPiece->GetRootBuildingIndex() + FIntVector(0, -1, 0));
}

void ABuildingPawn::MovePieceYPos()
{
	if (m_currentPiece != nullptr)
		this->MoveCurrentPiece(m_currentPiece->GetRootBuildingIndex() + FIntVector(0, 1, 0));
}

void ABuildingPawn::MovePieceZNeg()
{
	if (m_currentPiece != nullptr)
		this->MoveCurrentPiece(m_currentPiece->GetRootBuildingIndex() + FIntVector(0, 0, -1));
}

void ABuildingPawn::MovePieceZPos()
{
	if (m_currentPiece != nullptr)
		this->MoveCurrentPiece(m_currentPiece->GetRootBuildingIndex() + FIntVector(0, 0, 1));
}

void ABuildingPawn::MovePieceRotateNeg()
{
	
}

void ABuildingPawn::MovePieceRotatePos()
{
	
}

void ABuildingPawn::CyclePieceLeft()
{
	this->CycleCurrentPiece(false);
}

void ABuildingPawn::CyclePieceRight()
{
	this->CycleCurrentPiece(true);
}

