// Fill out your copyright notice in the Description page of Project Settings.

#include "AMeshGenerator.h"
#include "Public/Tool/GeometryUtility.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"

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
	m_blockID = -1;
	m_iteration = 1;
	m_percision = 200;
	m_alpha = 0.5;
	m_beta = 0.8;
	
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

void AAMeshGenerator::Division()
{
	TArray< FProcMeshSection> newMeshs;
	newMeshs.Empty();
	for (int mi = 0; mi < m_mesh->GetNumSections(); mi++)
	{
		newMeshs.Add(*(m_mesh->GetProcMeshSection(mi)));
	}

	for (int mi = 0; mi < newMeshs.Num(); mi++)
	{
		GeometryUtility::Division(newMeshs[mi], newMeshs[mi], m_percision);
	}
	
	m_mesh->ClearAllMeshSections();
	for (int mi = 0; mi < newMeshs.Num(); mi++)
	{
		AddMeshSection(mi, newMeshs[mi], FTransform());
	}
}

void AAMeshGenerator::Smooth()
{
	//Division();
	TArray< FProcMeshSection> newMeshs;
	newMeshs.Empty();
	for (int mi = 0; mi < m_mesh->GetNumSections(); mi++)
	{
		newMeshs.Add(*(m_mesh->GetProcMeshSection(mi)));
	}
	for (int i = 0; i < m_iteration; i++)
	{
		/*for (int j = 0; j < newMeshs.Num(); j++)
		{
			auto i_mesh = newMeshs[j];
			for (int k = 0; k < i_mesh.ProcIndexBuffer.Num() - 2; k += 3)
			{
				FVector va = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[k]].Position;
				FVector vb = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[k + 1]].Position;
				FVector vc = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[k + 2]].Position;
				DrawDebugLine(GetWorld(), va + offset, vb + offset, FColor(1, 0, 0, 1), true, -1, 0, 10);
				DrawDebugLine(GetWorld(), vb + offset, vc + offset, FColor(1, 0, 0, 1), true, -1, 0, 10);
				DrawDebugLine(GetWorld(), vc + offset, va + offset, FColor(1, 0, 0, 1), true, -1, 0, 10);
			}
		}*/

		for (int j = 0; j < newMeshs.Num(); j++)
		{
			auto i_mesh = newMeshs[j];
			GeometryUtility::OutputMesh(i_mesh, GetWorld(), FColor(0, 1, 1, 0), FVector(-2000, 2000, 200));
		}
		for (int mi = 0; mi < newMeshs.Num(); mi++)
			/*m_mesh->UpdateMeshSection()*/
			GeometryUtility::hcFilter(newMeshs[mi], newMeshs[mi], m_alpha, m_beta);
		
		for (int j = 0; j < newMeshs.Num(); j++)
		{
			auto i_mesh = newMeshs[j];
			GeometryUtility::OutputMesh(i_mesh, GetWorld(), FColor(1,0,0,0), FVector(2000, 2000, 200));
		}
		

	}

	m_mesh->ClearAllMeshSections();
	for (int mi = 0; mi < newMeshs.Num(); mi++)
	{
		AddMeshSection(mi, newMeshs[mi], FTransform());
	}
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
	GeometryUtility::m_world = GetWorld();
	GeometryUtility::m_block = m_blockID;
	for (int i = 1; i < m_mesh->GetNumSections(); i++)
	{
		FProcMeshSection addedMesh = *(m_mesh->GetProcMeshSection(i));

		finalMesh = GeometryUtility::MeshCombination(finalMesh, addedMesh, m_insertMode);
	}
	

	m_mesh->ClearAllMeshSections();
	AddMeshSection(0, finalMesh, FTransform());
}


