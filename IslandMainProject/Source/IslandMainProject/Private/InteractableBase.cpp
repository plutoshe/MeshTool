// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/InteractableBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapComp"));
	RootComponent = OverlapComp;
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(OverlapComp);
}

void AInteractableBase::Interact()
{
	if (!m_canIntereact) return;
	BPInteract();
}

void AInteractableBase::UnInteract()
{
	if (!m_canIntereact) return;
	BPUnInteract();
}

void AInteractableBase::EnableInputHint(const FName inputName)
{
	// Set the input in the interact widget
	WidgetComp->SetVisibility(true);
}

void AInteractableBase::DisableInputHint()
{
	WidgetComp->SetVisibility(false);
}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

