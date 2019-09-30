// Fill out your copyright notice in the Description page of Project Settings.

#include "AMeshGenerator.h"
#include  "Engine/StaticMesh.h"

// Sets default values
AAMeshGenerator::AAMeshGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	// New in UE 4.17, multi-threaded PhysX cooking.
	mesh->bUseAsyncCooking = true;
	
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
	CreateTriangle();
}

void AAMeshGenerator::AddMesh(UProceduralMeshComponent* i_addMesh)
{
	int existingMeshNum = mesh->GetNumSections();
	


	for (int i = 0; i < i_addMesh->GetNumSections(); i++)
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
		for (int j = 0; j < i_addMesh->GetProcMeshSection(i)->ProcVertexBuffer.Num(); j++)
		{
			vertices.Add(i_addMesh->GetProcMeshSection(i)->ProcVertexBuffer[j].Position);
			tangents.Add(i_addMesh->GetProcMeshSection(i)->ProcVertexBuffer[j].Tangent);
			normals.Add(i_addMesh->GetProcMeshSection(i)->ProcVertexBuffer[j].Normal);
			UV0.Add(i_addMesh->GetProcMeshSection(i)->ProcVertexBuffer[j].UV0);
			vertexColors.Add(i_addMesh->GetProcMeshSection(i)->ProcVertexBuffer[j].Color);
		}
		for (int j = 0; j < i_addMesh->GetProcMeshSection(i)->ProcIndexBuffer.Num(); j++)
		{
			Triangles.Add(i_addMesh->GetProcMeshSection(i)->ProcIndexBuffer[j]);
		}
		
		mesh->CreateMeshSection_LinearColor(existingMeshNum + i, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
		//mesh->CreateMeshSection_LinearColor(existingMeshNum + i, , i_addMesh->GetProcMeshSection(i)->ProcIndexBuffer, normals, UV0, vertexColors, tangents, true);

	}
}

void AAMeshGenerator::CreateTriangle()
{
	TArray<FVector> vertices;
	vertices.Add(FVector(0, 0, 0));
	vertices.Add(FVector(0, 100, 0));
	vertices.Add(FVector(0, 0, 100));

	TArray<int32> Triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	TArray<FVector> normals;
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	TArray<FVector2D> UV0;
	UV0.Add(FVector2D(0, 0));
	UV0.Add(FVector2D(10, 0));
	UV0.Add(FVector2D(0, 10));


	TArray<FProcMeshTangent> tangents;
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));

	TArray<FLinearColor> vertexColors;
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

	mesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);


	// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);
}

