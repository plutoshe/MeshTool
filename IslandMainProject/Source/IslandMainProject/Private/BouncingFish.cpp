// Fill out your copyright notice in the Description page of Project Settings.


#include "BouncingFish.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
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

	MaxHealth = 100;
	m_ImpulsePower = 25;
	m_BounceThreshold = 5;
}

void ABouncingFish::TraceToGround()
{
	FCollisionQueryParams CollisionQueryParam;
	CollisionQueryParam.AddIgnoredActor(this);
	CollisionQueryParam.AddIgnoredComponent(SuperMesh);
	CollisionQueryParam.AddIgnoredComponent(SphereComp);

	FHitResult outHit;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() - FVector::UpVector*1000.f;

	if (GetWorld()->LineTraceSingleByChannel(outHit, Start,
		End, ECC_WorldStatic, CollisionQueryParam)) {

		if (outHit.Distance <= m_BounceThreshold + SphereComp->GetScaledSphereRadius()) {
			BounceToRandomDirection();
		}
	}
}

void ABouncingFish::BounceToRandomDirection()
{
	FVector RNd = FMath::VRand();

	FVector dir = FVector::UpVector;
	dir.X = FMath::RandRange(-0.1f, 0.1f);
	dir.Y = FMath::RandRange(-0.1f, 0.1f);
	FVector impulse = dir * RNd * m_ImpulsePower;
	SphereComp->AddImpulse((impulse + FVector::UpVector * m_ImpulsePower) * 100, NAME_None);
}

// Called when the game starts or when spawned
void ABouncingFish::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABouncingFish::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceToGround();
}

