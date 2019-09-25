// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ISLANDMAINPROJECT_API IDamageable
{
	GENERATED_BODY()

public:
	virtual void TakeDamage(float damage);
	virtual void Die();

	UFUNCTION(BlueprintImplementableEvent, Category = Damageable)
		void BPOnTakeDamage(float damage);
protected:

	int m_currentHealth;
	int MaxHealth;
};
