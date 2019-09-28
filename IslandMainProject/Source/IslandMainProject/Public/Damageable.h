// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Damageable)
		void GetDamage(float damage);
		
	UFUNCTION(BlueprintImplementableEvent, Category = Damageable)
		void BPOnGetdamage();
	void Die();


protected:

	int m_currentHealth;
	int m_maxHealth;
};
