// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToolMover.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EToolState : uint8
{
	TS_Idle UMETA(DisplayName = "Idle"),
	TS_Ready UMETA(DisplayName = "ReadyToUse"),
	TS_Using UMETA(DisplayName = "Using")
};

UCLASS()
class ISLANDMAINPROJECT_API AToolMover : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AToolMover();

	UFUNCTION(BlueprintCallable, Category = Tools)
	void ToogleToolReady(bool ready);
	UFUNCTION(BlueprintCallable, Category = Tools)
	void UseTool();
	UFUNCTION(BlueprintCallable, Category = Tools)
	void UseToolFinish();

	UFUNCTION(BlueprintImplementableEvent, Category = Tools)
		void BPGetToolReady(FName toolID);
	UFUNCTION(BlueprintImplementableEvent, Category = Tools)
		void BPDisconnectTool();
	UFUNCTION(BlueprintImplementableEvent, Category = Tools)
		void BPUseTool(FName toolID);
	UFUNCTION(BlueprintImplementableEvent, Category = Tools)
		void BPUseToolFinish();
	UFUNCTION(BlueprintImplementableEvent, Category = Tools)
		void BPSwitchTool(FName toolID);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* ContainerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* SuperMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* AttacherComp;

	UPROPERTY(VisibleAnywhere, Category = Tools, meta = (AllowPrivateAccess = "true"))
		EToolState m_currentState;

	void SwitchToMode(EToolState newState);

	void SpawnToolsAtBeginning();

	class AToolBase* m_currentTool;
	TArray<class AToolBase*> m_cachedToolList;
	bool m_ToolReady;
	bool m_Using;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SwitchToTool(FName toolID);
};
