// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "AMeshGenerator.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API AAMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAMeshGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PostActorCreated() override;
	void PostLoad() override;
	
public:	
	UFUNCTION(BlueprintCallable, Category = "Tool|MeshGeneration")
		void AddMesh(UProceduralMeshComponent* i_addMesh, FTransform i_transform);
	UFUNCTION(BlueprintCallable, Category = "Tool|MeshGeneration")
		void Smooth();
	UFUNCTION(BlueprintCallable, Category = "Tool|MeshGeneration")
		void Division();

	void AddMeshSection(int i_id, const FProcMeshSection& i_src, const FTransform& i_transform);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		int m_insertMode;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		int m_blockID;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		int m_iteration;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		int m_addSmoothNum;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_percision;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_alpha;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float m_beta;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		bool m_meshConnectionSmooth;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		UProceduralMeshComponent* m_mesh;
};
