// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/ToolBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Public/ToolMover.h"

// Sets default values
AToolBase::AToolBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SuperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SuperMesh"));
	SuperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = RootComponent;

	InteractPointComp = CreateDefaultSubobject<USphereComponent>(TEXT("InteractPoint"));
	InteractPointComp->SetupAttachment(SuperMesh);

	InteractPointComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void AToolBase::StartUseTool()
{
	if (m_using || !m_enable) return;
	m_using = true;
	GetWorldTimerManager().SetTimer(m_CastHandle, this, &AToolBase::UseToolFinish, CoolDown, false);

}

void AToolBase::UseToolFinish()
{
	if (!m_using) return;
	m_using = false;
	if (m_parentMover)
		m_parentMover->UseToolFinish();

}

void AToolBase::SetMover(class AToolMover* mover)
{
	m_parentMover = mover;
}

void AToolBase::ToggleTool(bool enable)
{
	m_enable = enable;
	SuperMesh->SetVisibility(enable);
	InteractPointComp->SetCollisionEnabled(enable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AToolBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AToolBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

