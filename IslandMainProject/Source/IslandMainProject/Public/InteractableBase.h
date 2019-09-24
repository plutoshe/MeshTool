// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API AInteractableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBase();

	virtual void Interact();
	virtual void UnInteract();

	void EnableInputHint(const FName inputName);
	void DisableInputHint();

	UFUNCTION(BlueprintImplementableEvent, Category = Interactable)
		void BPInteract();
	UFUNCTION(BlueprintImplementableEvent, Category = Interactable)
		void BPUnInteract();

	FORCEINLINE bool GetCanInteract()const { return m_canIntereact; }
	FORCEINLINE void  SetCanInteract(const bool canInteract) { m_canIntereact = canInteract; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool m_canIntereact;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* OverlapComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* WidgetComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
