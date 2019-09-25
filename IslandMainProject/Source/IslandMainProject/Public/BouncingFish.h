// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GameFramework/Actor.h"
#include "BouncingFish.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API ABouncingFish : public AActor, public IDamageable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABouncingFish();



protected:

#pragma region GroundMovement
	void TraceToGround();
	void BounceToRandomDirection();
	float m_BounceThreshold;
	UPROPERTY(EditAnywhere, Category = FishGroundMovement)
		float m_ImpulsePower;

#pragma endregion

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SuperMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* SphereComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
