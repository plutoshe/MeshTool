// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Containers/Array.h"
#include "Components/BoxComponent.h"
#include "PickUpBase.h"
#include "ResouseSpawningServiceActor.generated.h"

UENUM(BlueprintType)
enum class SpawnMode : uint8
{
	Auto UMETA(DisplayName = "AutoSpawning"),
	Manual UMETA(DisplayName = "ManualSpawning")
};

UCLASS()
class ISLANDMAINPROJECT_API AResouseSpawningServiceActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AResouseSpawningServiceActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float SpawnPeriod;

	//UPROPERTY(VisibleAnywhere, BlueprintCallable)
	//	UClass TQueue<TSubclassOf<APickUpBase>*> ResouseQueue;

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void SpawnResource(int resourceid);

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		FVector GetRandomLocation();

	FORCEINLINE class UBoxComponent* GetSpawnArea() const { return SpawnArea; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* SpawnArea;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		TArray< TSubclassOf<class APickUpBase>> PickUpBlueprints;

	int m_resourceID = 1;
	FTimerDelegate m_timerDel;
	FTimerHandle m_spawnTimeHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
