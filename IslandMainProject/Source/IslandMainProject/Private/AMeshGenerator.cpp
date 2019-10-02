// Fill out your copyright notice in the Description page of Project Settings.

#include "AMeshGenerator.h"
#include "Public/Tool/GeometryUtility.h"
#include  "Engine/StaticMesh.h"

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

void AAMeshGenerator::AddMeshSection(int i_id, const FProcMeshSection& i_src, const FTransform &i_transform)
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
		vertices.Add(i_src.ProcVertexBuffer[j].Position + i_transform.GetLocation());
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
		GeometryUtility::MeshSectionIntersection(addedMesh, finalMesh, resultA);
		GeometryUtility::MeshSectionIntersection(finalMesh, addedMesh, resultB);

		finalMesh.ProcIndexBuffer.Empty();
		finalMesh.ProcVertexBuffer.Empty();
		finalMesh.ProcVertexBuffer.Append(resultA.ProcVertexBuffer);
		finalMesh.ProcVertexBuffer.Append(resultB.ProcVertexBuffer);
		finalMesh.ProcIndexBuffer.Append(resultA.ProcIndexBuffer);
		for (int j = 0; j < resultB.ProcIndexBuffer.Num(); j++)
		{
			finalMesh.ProcIndexBuffer.Add(resultB.ProcIndexBuffer[j] + resultA.ProcVertexBuffer.Num());
		}
	}
	m_mesh->ClearAllMeshSections();
	AddMeshSection(0, finalMesh, FTransform());
}

void AAMeshGenerator::CreateTriangle()
{
	//TArray<FVector> vertices;
	//vertices.Add(FVector(0, 0, 0));
	//vertices.Add(FVector(0, 100, 0));
	//vertices.Add(FVector(0, 0, 100));

	//TArray<int32> Triangles;
	//Triangles.Add(0);
	//Triangles.Add(1);
	//Triangles.Add(2);

	//TArray<FVector> normals;
	//normals.Add(FVector(1, 0, 0));
	//normals.Add(FVector(1, 0, 0));
	//normals.Add(FVector(1, 0, 0));

	//TArray<FVector2D> UV0;
	//UV0.Add(FVector2D(0, 0));
	//UV0.Add(FVector2D(10, 0));
	//UV0.Add(FVector2D(0, 10));


	//TArray<FProcMeshTangent> tangents;
	//tangents.Add(FProcMeshTangent(0, 1, 0));
	//tangents.Add(FProcMeshTangent(0, 1, 0));
	//tangents.Add(FProcMeshTangent(0, 1, 0));

	//TArray<FLinearColor> vertexColors;
	//vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	//vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	//vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

	//mesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);


	//// Enable collision data
	//mesh->ContainsPhysicsTriMeshData(true);
}

