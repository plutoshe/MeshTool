// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryUtility.h"

GeometryUtility::GeometryUtility()
{
}

GeometryUtility::~GeometryUtility()
{
}

bool GeometryUtility::IsPointInTriangle(FVector i_point, FVector i_v0, FVector i_v1, FVector i_v2)
{
	FVector lineA = i_v1 - i_v0;
	FVector lineB = i_v2 - i_v0;
	float paramA = (i_point.X * lineB.Y - i_point.Y * lineA.X) / (lineA.X * lineB.Y - lineA.Y * lineB.X);
	float paramB = (i_point.X - lineA.X * paramA) / lineB.X;
	if (paramA > 0 && paramA < 1 && paramB > 0 && paramB < 1 && paramA * lineA.Z + paramB * lineB.Z == i_point.Z)
	{
		return true;
	}
	return false;
}

bool GeometryUtility::IsPointInPolyhedron(FVector i_vertex, const FProcMeshSection& i_mesh)
{
	float maxZ = i_mesh.ProcVertexBuffer[0].Position.Z;
	float minZ = maxZ;
	for (int i = 1; i < i_mesh.ProcVertexBuffer.Num(); i++)
	{
		maxZ = FMath::Max(maxZ, i_mesh.ProcVertexBuffer[i].Position.Z);
		minZ = FMath::Min(minZ, i_mesh.ProcVertexBuffer[i].Position.Z);
	}
	FVector src(i_vertex.X, i_vertex.Y, minZ);
	FVector dst(i_vertex.X, i_vertex.Y, maxZ);
	FVector distLine = dst - src;
	int intersectFace = 0;
	for (int i = 0; i < i_mesh.ProcIndexBuffer.Num(); i++)
	{
		if (i % 3 == 2)
		{
			FVector v0 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[0]].Position;
			FVector v1 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[1]].Position;
			FVector v2 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[2]].Position;
			FVector normal = FVector::CrossProduct(v1 - v0, v2 - v0);
			float distSrc = FVector::DotProduct(src - v0, normal);
			float distDst = FVector::DotProduct(dst - v0, normal);
			if (!(distSrc > 0 && distDst > 0 || distSrc < 0 && distDst < 0))
			{
				FVector intersection = src + distLine * (-distSrc / (distDst - distSrc));
				if (IsPointInTriangle(intersection, v0, v1, v2))
				{
					intersectFace++;
				}
			}
		}
	}
	return intersectFace % 2 == 1;
}

void GeometryUtility::TraingleIntersectPolyhedron(
	TArray<FVector> i_vertices, 
	TArray<uint32> i_indices, 
	const FProcMeshSection& i_b, 
	TArray<FProcMeshVertex>& o_generateVertices, 
	TArray<uint32>& o_generateIndices)
{

}

void GeometryUtility::MeshSectionIntersection(const FProcMeshSection& i_a, const FProcMeshSection& i_b, FProcMeshSection& o_result)
{
	// Decide all vertices status for pointed mesh section
	TArray<bool> verticesStatus;
	TArray<uint32> indexConvdersion;
	TArray<FProcMeshVertex> addedVertices;
	TArray<uint32> addedIndices;
	o_result.ProcVertexBuffer.Empty();
	o_result.ProcIndexBuffer.Empty();
	verticesStatus.Empty();
	addedVertices.Empty();
	addedIndices.Empty();

	// initialization
	for (int i = 0; i < i_a.ProcVertexBuffer.Num(); i++)
	{
		verticesStatus.Add(IsPointInPolyhedron(i_a.ProcVertexBuffer[i].Position, i_b));
		addedVertices.Add(i_a.ProcVertexBuffer[i]);
	}
	for (int i = 0; i < i_a.ProcIndexBuffer.Num(); i++)
	{
		addedIndices.Add(i_a.ProcIndexBuffer[i]);
	}

	// generate intersection faces
	TArray<FVector> triangleVerticesArray;
	TArray<uint32> triangleIndicesArray;
	triangleVerticesArray.Init(FVector(0,0,0), 3);
	triangleIndicesArray.Init(0, 3);
	for (int i = 0; i < i_a.ProcIndexBuffer.Num(); i++)
	{
		triangleIndicesArray[i % 3] = i_a.ProcIndexBuffer[i];
		triangleVerticesArray[i % 3] = i_a.ProcVertexBuffer[i_a.ProcIndexBuffer[i]].Position;
		if (i % 3 == 2) {
			TraingleIntersectPolyhedron(triangleVerticesArray, triangleIndicesArray, i_b, addedVertices, addedIndices);
		}
	}
	indexConvdersion.Init(0, addedVertices.Num());

	// fliter vertex that in the polyhedron
	int filteringVerticesNum = 0;
	for (int i = 0; i < addedVertices.Num(); i++) {
		indexConvdersion[i] = filteringVerticesNum;
		if (i >= i_a.ProcVertexBuffer.Num() || !verticesStatus[i])
		{
			o_result.ProcVertexBuffer.Add(i_a.ProcVertexBuffer[i]);
			filteringVerticesNum++;
		}

		o_result.ProcVertexBuffer.Add(addedVertices[i]);
	}

	// fliter face that in the polyhedron
	bool isInMesh = false;
	for (int i = 0; i < addedIndices.Num(); i++)
	{
		isInMesh = isInMesh || verticesStatus[i_a.ProcIndexBuffer[i]];
		if (i % 3 == 2) {
			if (!isInMesh)
			{
				o_result.ProcIndexBuffer.Add(addedIndices[i]);
				o_result.ProcIndexBuffer.Add(addedIndices[i - 1]);
				o_result.ProcIndexBuffer.Add(addedIndices[i - 2]);
			}
			isInMesh = false;
		}
	}
}