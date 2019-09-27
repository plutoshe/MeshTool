// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Tool/ToolHarpoon.h"
#include "Components/SphereComponent.h"
#include "Damageable.h"

AToolHarpoon::AToolHarpoon()
{
	InteractPointComp->OnComponentBeginOverlap.AddDynamic(this, &AToolHarpoon::OnInteractableEnter);
	InteractPointComp->OnComponentEndOverlap.AddDynamic(this, &AToolHarpoon::OnInteractableLeft);

	Damage = 20.f;
}

void AToolHarpoon::StartUseTool()
{
	Super::StartUseTool();
	m_canDealDmg = true;
}

void AToolHarpoon::UseToolFinish()
{
	Super::UseToolFinish();
	m_dmgDealt = false;
	m_canDealDmg = false;
}

void AToolHarpoon::OnInteractableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherComp != nullptr && !m_dmgDealt && m_canDealDmg) {
		// This actor has Damageable event
		if (OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass())) {
			IDamageable::Execute_GetDamage(OtherActor, Damage);
			m_dmgDealt = true;
		}
	}
}

void AToolHarpoon::OnInteractableLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherComp != nullptr) {
		// This actor has Damageable event
		if (OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass())) {
		}
	}
}
