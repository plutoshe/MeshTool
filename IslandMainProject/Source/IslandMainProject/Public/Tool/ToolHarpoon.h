// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolBase.h"
#include "ToolHarpoon.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API AToolHarpoon : public AToolBase
{
	GENERATED_BODY()
public:
	AToolHarpoon();

protected:

	UPROPERTY(EditDefaultsOnly, Category = HarpoonTool)
		float Damage;

	// Parent functions
	void StartUseTool() override;
	void UseToolFinish() override;

	UFUNCTION()
	virtual	void OnInteractableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UFUNCTION()
	virtual	void OnInteractableLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	bool m_dmgDealt = false;
	bool m_canDealDmg = false;
};
