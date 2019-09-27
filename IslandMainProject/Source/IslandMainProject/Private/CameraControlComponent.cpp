// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraControlComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Classes/GameFramework/Pawn.h"

// Sets default values for this component's properties
UCameraControlComponent::UCameraControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UCameraControlComponent::InitializeCameraComponent()
{
	// Set the properties for camera components
	GetOwnersCameraComponents();
	SetCamerasProperties();

/*
	m_owner->InputComponent->BindAction("RotateCameraC", IE_Pressed, this, &UCameraControlComponent::RotateCamera90Clockwise);
	m_owner->InputComponent->BindAction("RotateCameraCC", IE_Pressed, this, &UCameraControlComponent::RotateCamera90CounterClockwise);
*/

		APawn* pawn = Cast<APawn>(m_owner);
		m_owner->InputComponent->BindAxis("LookUp");
		m_owner->InputComponent->BindAxis("LookRight");
		m_owner->InputComponent->BindAxis("ZoomIn");
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is an pawn doesn't get controller"));
	}
}

// Called when the game starts
void UCameraControlComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeCameraComponent();
}

void UCameraControlComponent::GetOwnersCameraComponents()
{
	m_owner = GetOwner();
	check(m_owner != nullptr);

	// Get m_owner's camera component
	TArray<UCameraComponent*> cameracomponents;
	m_owner->GetComponents(cameracomponents);
	check(cameracomponents.Num() == 1);
	if (cameracomponents.Num() == 1)
	{
		m_ownersCamera = cameracomponents[0];
	}

	// Get m_owner's spring arm component
	TArray<USpringArmComponent*> springarmcompnents;
	m_owner->GetComponents(springarmcompnents);
	check(springarmcompnents.Num() == 1);
	if (springarmcompnents.Num() == 1)
	{
		m_ownersCameraBoom = springarmcompnents[0];
	}

	return;
}

void UCameraControlComponent::SetCamerasProperties()
{

	// Create a camera boom...
	m_ownersCameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	m_ownersCameraBoom->TargetArmLength = 800.f;
	//m_ownersCameraBoom->RelativeRotation = FRotator(-45.0f, 0.f, 0.f);
	m_ownersCameraBoom->RelativeLocation = FVector(0.0f, 0.f, 0.f);
	m_ownersCameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	m_ownersCameraBoom->bUsePawnControlRotation = false;
	m_ownersCameraBoom->bInheritPitch = false;
	m_ownersCameraBoom->bInheritRoll = false;
	m_ownersCameraBoom->bInheritYaw = false;

		// Create a camera...
	m_ownersCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

/*
void UCameraControlComponent::RotateCamera90Clockwise()
{
	if (!m_bRotating)
	{
		m_destRotator = m_ownersCameraBoom->GetComponentRotation();
		m_destRotator.Add(0, 90, 0);
		m_bRotating = true;
	}
}*/

/*
void UCameraControlComponent::RotateCamera90CounterClockwise()
{
	if (!m_bRotating)
	{
		m_destRotator = m_ownersCameraBoom->GetComponentRotation();
		m_destRotator.Add(0, -90, 0);
		m_bRotating = true;
	}
}
*/

void UCameraControlComponent::PerformCameraRotation(float deltaSeconds)
{
	if(!m_owner->InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Component Is Null"));
		return;
	}
/*
	if (m_bRotating)
	{
		m_ownersCameraBoom->SetWorldRotation(FMath::RInterpTo(m_ownersCameraBoom->GetComponentRotation(), m_destRotator, deltaSeconds, 30.0f));
		if (m_ownersCameraBoom->GetComponentRotation().Equals(m_destRotator, 0.01f))
		{
			m_bRotating = false;
			m_ownersCameraBoom->SetWorldRotation(m_destRotator);
		}
	}*/

	// Rotate screen along with a mouse movement
	m_ownersCameraBoom->AddRelativeRotation(FRotator( (m_owner->InputComponent->GetAxisValue("LookUp")), m_owner->InputComponent->GetAxisValue("LookRight"), 0.f));
	// Zoom in to a screen based on mouse wheel axis, -10 is a magic number to adjust intensity
	m_ownersCameraBoom->TargetArmLength += -5 * m_owner->InputComponent->GetAxisValue("ZoomIn");
}


// Called every frame
void UCameraControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
	PerformCameraRotation(DeltaTime);
}
