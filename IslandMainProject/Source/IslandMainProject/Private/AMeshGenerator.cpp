// Fill out your copyright notice in the Description page of Project Settings.

#include "AMeshGenerator.h"
#include "Public/Tool/GeometryUtility.h"
#include  "Engine/StaticMesh.h"

class sortVertex {
public:
	FVector data;
	int index;
	sortVertex() : data(), index(0) {}
	sortVertex(FVector i_data, int i_index) : data(i_data), index(i_index) {}
};

// Sets default values
AAMeshGenerator::AAMeshGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = m_mesh;
	// New in UE 4.17, multi-threaded PhysX cooking.
	m_mesh->bUseAsyncCooking = true;
	m_mesh->ClearAllMeshSections();
	
}

// Called when the game starts or when spawned
void AAMeshGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAMeshGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAMeshGenerator::PostActorCreated()
{
	Super::PostActorCreated();
}

// This is called when actor is already in level and map is opened
void AAMeshGenerator::PostLoad()
{
	Super::PostLoad();
	//CreateTriangle();
}

void AAMeshGenerator::AddMeshSection(int i_id, const FProcMeshSection& i_src, const FTransform& i_transform)
{
	TArray<FVector> vertices;
	TArray<int32> Triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;
	vertices.Empty();
	Triangles.Empty();
	tangents.Empty();
	normals.Empty();
	UV0.Empty();
	vertexColors.Empty();
	for (int j = 0; j < i_src.ProcVertexBuffer.Num(); j++)
	{
		FVector pos = i_src.ProcVertexBuffer[j].Position * i_transform.GetScale3D(); // scale
		pos = i_transform.GetRotation().Rotator().RotateVector(pos);
		pos += i_transform.GetLocation();
		vertices.Add(pos);
		tangents.Add(i_src.ProcVertexBuffer[j].Tangent);
		normals.Add(i_src.ProcVertexBuffer[j].Normal);
		UV0.Add(i_src.ProcVertexBuffer[j].UV0);
		vertexColors.Add(i_src.ProcVertexBuffer[j].Color);
	}
	for (int j = 0; j < i_src.ProcIndexBuffer.Num(); j++)
	{
		Triangles.Add(i_src.ProcIndexBuffer[j]);
	}

	m_mesh->CreateMeshSection_LinearColor(i_id, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
}

void AAMeshGenerator::AddMesh(UProceduralMeshComponent* i_addMesh, FTransform i_transform)
{
	if (i_addMesh == nullptr)
	{
		return;
	}
	int existingMeshNum = m_mesh->GetNumSections();
	//UE_LOG(LogTemp, Log, TEXT("%s"), *(i_transform.GetTranslation().ToString()));
	for (int i = 0; i < i_addMesh->GetNumSections(); i++)
	{
		AddMeshSection(existingMeshNum + i, *(i_addMesh->GetProcMeshSection(i)), i_transform);
		//mesh->CreateMeshSection_LinearColor(existingMeshNum + i, , i_addMesh->GetProcMeshSection(i)->ProcIndexBuffer, normals, UV0, vertexColors, tangents, true);

	}
	FProcMeshSection finalMesh = *(m_mesh->GetProcMeshSection(0));
	for (int i = 1; i < m_mesh->GetNumSections(); i++)
	{
		FProcMeshSection addedMesh = *(m_mesh->GetProcMeshSection(i));
		FProcMeshSection resultA, resultB;
		TArray<int> planeAStatus;
		TArray<int> planeBStatus;
		planeAStatus.Init(0, addedMesh.ProcIndexBuffer.Num() / 3);
		planeBStatus.Init(0, finalMesh.ProcIndexBuffer.Num() / 3);
		GeometryUtility::MeshSectionIntersection(addedMesh, finalMesh, resultA, planeAStatus, planeBStatus, 0);
		GeometryUtility::MeshSectionIntersection(finalMesh, addedMesh, resultB, planeBStatus, planeAStatus, 0);
		GeometryUtility::MeshSectionIntersection(addedMesh, finalMesh, resultA, planeAStatus, planeBStatus, 1);
		GeometryUtility::MeshSectionIntersection(finalMesh, addedMesh, resultB, planeBStatus, planeAStatus, 1);
		finalMesh.ProcIndexBuffer.Empty();
		finalMesh.ProcVertexBuffer.Empty();
		addedMesh.ProcVertexBuffer.Empty();
		addedMesh.ProcIndexBuffer.Empty();
		addedMesh.ProcVertexBuffer.Append(resultA.ProcVertexBuffer);
		addedMesh.ProcVertexBuffer.Append(resultB.ProcVertexBuffer);
		TArray<sortVertex> p;
		p.Empty();
		p.Init(sortVertex(), 0);
		//p.Init(0, finalMesh.ProcVertexBuffer.Num());
		for (int pi = 0; pi < addedMesh.ProcVertexBuffer.Num(); pi++)
		{
			p.Add(sortVertex(
				addedMesh.ProcVertexBuffer[pi].Position,
				pi));
		}
		p.Sort([](const sortVertex& a, const sortVertex& b) {
			if (GeometryUtility::eps(a.data.X - b.data.X) > 0)
			{
				return true;
			}
			if (GeometryUtility::eps(a.data.X - b.data.X) == 0 && GeometryUtility::eps(a.data.Y - b.data.Y) > 0)
			{
				return true;
			}
			if (GeometryUtility::eps(a.data.X - b.data.X) == 0 && GeometryUtility::eps(a.data.Y - b.data.Y) == 0 && GeometryUtility::eps(a.data.Z - b.data.Z) > 0)
			{
				return true;
			}
			return false;
			});
		TArray<int32> rIndex, vertexPlaneStatus;
		rIndex.Init(0, p.Num());
		vertexPlaneStatus.Init(0, p.Num());
		int32 newIndexNum = 0;
		rIndex[p[0].index] = 0;
		finalMesh.ProcVertexBuffer.Add(addedMesh.ProcVertexBuffer[p[0].index]);
		vertexPlaneStatus[0] |= 1 << (int)((p[0].index >= resultA.ProcVertexBuffer.Num()));
		for (int pi = 1; pi < p.Num(); pi++)
		{
			if (GeometryUtility::eps(p[pi].data.X - p[pi - 1].data.X) != 3 &&
				GeometryUtility::eps(p[pi].data.Y - p[pi - 1].data.Y) != 3 &&
				GeometryUtility::eps(p[pi].data.Z - p[pi - 1].data.Z) != 3)
			{
				newIndexNum++;
				finalMesh.ProcVertexBuffer.Add(addedMesh.ProcVertexBuffer[p[pi].index]);
			}			
			vertexPlaneStatus[newIndexNum] |= 1 << (int)((p[pi].index >= resultA.ProcVertexBuffer.Num()));
			rIndex[p[pi].index] = newIndexNum;
		}

		if (m_insertMode != 1)
		{
			for (int j = 0; j < resultA.ProcIndexBuffer.Num(); j += 3)
			{
				if ((planeAStatus[resultA.ProcIndexBuffer[j]] == 3 ||
					planeAStatus[resultA.ProcIndexBuffer[j + 1]] == 3 ||
					planeAStatus[resultA.ProcIndexBuffer[j + 2]] == 3) ||
					(vertexPlaneStatus[rIndex[resultA.ProcIndexBuffer[j]]] == 3 &&
						vertexPlaneStatus[rIndex[resultA.ProcIndexBuffer[j + 1]]] == 3 &&
						vertexPlaneStatus[rIndex[resultA.ProcIndexBuffer[j + 2]]] == 3))
				{
					finalMesh.ProcIndexBuffer.Add(rIndex[resultA.ProcIndexBuffer[j]]);
					finalMesh.ProcIndexBuffer.Add(rIndex[resultA.ProcIndexBuffer[j + 1]]);
					finalMesh.ProcIndexBuffer.Add(rIndex[resultA.ProcIndexBuffer[j + 2]]);
				}
			}
		}
		if (m_insertMode != 2)
		{
			for (int j = 0; j < resultB.ProcIndexBuffer.Num(); j += 3)
			{
				if ((planeBStatus[resultB.ProcIndexBuffer[j]] == 3 ||
					planeBStatus[resultB.ProcIndexBuffer[j + 1]] == 3 ||
					planeBStatus[resultB.ProcIndexBuffer[j + 2]] == 3) ||
					(vertexPlaneStatus[rIndex[resultB.ProcIndexBuffer[j] + resultA.ProcVertexBuffer.Num()]] == 3 &&
						vertexPlaneStatus[rIndex[resultB.ProcIndexBuffer[j + 1] + resultA.ProcVertexBuffer.Num()]] == 3 &&
						vertexPlaneStatus[rIndex[resultB.ProcIndexBuffer[j + 2] + resultA.ProcVertexBuffer.Num()]] == 3))
				{
					finalMesh.ProcIndexBuffer.Add(rIndex[resultB.ProcIndexBuffer[j] + resultA.ProcVertexBuffer.Num()]);
					finalMesh.ProcIndexBuffer.Add(rIndex[resultB.ProcIndexBuffer[j + 1] + resultA.ProcVertexBuffer.Num()]);
					finalMesh.ProcIndexBuffer.Add(rIndex[resultB.ProcIndexBuffer[j + 2] + resultA.ProcVertexBuffer.Num()]);
				}
			}
		}

	}
	m_mesh->ClearAllMeshSections();
	AddMeshSection(0, finalMesh, FTransform());
}


