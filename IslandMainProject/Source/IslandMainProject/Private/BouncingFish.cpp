// Fill out your copyright notice in the Description page of Project Settings.
#ifndef IMPULSE_SCALAR
#define IMPULSE_SCALAR 100
#endif // !IMPULSE_SCALAR

#include "BouncingFish.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Public/InventoryComponent.h"

// Sets default values
ABouncingFish::ABouncingFish()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetSimulatePhysics(true);


	SuperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SuperMesh"));
	SuperMesh->SetupAttachment(RootComponent);
	SuperMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusBar"));
	WidgetComp->SetupAttachment(RootComponent);

	LootComp = CreateDefaultSubobject <UInventoryComponent>(TEXT("LootComp"));

	m_maxHealth = 100;
	m_currentHealth = m_maxHealth;
	ImpulsePower = 100;
	TailMoveSpeed = 10;
	MaxSwingAngle = 30;
	HorizontalBounceScalar = 0.1f;
	m_BounceDistanceThreshold = 5;
}

void ABouncingFish::TraceToGround()
{
	FCollisionQueryParams CollisionQueryParam;
	CollisionQueryParam.AddIgnoredActor(this);
	CollisionQueryParam.AddIgnoredComponent(SuperMesh);
	CollisionQueryParam.AddIgnoredComponent(SphereComp);

	FHitResult outHit;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() - FVector::UpVector * 1000.f;

	if (GetWorld()->LineTraceSingleByChannel(outHit, Start,
		End, ECC_WorldStatic, CollisionQueryParam)) {

		if (outHit.Distance <= m_BounceDistanceThreshold + SphereComp->GetScaledSphereRadius()) {
			BounceToRandomDirection();
		}
	}
}

void ABouncingFish::BounceToRandomDirection()
{
	FVector dir = FVector::UpVector;
	dir.X = FMath::RandRange(-1.f, 1.f) * HorizontalBounceScalar;
	dir.Y = FMath::RandRange(-1.f, 1.f) * HorizontalBounceScalar;
	FVector impulse = dir * ImpulsePower;
	SphereComp->AddImpulse(impulse * IMPULSE_SCALAR, NAME_None);
}


void ABouncingFish::GetDamage_Implementation(float damage)
{
	float actualDmg;
	if (damage > m_currentHealth)
		actualDmg = m_currentHealth;
	else actualDmg = damage;
	m_currentHealth -= actualDmg;
	BPOnGetdamage();
	if (m_currentHealth <= 0)
		Die();
}

void ABouncingFish::Die()
{
	DropAllItems();
	Destroy();
}

void ABouncingFish::DropAllItems()
{
	LootComp->ThrowAllItems();
}

float ABouncingFish::GetPercentageOfHealth()
{
	return (float) m_currentHealth / m_maxHealth;
}

// Called when the game starts or when spawned
void ABouncingFish::BeginPlay()
{
	Super::BeginPlay();

}

void ABouncingFish::RotateBodyYaw(float t)
{
	m_startTime += t;
	FRotator newRot = SuperMesh->RelativeRotation;
	newRot.Yaw = MaxSwingAngle * FMath::Sin(TailMoveSpeed * m_startTime);
	SuperMesh->SetRelativeRotation(newRot);
}

// Called every frame
void ABouncingFish::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceToGround();
	RotateBodyYaw(DeltaTime);
}

