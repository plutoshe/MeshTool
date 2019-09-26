// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Classes/GameFramework/Actor.h"
#include "Engine/Classes/Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "PickUpBase.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API APickUpBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool BCanBeUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PickupRadius = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RespawnTime;

	FORCEINLINE class UStaticMeshComponent* GetStaticMesh() const { return SuperMesh; }

	FORCEINLINE class USphereComponent* GetSphereComponent() const { return OverlapComp; }

	FORCEINLINE class UCapsuleComponent* GetRootCompoennt() const { return CapusuleComponent; }

protected:

	// Root Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* CapusuleComponent;

	// Overlap Function for going to player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* OverlapComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SuperMesh;

	UPROPERTY(EditDefaultsOnly, Category = Component)
		TArray<class UStaticMesh*> m_RandomMeshList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool BAbleToPickup;

	// The pick up is get collected right now
	bool m_bIsGetCollected;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool RandomizeScaleOnSpawn;

	UFUNCTION()
		void OnPawnEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void GravitateTowardPlayer(float deltaTime);

	UFUNCTION()
		void RandomizeScale();

	UFUNCTION()
		UStaticMesh* GetRandomMesh();

	UFUNCTION()
		void AbleToPickUp();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region FloatingEffect
	// The timing offset for floating effect
	float m_floatingEffectOffset;
	// The thresh hold for getting picked up
	float m_thresholdToCollect;
	
	float m_floatingEffectRange;

	float m_floatingEffectSpeed;

	float m_floatingEffectFrequency;

	void SimulateFloatingEffect(float deltaTime);
#pragma endregion

private:

	class AVenturePawn* m_owner;

	FTimerHandle m_spawnHandle;

public:

	void AddImpulseToOverlapComp(const FVector _dir, float _magnitute);

	//Check if the component is destroied.
	bool m_bIsValid;

	virtual void Tick(float DeltaTime) override;

};
