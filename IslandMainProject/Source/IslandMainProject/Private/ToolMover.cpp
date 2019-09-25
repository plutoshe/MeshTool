// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/ToolMover.h"
#include "Components/SceneComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ChildActorComponent.h"
#include "IslandMainProjectGameModeBase.h"
#include "Public/ToolBase.h"
// Sets default values
AToolMover::AToolMover()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ContainerComp = CreateDefaultSubobject<USceneComponent>(TEXT("Container"));
	RootComponent = ContainerComp;

	SuperMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SuperMesh"));
	SuperMesh->SetupAttachment(RootComponent);

	AttacherComp = CreateDefaultSubobject<USceneComponent>(TEXT("Attacher"));
	AttacherComp->SetupAttachment(RootComponent);

}

void AToolMover::ToogleToolReady(bool ready)
{

	m_ToolReady = ready;
	if (m_ToolReady) {
		BPGetToolReady(m_currentTool->GetToolID());
	}
	else {
		BPDisconnectTool();
	}

}

void AToolMover::UseTool()
{
	if (!m_Using && m_currentTool && m_ToolReady) {
		m_Using = true;
		m_currentTool->StartUseTool();
		BPUseTool(m_currentTool->GetToolID());
	}

}

void AToolMover::UseToolFinish()
{
	if (m_Using && m_currentTool && m_ToolReady) {
		m_Using = false;
		BPUseToolFinish();
	}

}

// Called when the game starts or when spawned
void AToolMover::BeginPlay()
{
	Super::BeginPlay();
	SpawnToolsAtBeginning();
}

void AToolMover::SwitchToMode(EToolState newState)
{
	if (newState != m_currentState) {
		m_currentState = newState;
		switch (m_currentState)
		{
		case EToolState::TS_Idle:

			break;
		case EToolState::TS_Ready:

			break;
		case EToolState::TS_Using:

			break;
		default:
			break;
		}
	}
}


void AToolMover::SpawnToolsAtBeginning()
{
	AIslandMainProjectGameModeBase* GM = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GM) {
		UDataTable* toolTable = GM->GetToolDB();
		if (toolTable) {
			TArray<FName> RowNames = toolTable->GetRowNames();
			for (auto& name : RowNames)
			{
				FUseableTool* useableTool = toolTable->FindRow<FUseableTool>(name, "");
				if (useableTool)
				{
					FActorSpawnParameters spawnParameter;
					// Make sure it spawns no matter what happens;
					spawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					AToolBase* toolBase = GetWorld()->SpawnActor<AToolBase>(useableTool->ToolActorClass, spawnParameter);
					toolBase->ToggleTool(false);
					toolBase->SetMover(this);
					m_cachedToolList.Add(toolBase);
				}
			}

		}
	}
}

// Called every frame
void AToolMover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttacherComp->SetWorldLocation(SuperMesh->GetSocketLocation("RingSocket"));
	AttacherComp->SetWorldRotation(SuperMesh->GetSocketRotation("RingSocket"));
}

void AToolMover::SwitchToTool(FName toolID)
{
	if (m_Using) return;
	if (m_currentTool == nullptr) {
		// No tool is geting ready
		for (auto it = m_cachedToolList.begin(); it != m_cachedToolList.end(); ++it)
		{
			if ((*it)->GetToolID().IsEqual(toolID)) {
				m_currentTool = (*it);
				m_currentTool->ToggleTool(true);
				m_currentTool->AttachToComponent(AttacherComp, FAttachmentTransformRules::KeepRelativeTransform);
				ToogleToolReady(true);
				return;
			}
		}
	}
	else {
		// there is a tool already
		if (!m_currentTool->GetToolID().IsEqual(toolID)) {
			// it is not the same one, disconnect the previous one
			m_currentTool->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			m_currentTool->ToggleTool(false);
			m_currentTool = nullptr;
			// find the new tool id in cached list
			for (auto it = m_cachedToolList.begin(); it != m_cachedToolList.end(); ++it)
			{
				if ((*it)->GetToolID().IsEqual(toolID)) {
					m_currentTool = (*it);
					// if find a matching one
					m_currentTool->ToggleTool(true);
					m_currentTool->AttachToComponent(AttacherComp, FAttachmentTransformRules::KeepRelativeTransform);
					BPSwitchTool(toolID);
					return;
				}
			}
			// not find a matching one
			ToogleToolReady(false);
		}
		else {
			// it is the same one
			ToogleToolReady(!m_ToolReady);
		}
	}

}

