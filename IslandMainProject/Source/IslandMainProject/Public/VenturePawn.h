// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	/* ----Useful HEAD
	FORCEINLINE class UInventoryComponent* GetInventoryComponent() { return InventoryComp; }
	*/

	FORCEINLINE  FVector GetInteractPointLocation() { return InteractPointComp->GetComponentLocation(); }
	FORCEINLINE  FRotator GetInteractPointRotation() { return InteractPointComp->GetComponentRotation(); }
	/* ----Useful HEAD
	void InteractWithTool(class AAPInteractItemBase* interactBase);
	*/
protected:
	virtual void BeginPlay() override;
#pragma region PlayerInputFunctions
	void MoveForward(float axis);
	void MoveRight(float axis);
	void RotateCamera90Clockwise();
	void RotateCamera90CounterClockwise();
	void Jump();
	void Crouch();
	void UnCrouch();
	/* ----Useful HEAD
	void OnMouseClick();
	*/
#pragma endregion

	bool m_bRotating;
	FRotator m_DestRotator;
	UPROPERTY(EditDefaultsOnly, Category = Camera)
		float m_CameraRotateSpeed;

	void PerformCameraRotation(float DeltaSeconds);

	/* ----Useful HEAD
	void SetNewMoveDestination(const FHitResult& outHit);
	*/
	// Navigation component
	class UPathFollowingComponent* m_PFollowComp;
	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
		void MoveTo(AController* i_Controller, const FVector& GoalLocation);
	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
		void CancelMoveToLocation();

	class UPathFollowingComponent* InitNavigationControl(AController& Controller);

	/* ----Useful HEAD

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
		class UInventoryComponent* InventoryComp;
	class AAPToolBase* m_usingTool;
	*/
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComp;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* InteractPointComp;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
