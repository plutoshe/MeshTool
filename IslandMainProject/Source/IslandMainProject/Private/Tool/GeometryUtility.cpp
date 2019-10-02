// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryUtility.h"

GeometryUtility::GeometryUtility()
{
}

GeometryUtility::~GeometryUtility()
{
}
// 1 means in
// 2 means on the line
// view point situation as line situation
int GeometryUtility::IsPointInTriangle(FVector i_point, FVector i_v0, FVector i_v1, FVector i_v2)
{
	FVector lineA = i_v1 - i_v0;
	FVector lineB = i_v2 - i_v0;
	i_point -= i_v0;
	float paramA = (i_point.X * lineB.Y - i_point.Y * lineB.X) / (lineA.X * lineB.Y - lineA.Y * lineB.X);
	float paramB;
	if (lineB.X != 0)
	{
		paramB = (i_point.X - lineA.X * paramA) / lineB.X;
	}
	else
	{
		paramB = (i_point.Y - lineA.Y * paramA) / lineB.Y;
	}
	
	if (paramA > 0 &&  paramB > 0 && paramA + paramB < 1 && paramA * lineA.Z + paramB * lineB.Z == i_point.Z)
	{
		return 1;
	}
	if (paramA >= 0 && paramB >= 0 && paramB + paramA <= 1 && paramA * lineA.Z + paramB * lineB.Z == i_point.Z)
	{
		return 2;
	}
	return 0;
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
	FVector src(i_vertex.X, i_vertex.Y, i_vertex.Z);
	FVector dst(i_vertex.X, i_vertex.Y, maxZ + 1);
	FVector distLine = dst - src;
	int intersectFace = 0;
	int intersectLine = 0;
	for (int i = 0; i < i_mesh.ProcIndexBuffer.Num() - 2; i+=3)
	{
		FVector v0 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[i]].Position;
		FVector v1 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[i + 1]].Position;
		FVector v2 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[i + 2]].Position;
		FVector normal = FVector::CrossProduct(v1 - v0, v2 - v0);
		normal.Normalize();
		float distSrc = FVector::DotProduct(src - v0, normal);
		float distDst = FVector::DotProduct(dst - v0, normal);
		if (!(distSrc > 0 && distDst > 0 || distSrc < 0 && distDst < 0))
		{
			FVector intersection = src + distLine * (-distSrc / (distDst - distSrc));
			int inFaceResult = IsPointInTriangle(intersection, v0, v1, v2);
			switch (inFaceResult)
			{
			case 1: intersectFace++; break;
			case 2: intersectLine++; break;
			default:
				break;
			}

		}
	}
	return (intersectLine / 2 + intersectFace) % 2 == 1;
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
	}

	// fliter face that in the polyhedron
	bool isInMesh = false;
	for (int i = 0; i < addedIndices.Num()-2; i+=3)
	{
		isInMesh = verticesStatus[addedIndices[i]] || verticesStatus[addedIndices[i + 1]] || verticesStatus[addedIndices[i + 2]];
		
		if (!isInMesh)
		{
			o_result.ProcIndexBuffer.Add(indexConvdersion[addedIndices[i]]);
			o_result.ProcIndexBuffer.Add(indexConvdersion[addedIndices[i + 1]]);
			o_result.ProcIndexBuffer.Add(indexConvdersion[addedIndices[i + 2]]);
		}
	}
}