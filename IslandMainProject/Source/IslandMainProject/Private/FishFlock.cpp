// Fill out your copyright notice in the Description page of Project Settings.

#include "FishFlock.h"
//#include "FlockingGameMode.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "ConstructorHelpers.h"
#include "ParallelFor.h"
#include "Event.h"
#include "ScopeLock.h"
#include "Engine/StaticMesh.h"

FVector checkMapRange(const FVector& mapMinSize, const FVector& mapMaxSize, const FVector& currentPosition, const FVector& currentVelocity)
{
	FVector newVelocity = currentVelocity;
	if (currentPosition.X > mapMaxSize.X || currentPosition.X < -mapMinSize.X) {
		newVelocity.X *= -1.f;
	}
	if (currentPosition.Y > mapMaxSize.Y || currentPosition.Y < -mapMinSize.Y) {
		newVelocity.Y *= -1.f;
	}

	if (currentPosition.Z > mapMaxSize.Z || currentPosition.Z < -mapMinSize.Z) {
		newVelocity.Z *= -1.f;
	}
	return newVelocity;
}

AFishFlock::AFishFlock()
{
	m_currentStatesIndex = 0;
	m_previousStatesIndex = 1;
	m_isCpuSingleThread = true;
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshOb(TEXT("StaticMesh'/Game/Characters/NPC/seaBass.seaBass'"));
	m_staticMesh = StaticMeshOb.Object;

}

void AFishFlock::OnConstruction(const FTransform& Transform)
{
	if (m_instancedStaticMeshComponent) {
		m_instancedStaticMeshComponent->ClearInstances();
	} else 
	{ 
		m_instancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
		m_instancedStaticMeshComponent->RegisterComponent();
		m_instancedStaticMeshComponent->SetStaticMesh(m_staticMesh);
		m_instancedStaticMeshComponent->SetFlags(RF_Transactional);
		this->AddInstanceComponent(m_instancedStaticMeshComponent);
	}
	

	for (int i = 0; i < m_fishNum; i++) {
		FVector randomPos(
			FMath::RandRange(
				m_mapMinSize.X + (1 - m_spawningRange.X) / 2 * (m_mapMaxSize.X - m_mapMinSize.X) , 
				m_mapMaxSize.X - (1 - m_spawningRange.X) / 2 * (m_mapMaxSize.X - m_mapMinSize.X)),
			FMath::RandRange(
				m_mapMinSize.Y + (1 - m_spawningRange.Y) / 2 * (m_mapMaxSize.Y - m_mapMinSize.Y),
				m_mapMaxSize.Y - (1 - m_spawningRange.Y) / 2 * (m_mapMaxSize.Y - m_mapMinSize.Y)),
			FMath::RandRange(
				m_mapMinSize.Z + (1 - m_spawningRange.Z) / 2 * (m_mapMaxSize.Z - m_mapMinSize.Z),
				m_mapMaxSize.Z - (1 - m_spawningRange.Z) / 2 * (m_mapMaxSize.Z - m_mapMinSize.Z)));
		FRotator randRotator(0, FMath::RandRange(0, 90), 0);

		FTransform t;
		t.SetLocation(randomPos);
		t.SetRotation(randRotator.Quaternion());
		t.SetScale3D(FVector(1, 1, 1));

		int32 instanceId = m_instancedStaticMeshComponent->AddInstance(t);
	}
}

void AFishFlock::BeginPlay()
{
	Super::BeginPlay();
	
	m_fishStates = new FishState * [m_fishNum];
	for (int i = 0; i < m_instancedStaticMeshComponent->GetInstanceCount(); i++) {
		m_fishStates[i] = new FishState[2];
		FishState state;
		FishState stateCopy;
		FTransform transfromOfInitialFish;
		m_instancedStaticMeshComponent->GetInstanceTransform(i, transfromOfInitialFish);
		stateCopy.instanceId = state.instanceId = i;
		stateCopy.position = state.position = transfromOfInitialFish.GetLocation();
		stateCopy.velocity = state.velocity = transfromOfInitialFish.GetRotation().Vector() * FMath::RandRange(100, 400);
		stateCopy.acceleration = state.acceleration = FVector::ZeroVector;
		m_fishStates[i][m_currentStatesIndex] = state;
		m_fishStates[i][m_previousStatesIndex] = stateCopy;
	}
}

void AFishFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_elapsedTime > 0.005f) {
		Calculate(m_fishStates, DeltaTime, m_isCpuSingleThread);

		m_elapsedTime = 0.f;
	}
	else {
		m_elapsedTime += DeltaTime;
	}
}

void AFishFlock::Calculate(FishState**& agents, float DeltaTime, bool isSingleThread)
{
	/*if (tmp == 1) return;
	tmp++;*/
	

	float kCoh = m_kCohesion, kSep = m_kSeparation, kAlign = m_kAlignment;
	float rCohesion = m_radiusCohesion, rSeparation = m_radiusSeparation, rAlignment = m_radiusAlignment;
	float maxAccel = m_maxAcceleration;
	float maxVel = m_maxVelocity;
	FVector mapSzMax = m_mapMaxSize;
	FVector mapSzMin = m_mapMinSize;
	const int32 fishNum = m_fishNum;

	int currentStatesIndex = m_currentStatesIndex;
	int previousStatesIndex = m_previousStatesIndex;
//LogTemp: 0 acc : X = 27.487 Y = 76.088 Z = 7.785, speed : X = 117.139 Y = 121.371 Z = -2.611, position : X = -257.862 Y = -850.633 Z = 14.620
	//LogTemp: 0 acc: X=49.830 Y=109.210 Z=7.291 , speed: X=-117.554 Y=-122.281 Z=2.551, position: X=-256.882 Y=-849.614 Z=14.599
	ParallelFor(fishNum,
		[&agents, fishNum, currentStatesIndex, previousStatesIndex, kCoh, kSep, kAlign, rCohesion, rSeparation, rAlignment, maxAccel, maxVel, mapSzMax, mapSzMin, DeltaTime, isSingleThread](int32 fishID)
		{
			FVector cohesion(FVector::ZeroVector), separation(FVector::ZeroVector), alignment(FVector::ZeroVector);
			int32 cohesionCnt = 0, separationCnt = 0, alignmentCnt = 0;
			for (int i = 0; i < fishNum; i++) {
				if (i != fishID) {
					float distance = FVector::Distance(agents[i][previousStatesIndex].position, agents[fishID][previousStatesIndex].position);
					if (distance > 0) {
						if (distance < rCohesion) {
							cohesion += agents[i][previousStatesIndex].position;
							cohesionCnt++;
						}
						if (distance < rSeparation) {
							separation += agents[i][previousStatesIndex].position - agents[fishID][previousStatesIndex].position;
							separationCnt++;
						}
						if (distance < rAlignment) {
							alignment += agents[i][previousStatesIndex].velocity;
							alignmentCnt++;
						}
					}
				}
			}

			if (cohesionCnt != 0) {
				cohesion /= cohesionCnt;
				cohesion -= agents[fishID][previousStatesIndex].position;
				cohesion.Normalize();
			}

			if (separationCnt != 0) {
				separation /= separationCnt;
				separation *= -1.f;
				separation.Normalize();
			}

			if (alignmentCnt != 0) {
				alignment /= alignmentCnt;
				alignment.Normalize();
			}

			agents[fishID][currentStatesIndex].acceleration = (cohesion * kCoh + separation * kSep + alignment * kAlign).GetClampedToMaxSize(maxAccel);
			UE_LOG(LogTemp, Log, TEXT("%d acc: %s , speed: %s, position: %s"), 
				fishID , 
				*(agents[fishID][currentStatesIndex].acceleration.ToString()), 
				*(agents[fishID][currentStatesIndex].velocity.ToString()),
				*(agents[fishID][currentStatesIndex].position.ToString()));
			//agents[fishID][currentStatesIndex].acceleration.Z = 0;

			agents[fishID][currentStatesIndex].velocity = agents[fishID][1 - currentStatesIndex].velocity + agents[fishID][currentStatesIndex].acceleration * DeltaTime;
			agents[fishID][currentStatesIndex].velocity = agents[fishID][currentStatesIndex].velocity.GetClampedToMaxSize(maxVel);
			agents[fishID][currentStatesIndex].position = agents[fishID][1 - currentStatesIndex].position + agents[fishID][currentStatesIndex].velocity * DeltaTime;
			agents[fishID][currentStatesIndex].velocity = checkMapRange(mapSzMin, mapSzMax, agents[fishID][currentStatesIndex].position, agents[fishID][currentStatesIndex].velocity);
			}, isSingleThread);
	
	//for (int i = 0; i < fishNum; i++) {
	//	FHitResult hit(ForceInit);
	//	if (collisionDetected(agents[i][previousStatesIndex].position, agents[i][currentStatesIndex].position, hit)) {
	//		agents[i][currentStatesIndex].position -= agents[i][currentStatesIndex].velocity * DeltaTime;
	//		agents[i][currentStatesIndex].velocity *= -1.0;
	//		agents[i][currentStatesIndex].position += agents[i][currentStatesIndex].velocity * DeltaTime;
	//	}
	//}

	for (int i = 0; i < m_fishNum; i++) {
		FTransform transform;
		m_instancedStaticMeshComponent->GetInstanceTransform(m_fishStates[i][m_currentStatesIndex].instanceId, transform);
		transform.SetLocation(m_fishStates[i][m_currentStatesIndex].position);
		FVector direction = m_fishStates[i][m_currentStatesIndex].velocity;
		direction.Normalize();
		transform.SetRotation(FRotationMatrix::MakeFromX(direction).Rotator().Add(0.f, -90.f, 0.f).Quaternion());
		m_instancedStaticMeshComponent->UpdateInstanceTransform(m_fishStates[i][m_currentStatesIndex].instanceId, transform, false, true);
	}
	swapFishStatesIndexes();
}

bool AFishFlock::collisionDetected(const FVector& start, const FVector& end, FHitResult& hitResult)
{
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	//RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	return GetWorld()->SweepSingleByChannel(hitResult, start, end, FQuat(), ECC_WorldStatic, FCollisionShape::MakeSphere(200), RV_TraceParams);
}

void AFishFlock::swapFishStatesIndexes()
{
	int32 tmp = m_currentStatesIndex;
	m_currentStatesIndex = m_previousStatesIndex;
	m_previousStatesIndex = tmp;
}