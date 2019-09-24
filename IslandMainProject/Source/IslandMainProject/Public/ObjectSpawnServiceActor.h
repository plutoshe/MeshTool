// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Containers/Array.h"
#include "Components/BoxComponent.h"
#include "PickUpBase.h"
#include "ObjectSpawnServiceActor.generated.h"

//UENUM(BlueprintType)
//enum class SpawnMode : uint8
//{
//	Auto UMETA(DisplayName = "AutoSpawning"),
//	Manual UMETA(DisplayName = "ManualSpawning")
//};

UCLASS()
class ISLANDMAINPROJECT_API AObjectSpawnServiceActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObjectSpawnServiceActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float SpawnPeriod;

	//There is a bug when you set UPROPERTY to it, so I turn it off
	//UPROPERTY(VisibleAnywhere, BlueprintCallable)
	TQueue<TSubclassOf<APickUpBase>> ResourceQueue;

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void PopResource();

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void SpawnResource(TSubclassOf<class APickUpBase> spawnResource);

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		FVector GetRandomLocation();

	FORCEINLINE class UBoxComponent* GetSpawnArea() const
	{
		return SpawnArea;
	}

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* SpawnArea;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		TArray< TSubclassOf<class APickUpBase>> PickUpBlueprints;

	TSubclassOf<class APickUpBase> m_ResourceObject;
	FTimerDelegate m_timerDel;
	FTimerHandle m_spawnTimeHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};