// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
/**
 * 
 */
class ISLANDMAINPROJECT_API GeometryUtility
{
public:
	GeometryUtility();
	~GeometryUtility();

	static bool IsPointOnLineSegment(const FVector& i_point, const FVector& i_v0, const FVector& i_v1);
	static int IsPointInTriangle(FVector i_point, FVector i_v0, FVector i_v1, FVector i_v2);
	static bool IsPointInPolyhedron(FVector i_vertices, const FProcMeshSection& i_mesh);
	static void TraingleIntersectPolyhedron(TArray<FVector> i_vertex, TArray<uint32> i_indices, const FProcMeshSection& i_b, TArray<FVector> &o_generateVertices, TArray<uint32> &o_generateIndices);
	static void MeshSectionIntersection(const FProcMeshSection &i_a, const FProcMeshSection &i_b, FProcMeshSection &o_result);
	static bool GetLineAndPlaneIntersectionPoint(const FVector& i_va, const FVector& i_vb, const FVector& i_normal, FVector &o_intersection);
	static bool GetLineAndLineIntersectionPoint(const FVector& i_va, const FVector& i_vb, const FVector& i_linea, const FVector& i_lineb, FVector &o_intersection);
};
