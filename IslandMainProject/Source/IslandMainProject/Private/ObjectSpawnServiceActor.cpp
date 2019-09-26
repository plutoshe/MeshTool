// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectSpawnServiceActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "PickUpBase.h"

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
		static ConstructorHelpers::FObjectFinder<UBlueprint> AActor(TEXT(WOODPICKUPPATH));
		if (AActor.Object)
		{
			PickUpBlueprints.Add((UClass*)AActor.Object->GeneratedClass);
		}
	}
	{
		static ConstructorHelpers::FObjectFinder<UBlueprint> AActor(TEXT(FISHPICKUPPATH));
		if (AActor.Object)
		{
			PickUpBlueprints.Add((UClass*)AActor.Object->GeneratedClass);
		}
	}
}

void AObjectSpawnServiceActor::PopObject()
{
	TSubclassOf<class AActor> pop;
	ObjectQueue.Dequeue(pop);
	SpawnObject(pop);
}

void AObjectSpawnServiceActor::SpawnObject(TSubclassOf<class AActor> spawnobject)
{
	if (spawnobject)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some actor is spawning"));

			FActorSpawnParameters spawnParams;
			FRotator rotator;
			FVector spawnLocation = this->GetActorLocation();

			AActor* object = world->SpawnActor<AActor>(spawnobject, GetRandomLocation(), rotator, spawnParams);
			m_storedObjects.Add(object);
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

bool AObjectSpawnServiceActor::EnqueueObject(int objectnumber)
{
	if (ObjectQueue.Enqueue(PickUpBlueprints[objectnumber]))
	{
		return true;
	}
	return false;
}

void AObjectSpawnServiceActor::ChangeTimerTime(float time)
{
	GetWorld()->GetTimerManager().ClearTimer(m_spawnTimeHandle);
	m_timerDel.BindUFunction(this, FName("MassSpawnObject"), SpawnAmount, 2);
	GetWorldTimerManager().SetTimer(m_spawnTimeHandle, m_timerDel, time, true);
}

void AObjectSpawnServiceActor::MassSpawnObject(int numberofamount, float spawnlag)
{
	CleanUpStoredObjects();
	if (m_storedObjects.Num() >= MaxExistAmount)
	{
		ChangeTimerTime(3);
		return;
	}
	if (spawnlag < 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Worng spawnlag parameter"));
		return;
	}
	for (int i = 0; i < numberofamount; i++)
	{
		float randomtime = FMath::FRandRange(0, spawnlag);
		int randomitem = FMath::RandRange(0, PickUpBlueprints.Num() -1 );
		SpawnObject(PickUpBlueprints[randomitem]);
	}
	
	ChangeTimerTime(SpawnPeriod);
}

// Called when the game starts or when spawned
void AObjectSpawnServiceActor::BeginPlay()
{
	Super::BeginPlay();

	m_timerDel.BindUFunction(this, FName("MassSpawnObject"), SpawnAmount, 2);
	GetWorldTimerManager().SetTimer(m_spawnTimeHandle, m_timerDel, SpawnPeriod, true, InitialSpawnTiming);
}

void AObjectSpawnServiceActor::CleanUpStoredObjects()
{
	for (int i = 0; i < m_storedObjects.Num(); i++)
	{
			APickUpBase* pickupbase = Cast<APickUpBase>(m_storedObjects[i]);
			if (pickupbase)
			{
				if (!pickupbase->m_bIsValid)
				{
					m_storedObjects.RemoveAt(i);
				}
			}
	}
}

// Called every frame
void AObjectSpawnServiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

