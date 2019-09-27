// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolPickaxe.h"
#include "Components/SphereComponent.h"
#include "InteractableBase.h"
#include "Damageable.h"

AToolPickaxe::AToolPickaxe()
{
	InteractPointComp->OnComponentBeginOverlap.AddDynamic(this, &AToolPickaxe::OnInteractableEnter);
	InteractPointComp->OnComponentEndOverlap.AddDynamic(this, &AToolPickaxe::OnInteractableLeft);

	Damage = 20.f;
}


void AToolPickaxe::StartUseTool()
{
	Super::StartUseTool();
	m_canInteract = true;
}

void AToolPickaxe::UseToolFinish()
{
	Super::UseToolFinish();
	m_interacted = false;
	m_canInteract = false;
}

void AToolPickaxe::OnInteractableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherComp != nullptr && !m_interacted && m_canInteract)
	{
		// This actor has Damageable event
		if (OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
		{
			IDamageable::Execute_GetDamage(OtherActor, Damage);
		}
		else if (OtherActor->IsA(AInteractableBase::StaticClass()))
		{
			AInteractableBase* interactive = Cast<AInteractableBase>(OtherActor);
			interactive->Interact();
			interactive->BPInteract();
		}
		m_interacted = true;
	}
}

void AToolPickaxe::OnInteractableLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherComp != nullptr)
	{
		// This actor has Damageable event
		if (OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
		{
		}
		else if (OtherActor->IsA(AInteractableBase::StaticClass()))
		{
			AInteractableBase* interactive = Cast<AInteractableBase>(OtherActor);
			interactive->UnInteract();
			interactive->BPUnInteract();
		}
	}
}