// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpBase.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API APickUpBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpBase();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName m_ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText m_Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText m_Describtion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_bCanBeUsed;

	FORCEINLINE class UStaticMeshComponent* GetStaticMesh() const { return SuperMesh; }

protected:

	// Overlap Function for going to player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* OverlapComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SuperMesh;

	UPROPERTY(EditDefaultsOnly, Category = Component)
		TArray<class UStaticMesh*> m_RandomMeshList;

	UFUNCTION()
		void OnPawnEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

#pragma region SpawnFunctions
protected:
	FTimerHandle m_SpawnHandle;
	float m_timeAvoidPickUpAfterSpawn;
	void AllowToPickUp();

public:
	void AddImpulseToOverlapComp(const FVector _dir, float _magnitute);

#pragma endregion
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_bCanMoveToPlayer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float m_MoveSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool m_bRandomizeScaleOnSpawn;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	class AVenturePawn* m_InsideCharacter;
	bool m_bMovingToPlayer;

	float m_startTime;
	UPROPERTY(EditDefaultsOnly)
		float m_ThresholdToDestroy;
	UPROPERTY(EditDefaultsOnly)
		float m_floatDistance;
	UPROPERTY(EditDefaultsOnly)
		float m_floatSpeed;


	UFUNCTION(BlueprintCallable, Category = "Utils")
		void StartMoveToPlayer(AVenturePawn* m_InsideCharacter);

	void RandomizeScale();
	void SimulateFloat(float _deltaTime);

	class UStaticMesh* GetRandomMesh();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
