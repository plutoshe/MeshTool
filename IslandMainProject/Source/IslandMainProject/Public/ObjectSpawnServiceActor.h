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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float SpawnPeriod = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float InitialSpawnTiming = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int SpawnAmountAtOneTime = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int MaxExistAmount = 5;

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void PopObject();

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void SpawnObject(TSubclassOf<class AActor> spawnobject);

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		FVector GetRandomLocation();

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		bool EnqueueObject(int objectnumber);

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void ChangeTimerTime(float frequency);

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void MassSpawnObject(int numberofamount, float spawnlag);

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
		TArray< TSubclassOf<class AActor>> PickUpBlueprints;

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void CleanUpStoredObjects();

	class TArray<AActor*> m_storedObjects;

	// Queue for object spawning
	TQueue<TSubclassOf<AActor>> ObjectQueue;

	FTimerDelegate m_timerDel;
	FTimerHandle m_spawnTimeHandle;

	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
