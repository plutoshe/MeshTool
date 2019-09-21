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
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapComp"));
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Block);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	OverlapComp->SetSimulatePhysics(true);

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &APickUpBase::OnPawnEnter);

	OverlapComp->SetSphereRadius(200.f);

	RootComponent = OverlapComp;

	SuperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SuperMesh"));
	SuperMesh->SetupAttachment(RootComponent);
	SuperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_MoveSpeed = 700.f;
	m_bCanMoveToPlayer = false;
	m_timeAvoidPickUpAfterSpawn = 1.4f;

	m_thresholdToCollect = 3;
	m_floatingEffectDistance = 30;
	m_floatingEffectSpeed = 3;
}

void APickUpBase::OnPawnEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {
		AVenturePawn* characterBase = Cast<AVenturePawn>(OtherActor);
		if (characterBase && m_bCanMoveToPlayer) {
			// Move to character
			m_owner = characterBase;

			m_bMovingToPlayer = true;

			OverlapComp->SetSimulatePhysics(false);
			OverlapComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			SuperMesh->SetSimulatePhysics(false);
			SuperMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
			SuperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void APickUpBase::AllowToPickUp()
{
	m_bCanMoveToPlayer = true;

}

void APickUpBase::AddImpulseToOverlapComp(const FVector _dir, float _magnitute)
{
	OverlapComp->AddImpulse(_dir*_magnitute, NAME_None, true);
}

// Called when the game starts or when spawned
void APickUpBase::BeginPlay()
{
	Super::BeginPlay();

	m_startTime = FMath::RandRange(5, 20);
	if (m_bRandomizeScaleOnSpawn) {
		RandomizeScale();
		UStaticMesh* rndMesh = GetRandomMesh();
		if (rndMesh)
			SuperMesh->SetStaticMesh(rndMesh);
	}

	GetWorldTimerManager().SetTimer(m_SpawnHandle, this, &APickUpBase::AllowToPickUp, m_timeAvoidPickUpAfterSpawn, false);

}

void APickUpBase::GravitateTowardPlayer(float deltaTime)
{
	FVector dir = m_owner->GetActorLocation() - GetActorLocation();
	SetActorLocation(GetActorLocation() + dir.GetSafeNormal() * m_MoveSpeed * deltaTime);
	SetActorScale3D( (dir.Size()/OverlapComp->GetUnscaledSphereRadius()) * FVector(1, 1, 1));


	// The item will disappear once it comes close to a certain distance
	if (dir.Size() < m_thresholdToCollect)
	{
		m_bMovingToPlayer = false;
		UInventoryComponent* playerInven = m_owner->GetInventoryComponent();
		if (playerInven && playerInven->AddItem(m_ItemID, m_Amount))
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
	if (m_floatingEffectDistance > 0) {
		FVector NewLoc = SuperMesh->RelativeLocation;

		float deltaZ = (FMath::Sin(m_startTime * m_floatingEffectSpeed + deltaTime) - FMath::Sin(m_startTime * m_floatingEffectSpeed));
		NewLoc.Z += deltaZ * m_floatingEffectDistance;
		m_startTime += deltaTime;
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

	if (m_bCanMoveToPlayer && m_bMovingToPlayer) {
		GravitateTowardPlayer(DeltaTime);
	}
}

