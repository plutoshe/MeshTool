// Fill out your copyright notice in the Description page of Project Settings.


#include "ResouseSpawningServiceActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
AResouseSpawningServiceActor::AResouseSpawningServiceActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> APickUpBase(TEXT("Blueprint'/Game/Environment/PickUpResource/Blurprints/BP_WoodPickUp.BP_WoodPickUp'"));
	if (APickUpBase.Object)
	{
		ThisBlueprint = (UClass*)APickUpBase.Object->GeneratedClass;
	}
}

void AResouseSpawningServiceActor::SpawnResource()
{
		UWorld* world = GetWorld();
		if (world)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;

			FRotator rotator;

			FVector spawnLocation = this->GetActorLocation();

			APickUpBase * test = world->SpawnActor<APickUpBase>(ThisBlueprint, spawnLocation, rotator, spawnParams);
		}
}

// Called when the game starts or when spawned
void AResouseSpawningServiceActor::BeginPlay()
{
	Super::BeginPlay();
	SpawnResource();
}

// Called every frame
void AResouseSpawningServiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

