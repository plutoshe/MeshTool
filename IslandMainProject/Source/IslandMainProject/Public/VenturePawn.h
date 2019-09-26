// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IslandMainProjectGameModeBase.h"
#include "VenturePawn.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API AVenturePawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVenturePawn();


	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return CameraComp; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UInventoryComponent* GetInventoryComponent() { return InventoryComp; }

	/* ----Useful HEAD
	void InteractWithTool(class AAPInteractItemBase* interactBase);
	*/
protected:
	virtual void BeginPlay() override;
#pragma region PlayerInputFunctions
	void MoveForward(float axis);
	void MoveRight(float axis);
	void Jump();
	void Crouch();
	void UnCrouch();
	void OnMouseClick();
	void OnMouseHold();
	void OnMouseRelease();

	void SwitchToItem1();
	void SwitchToItem2();

#pragma endregion

	void SetNewMoveDestination(const FHitResult& outHit);

	// Navigation component
	class UPathFollowingComponent* m_PFollowComp;
	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
		void MoveTo(AController* i_Controller, const FVector& GoalLocation);
	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
		void CancelMoveToLocation();

	class UPathFollowingComponent* InitNavigationControl(AController& Controller);

#pragma region Interactable
	class AInteractableBase* m_currentInteractable;
	UFUNCTION()
		void OnInteractableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void OnInteractableLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion

#pragma region Tools

protected:

	// Pinter to a game mode class
	UPROPERTY()
		AIslandMainProjectGameModeBase* m_gameMode;

	class AToolMover* m_toolMover;
	UPROPERTY(EditDefaultsOnly, Category = Tool)
		TSubclassOf<class AToolMover> ToolMoverClass;

	UPROPERTY(EditDefaultsOnly, Category = Tool)
	FVector MoverRelativeLocation;
	UPROPERTY(EditDefaultsOnly, Category = Tool)
	FRotator MoverRelativeRotatoin;

	bool m_OnMouseHold;

	void SpawnMover();
	void UpdateMoverTransformation();
#pragma endregion


	/* ----Useful HEAD
	class AAPToolBase* m_usingTool;
	*/
private:
	/** Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComp;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* InteractPointComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCameraControlComponent* CameraControlComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
		class UInventoryComponent* InventoryComp;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
