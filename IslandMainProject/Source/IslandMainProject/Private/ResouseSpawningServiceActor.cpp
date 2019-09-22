// Fill out your copyright notice in the Description page of Project Settings.


#include "ResouseSpawningServiceActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

#define WOODPICKUPPATH "Blueprint'/Game/Environment/PickUpResource/Blurprints/BP_WoodPickUp.BP_WoodPickUp'"
#define FISHPICKUPPATH "Blueprint'/Game/Environment/PickUpResource/Blurprints/BP_FishPickUp.BP_FishPickUp'"

// Sets default values
AResouseSpawningServiceActor::AResouseSpawningServiceActor()
{
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
}

void AResouseSpawningServiceActor::SpawnResource(int resourceid)
{
		UWorld* world = GetWorld();
		if (world)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;

			FRotator rotator;

			FVector spawnLocation = this->GetActorLocation();

			APickUpBase * test = world->SpawnActor<APickUpBase>(PickUpBlueprints[resourceid], spawnLocation, rotator, spawnParams);
		}

		//if (world)
		//{
		//	FActorSpawnParameters spawnParams;
		//	spawnParams.Owner = this;

		//	FRotator rotator;

		//	FVector spawnLocation = this->GetActorLocation();

		//	APickUpBase * test = world->SpawnActor<APickUpBase>(PickUpBlueprints[1], spawnLocation, rotator, spawnParams);
		//}
}

// Called when the game starts or when spawned
void AResouseSpawningServiceActor::BeginPlay()
{
	Super::BeginPlay();
	//SpawnResource();

	m_timerDel.BindUFunction(this, FName("SpawnResource"), m_resourceID);
	GetWorldTimerManager().SetTimer(m_spawnTimeHandle, m_timerDel, 5, true);
}

// Called every frame
void AResouseSpawningServiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

