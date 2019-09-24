// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectSpawnServiceActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

#define WOODPICKUPPATH "Blueprint'/Game/Environment/PickUpResource/Blurprints/BP_WoodPickUp.BP_WoodPickUp'"
#define FISHPICKUPPATH "Blueprint'/Game/Environment/PickUpResource/Blurprints/BP_FishPickUp.BP_FishPickUp'"

// Sets default values
AObjectSpawnServiceActor::AObjectSpawnServiceActor()
{
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetSimulatePhysics(false);
	SpawnArea->SetupAttachment(RootComponent);

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	{
		static ConstructorHelpers::FObjectFinder<UBlueprint> APickUpBase(TEXT(WOODPICKUPPATH));
		if (APickUpBase.Object)
		{
			PickUpBlueprints.Add((UClass*)APickUpBase.Object->GeneratedClass);
		}
	}
	{
		static ConstructorHelpers::FObjectFinder<UBlueprint> APickUpBase(TEXT(FISHPICKUPPATH));
		if (APickUpBase.Object)
		{
			PickUpBlueprints.Add((UClass*)APickUpBase.Object->GeneratedClass);
		}
	}

	ResourceQueue.Enqueue(PickUpBlueprints[0]);
	ResourceQueue.Enqueue(PickUpBlueprints[0]);
	ResourceQueue.Enqueue(PickUpBlueprints[1]);
}

void AObjectSpawnServiceActor::PopResource()
{
	TSubclassOf<class APickUpBase> pop;
	ResourceQueue.Dequeue(pop);
	SpawnResource(pop);
}

void AObjectSpawnServiceActor::SpawnResource(TSubclassOf<class APickUpBase> spawnresource)
{
	if (spawnresource)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some actor is spawning"));

			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;

			FRotator rotator;

			FVector spawnLocation = this->GetActorLocation();

			APickUpBase* test = world->SpawnActor<APickUpBase>(spawnresource, GetRandomLocation(), rotator, spawnParams);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("There is no item to spawn from spawning service actor"));
	}
}

FVector AObjectSpawnServiceActor::GetRandomLocation()
{
	FVector area = SpawnArea->GetScaledBoxExtent();
	float spawnx = FMath::RandRange(-1 * area.X, area.X);
	float spawny = FMath::RandRange(-1 * area.Y, area.Y);

	FVector spawnpoint = GetActorLocation() + FVector(spawnx, spawny, 0);

	return spawnpoint;
}

// Called when the game starts or when spawned
void AObjectSpawnServiceActor::BeginPlay()
{
	Super::BeginPlay();
	//SpawnResource();

	//m_timerDel.BindUFunction(this, FName("SpawnResource"), m_resouceObject);
	//GetWorldTimerManager().SetTimer(m_spawnTimeHandle, m_timerDel, 5, true);
	GetWorldTimerManager().SetTimer(m_spawnTimeHandle, this, &AObjectSpawnServiceActor::PopResource, 5, false);
}

// Called every frame
void AObjectSpawnServiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

