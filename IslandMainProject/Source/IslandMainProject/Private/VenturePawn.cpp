// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Public/VenturePawn.h"
#include "Public/Constants/GameInputConstants.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "IslandMainProjectGameModeBase.h"
#include "Components/SphereComponent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"

#include "Public/ToolMover.h"
#include "Public/PickUpBase.h"
/* ----Useful HEAD
#include "Public/APToolBase.h"
#include "BuildingSystemPawn.h"
*/
#include "Public/InteractableBase.h"
#include "Public/InventoryComponent.h"
#include "Public/CameraControlComponent.h"

AVenturePawn::AVenturePawn()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// Create a camera...
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComp->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Interact Point component
	InteractPointComp = CreateDefaultSubobject<USphereComponent>(TEXT("InteractPointComp"));
	InteractPointComp->SetupAttachment(RootComponent);
	InteractPointComp->SetSphereRadius(32.f);
	InteractPointComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractPointComp->OnComponentBeginOverlap.AddDynamic(this, &AVenturePawn::OnInteractableEnter);
	InteractPointComp->OnComponentEndOverlap.AddDynamic(this, &AVenturePawn::OnInteractableLeft);

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	CameraControlComp = CreateDefaultSubobject<UCameraControlComponent>(TEXT("CameraControlComp"));
}

void AVenturePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateMoverTransformation();
}
void AVenturePawn::BeginPlay()
{
	Super::BeginPlay();
	// Spawn tools when activates
	//SpawnUsefulTools();

	SpawnMover();
	OnMouseHold();
}


#pragma region PlayerInputFunctions
void AVenturePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AVenturePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVenturePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AVenturePawn::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AVenturePawn::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AVenturePawn::UnCrouch);

	PlayerInputComponent->BindAction("Item1", IE_Pressed, this, &AVenturePawn::SwitchToItem1);
	PlayerInputComponent->BindAction("Item2", IE_Pressed, this, &AVenturePawn::SwitchToItem2);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AVenturePawn::OnMouseClick);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AVenturePawn::OnMouseRelease);

	//Initialize for CameraControl Component
	CameraControlComp->InitializeCameraComponent();
}

void AVenturePawn::MoveForward(float axis)
{
	if ((axis > 0.1f || axis < -0.1f)) CancelMoveToLocation();
	FVector dir = CameraComp->GetForwardVector();
	dir.Z = 0;
	AddMovementInput(dir.GetSafeNormal()*axis);
}

void AVenturePawn::MoveRight(float axis)
{
	if ((axis > 0.1f || axis < -0.1f)) CancelMoveToLocation();

	AddMovementInput(CameraComp->GetRightVector() * axis);

}

void AVenturePawn::Jump()
{
	Super::Jump();
}

void AVenturePawn::Crouch()
{
	Super::Crouch();
}

void AVenturePawn::UnCrouch()
{
	Super::UnCrouch();
}



#pragma endregion

void AVenturePawn::OnMouseClick()
{
	//if (m_currentInteractable != nullptr)
	//{
	//	m_currentInteractable->Interact();
	//}
	//else
	//{
	//	// Debug use
	//	m_toolMover->UseTool();
	//	m_OnMouseHold = true;
	//}

	m_toolMover->UseTool();
	m_OnMouseHold = true;
}

void AVenturePawn::OnMouseHold()
{
	if (m_OnMouseHold) {
		m_toolMover->UseTool();
	}
}

void AVenturePawn::OnMouseRelease()
{
	m_OnMouseHold = false;
}

void AVenturePawn::SwitchToItem1()
{
	// Debug use
	m_toolMover->SwitchToTool(FName("Harpoon"));
}

void AVenturePawn::SwitchToItem2()
{
	// Debug use
	m_toolMover->SwitchToTool(FName("Pickaxe"));
}

/* ----Useful HEAD
void AVenturePawn::InteractWithTool(AAPInteractItemBase* interactBase)
{
	if (interactBase) {
		if (interactBase->m_bRequireTool) {
			if (m_usingTool != nullptr) {
				m_usingTool->StartUse(interactBase);
			}
		}
		else {
			interactBase->Interact();
		}
	}
	else {
		if (m_usingTool != nullptr) {
			m_usingTool->StartUse(nullptr);
		}
	}

}
*/
void AVenturePawn::SetNewMoveDestination(const FHitResult& outHit)
{
	/* ----Useful HEAD
	AAPInteractItemBase* interactBase = Cast<AAPInteractItemBase>(outHit.Actor);

	if (interactBase) {
		float const Distance = FVector::Dist(outHit.ImpactPoint, MyPawn->GetActorLocation());

		if ((Distance > 120.0f))
		{
			if (!interactBase->GetCanPawnInteract())
				MoveTo(GetController(), outHit.ImpactPoint);
			//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, outHit.ImpactPoint);
		}

		if (interactBase->GetCanPawnInteract()) {
			InteractWithTool(interactBase);
			FRotator newROt = (outHit.Actor->GetActorLocation() - GetActorLocation()).Rotation();
			newROt.Pitch = 0;
			newROt.Roll = 0;
			SetActorRotation(newROt);
		}
	}
	else {
		// not interacting with interactItemBase
		InteractWithTool(nullptr);
	}
	*/
}
UPathFollowingComponent* AVenturePawn::InitNavigationControl(AController& Controller)
{

	AAIController* AsAIController = Cast<AAIController>(&Controller);
	UPathFollowingComponent* PathFollowingComp = nullptr;

	if (AsAIController)
	{
		PathFollowingComp = AsAIController->GetPathFollowingComponent();
	}
	else
	{
		PathFollowingComp = Controller.FindComponentByClass<UPathFollowingComponent>();
		if (PathFollowingComp == nullptr)
		{
			PathFollowingComp = NewObject<UPathFollowingComponent>(&Controller);
			PathFollowingComp->RegisterComponentWithWorld(Controller.GetWorld());
			PathFollowingComp->Initialize();
		}
	}

	return PathFollowingComp;

}

void AVenturePawn::OnInteractableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {
		AInteractableBase* interactBase = Cast<AInteractableBase>(OtherActor);
		if (interactBase){
			interactBase->SetCanInteract(true);
			interactBase->EnableInputHint(FName("E"));
			UE_LOG(LogTemp, Log, TEXT("Interact Enter"));
			m_currentInteractable = interactBase;
		}
	}

}

void AVenturePawn::OnInteractableLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {
		AInteractableBase* interactBase = Cast<AInteractableBase>(OtherActor);
		if (interactBase) {
			interactBase->SetCanInteract(false);
			interactBase->DisableInputHint();
			interactBase->UnInteract();
			m_currentInteractable = nullptr;

		}
	}
}

void AVenturePawn::SpawnMover()
{
	m_toolMover = GetWorld()->SpawnActor<AToolMover>(ToolMoverClass);
}

void AVenturePawn::UpdateMoverTransformation()
{
	if (m_toolMover) {
		m_toolMover->SetActorLocation(GetActorLocation() + MoverRelativeLocation);
		m_toolMover->SetActorRotation(GetActorRotation() + MoverRelativeRotatoin);
	}
}

void AVenturePawn::MoveTo(AController* i_Controller, const FVector& GoalLocation)
{
	UNavigationSystemV1* NavSys = i_Controller ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(i_Controller->GetWorld()) : nullptr;
	if (NavSys == nullptr || i_Controller == nullptr || i_Controller->GetPawn() == nullptr)
	{
		UE_LOG(LogNavigation, Warning, TEXT("UNavigationSystemV1::SimpleMoveToActor called for NavSys:%s i_Controller:%s controlling Pawn:%s (if any of these is None then there's your problem"),
			*GetNameSafe(NavSys), *GetNameSafe(i_Controller), i_Controller ? *GetNameSafe(i_Controller->GetPawn()) : TEXT("NULL"));
		return;
	}

	m_PFollowComp = InitNavigationControl(*i_Controller);
	UE_LOG(LogTemp, Log, TEXT("init: %d"), m_PFollowComp->bIsActive);

	if (m_PFollowComp == nullptr)
	{
		return;
	}

	if (!m_PFollowComp->IsPathFollowingAllowed())
	{
		return;
	}

	const bool bAlreadyAtGoal = m_PFollowComp->HasReached(GoalLocation, EPathFollowingReachMode::OverlapAgent);

	// script source, keep only one move request at time
	if (m_PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
	{
		m_PFollowComp->AbortMove(*NavSys, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest
			, FAIRequestID::AnyRequest, bAlreadyAtGoal ? EPathFollowingVelocityMode::Reset : EPathFollowingVelocityMode::Keep);
	}

	// script source, keep only one move request at time
	if (m_PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
	{
		m_PFollowComp->AbortMove(*NavSys, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest);
	}

	if (bAlreadyAtGoal)
	{
		m_PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
	}
	else
	{
		const ANavigationData* NavData = NavSys->GetNavDataForProps(i_Controller->GetNavAgentPropertiesRef());
		if (NavData)
		{
			FPathFindingQuery Query(i_Controller, *NavData, i_Controller->GetNavAgentLocation(), GoalLocation);
			FPathFindingResult Result = NavSys->FindPathSync(Query);
			if (Result.IsSuccessful())
			{
				m_PFollowComp->RequestMove(FAIMoveRequest(GoalLocation), Result.Path);
			}
			else if (m_PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
			{
				m_PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
			}
		}
	}
}

void AVenturePawn::CancelMoveToLocation()
{
	if (m_PFollowComp && m_PFollowComp->GetStatus() == EPathFollowingStatus::Moving)
		m_PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Aborted);
}
