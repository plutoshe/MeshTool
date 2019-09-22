// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FishFlock.generated.h"

struct FishState {
	int32 instanceId;
	FVector position;
	FVector velocity;
	FVector acceleration;
};

UCLASS()
class ISLANDMAINPROJECT_API AFishFlock : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFishFlock();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		int32 m_fishNum = 100;            // total instances of fish
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		FVector m_mapMaxSize = FVector(2000, 2000, 2000);          // size of area where fish can flock
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		FVector m_mapMinSize = FVector(-2000, -2000, -100);          // size of area where fish can flock
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		FVector m_spawningRange = FVector(0.5f, 0.5f, 0.8f);		// spawning range in persents of map size
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_maxVelocity = 1500.f;        // maximum velocity of fish
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_maxAcceleration = 1500.f;    // maximum acceleration of fish
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_radiusCohesion = 1000.f;     // Cohesion radius. The radius inside which the fish will tend to inside the circle (approach) 
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_radiusSeparation = 250.f;   // Separation radius. The radius within which the fish will tend to avoid collisions 
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_radiusAlignment = 240.f;    // Alignment radius. The radius inside which the fish will tend to follow in one direction
	// Gain factors for the three types of fish behavior. By default  all three gain factors are equals 1.0f
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_kCohesion = 100.f;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_kSeparation = 1.f;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_kAlignment = 20.f;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		UStaticMesh* m_staticMesh;

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		FVector m_MainPoint;

protected:
	void Calculate(FishState**& agents, float DeltaTime, bool isSingleThread);
	bool collisionDetected(const FVector& start, const FVector& end, FHitResult& hitResult);
	void swapFishStatesIndexes();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		// Fish instanced static mesh component. This component contains all of the fish instances on the scene
		UInstancedStaticMeshComponent* m_instancedStaticMeshComponent;	
private:
	// Array of fish states if flocking behaviour calculates on CPU
	FishState** m_fishStates;

	// index of fish states array where stored current states of fish
	int32 m_currentStatesIndex;

	// index of fish states array where stored previous states of fish
	int32 m_previousStatesIndex;

	// Single or multithreaded algorithm. CPU only.
	bool m_isCpuSingleThread;

	// time elapsed from last calculation
	float m_elapsedTime = 0.f;
	
	// Fish static mesh object
};
