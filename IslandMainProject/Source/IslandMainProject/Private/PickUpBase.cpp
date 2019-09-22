// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/PickUpBase.h"
#include "Components/StaticMeshComponent.h"
#include "Public/InventoryComponent.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Public/VenturePawn.h"

// Sets default values
APickUpBase::APickUpBase()
{
	CapusuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapusuleComponent"));
	CapusuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapusuleComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CapusuleComponent->SetSimulatePhysics(true);
	RootComponent = CapusuleComponent;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapComp"));
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Block);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	OverlapComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	OverlapComp->SetSphereRadius(PickupRadius);
	OverlapComp->SetupAttachment(RootComponent);

	SuperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SuperMesh"));
	SuperMesh->SetupAttachment(RootComponent);
	SuperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_floatingEffectSpeed = 700.f;
	BAbleToPickup = false;
	RespawnTime = 1.4f;

	m_thresholdToCollect = 3;
	m_floatingEffectRange = 30;
	m_floatingEffectFrequency = 3;
}

void APickUpBase::OnPawnEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {
		AVenturePawn* characterBase = Cast<AVenturePawn>(OtherActor);
		if (characterBase && BAbleToPickup)
		{
			// Move to character
			m_owner = characterBase;

			m_bIsGetCollected = true;

			SuperMesh->SetSimulatePhysics(false);
			SuperMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
			SuperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void APickUpBase::AbleToPickUp()
{
	BAbleToPickup = true;

}

void APickUpBase::AddImpulseToOverlapComp(const FVector _dir, float _magnitute)
{
	OverlapComp->AddImpulse(_dir*_magnitute, NAME_None, true);
}

// Called when the game starts or when spawned
void APickUpBase::BeginPlay()
{
	Super::BeginPlay();

	m_floatingEffectOffset = FMath::RandRange(5, 20);
	if (RandomizeScaleOnSpawn) {
		RandomizeScale();
		UStaticMesh* rndMesh = GetRandomMesh();
		if (rndMesh)
			SuperMesh->SetStaticMesh(rndMesh);
	}

	GetWorldTimerManager().SetTimer(m_SpawnHandle, this, &APickUpBase::AbleToPickUp, RespawnTime, false);
}

void APickUpBase::GravitateTowardPlayer(float deltaTime)
{
	FVector dir = m_owner->GetActorLocation() - GetActorLocation();
	SetActorLocation(GetActorLocation() + dir.GetSafeNormal() * m_floatingEffectSpeed * deltaTime);
	float scalepercentage = dir.Size() / OverlapComp->GetUnscaledSphereRadius();
	if(scalepercentage <= 1)
		SetActorScale3D(scalepercentage * FVector(1, 1, 1));


	// The item will disappear once it comes close to a certain distance
	if (dir.Size() < m_thresholdToCollect)
	{
		m_bIsGetCollected = false;
		UInventoryComponent* playerInven = m_owner->GetInventoryComponent();
		if (playerInven && playerInven->AddItem(ItemID, Amount))
		{
			m_owner = nullptr;
			Destroy();
		}
	}
}

void APickUpBase::RandomizeScale()
{
	float rnd = FMath::RandRange(0.9f, 1.3f);
	SetActorScale3D(GetActorScale()*rnd);
	SuperMesh->SetRelativeRotation(SuperMesh->RelativeRotation + FMath::VRand().Rotation());
}

void APickUpBase::SimulateFloatingEffect(float deltaTime)
{
	if (m_floatingEffectRange > 0) {
		FVector NewLoc = SuperMesh->RelativeLocation;

		float deltaZ = (FMath::Sin(m_floatingEffectOffset * m_floatingEffectFrequency + deltaTime) - FMath::Sin(m_floatingEffectOffset * m_floatingEffectFrequency));
		NewLoc.Z += deltaZ * m_floatingEffectRange;
		m_floatingEffectOffset += deltaTime;
		SuperMesh->SetRelativeLocation(NewLoc);
	}
}

UStaticMesh* APickUpBase::GetRandomMesh()
{
	if (m_RandomMeshList.Num() < 1) return nullptr;
	int idx = FMath::RandRange(0, m_RandomMeshList.Num() - 1);

	return m_RandomMeshList[idx];
}

// Called every frame
void APickUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SimulateFloatingEffect(DeltaTime);

	if (BAbleToPickup && m_bIsGetCollected) {
		GravitateTowardPlayer(DeltaTime);
	}
}

