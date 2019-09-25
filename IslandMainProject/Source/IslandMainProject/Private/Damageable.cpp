// Fill out your copyright notice in the Description page of Project Settings.


#include "Damageable.h"

// Add default functionality here for any IDamageable functions that are not pure virtual.

void IDamageable::TakeDamage(float damage)
{
	int actualDamage;
	if (m_currentHealth > damage) {
		actualDamage = damage;
	}
	else {
		actualDamage = m_currentHealth;
	}
	m_currentHealth -= actualDamage;

	BPOnTakeDamage(actualDamage);
	if (m_currentHealth <= 0) {
		Die();
	}
}

void IDamageable::Die()
{
}
