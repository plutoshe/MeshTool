// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
/**
 * 
 */

class DVector;
class DVertex;
class ISLANDMAINPROJECT_API GeometryUtility
{
public:
	GeometryUtility();
	~GeometryUtility();
	static const double eps_const;
	static UWorld* m_world;
	static int eps(double gap)
	{
		if (gap > eps_const)
		{
			return 1;
		}
		else if (gap < -eps_const)
		{
			return -1;
		}
		return 0;

	}

	template <class T>
	class sortVertex {
	public:
		T data;
		int index;
		sortVertex() : data(), index(0) {}
		sortVertex(T i_data, int i_index) : data(i_data), index(i_index) {}
		sortVertex(const sortVertex& i_p) : data(i_p.data), index(i_p.index) {}
		void operator = (const sortVertex& i_p) {
			data = i_p.data; index = i_p.index;
		}
		//sortVertex(FVector i_data, int i_index) : data(i_data), index(i_index) {}
	};
	static bool IsPointOnLineSegment(const DVector& i_point, const DVector& i_v0, const DVector& i_v1);
	static int IsPointInTriangle(DVector i_point, DVector i_v0, DVector i_v1, DVector i_v2);
	static bool IsPointInPolyhedron(DVector i_vertices, const FProcMeshSection& i_mesh);
	static void TraingleIntersectPolyhedron(
		const FProcMeshSection& i_b, 
		TArray<DVertex> &o_vertices,
		TArray<uint32> &o_indices);	
	static void MeshSectionIntersection(const FProcMeshSection& i_a, const FProcMeshSection& i_b, FProcMeshSection& o_result, TArray<bool>& o_oldvertexIdentifier);
	static bool GetLineAndPlaneIntersectionPoint(const DVector& i_va, const DVector& i_vb, const DVector& i_planePoint, const DVector& i_planeNormal, DVector&o_intersection);
	static bool GetLineAndLineIntersectionPoint(const DVector& i_va, const DVector& i_vb, const DVector& i_linea, const DVector& i_lineb, DVector&o_intersection);
	static FProcMeshSection MeshCombination(FProcMeshSection i_meshA, FProcMeshSection i_meshB, int i_insertMode);
	static TArray<int> m_vertexBorder[2];
	static int m_currentVertexBorderID;
	static int FindFather(TArray<int>& i_status, int i_a);
	static void DisjointSetLink(TArray<int>& i_status, int i_a, int i_b);
	static int m_block;
	static int AddNewVertex(TArray<DVertex>& o_vertices, DVertex newData);
	static void BorderLink(TArray<int>& i_disjointSetStatus, TMap<int, int>& i_indexPartitionPointMapping, TArray<int>& i_intersetctionsStatus, int i_indexA, int i_indexB);
	static void hcFilter(FProcMeshSection i_in, FProcMeshSection& o_out, float alpha, float beta);
	static TArray<DVector> centralFilter(TArray<DVector> i_vertices, TArray<uint32> i_indices);
	static TArray<DVector> laplacianFilter(TArray<DVector> i_vertices, TArray<uint32> i_indices);
	static TArray<DVector> laplacianFilterWithDistance(TArray<DVector> i_vertices, TArray<uint32> i_indices, float i_lambda = 1.0f);
	/*static void laplacianFilterWithCurvatureFlow(TArray<DVector> i_vertices, TArray<uint32> i_indices);
	static float GetCurveWeight(DVector origin, DVector adjcent, TArray<DVector>& adjVertices, TArray<uint32>& adjacentFaceIndex);*/
 	static void findAdjacentNeighbors(TArray<DVector> i_vertices, TArray<uint32> i_indices, DVector vertex, TArray<DVector>& adjacentV, TArray<uint32>&adjacentI);
	static void findAdjacentFace(TArray<DVector> i_vertices, TArray<uint32> i_indices, DVector vertex, TArray<uint32>& adjacentFaceIndex);
	static void Division(FProcMeshSection i_in, FProcMeshSection& o_out, float i_percision);
	static void DivisionPointSegment(
		FProcMeshSection& o_out,
		TMap<int32, TPair<int32, int32>>& o_divisionMap,
		int i_index,
		const FProcMeshVertex& i_origin,
		int32& o_start,
		int32& o_num,
		float i_percision,
		int i_addedSegmentNum);
	static void DivisionPointFaces(FProcMeshSection& o_out, int32 i_centerPointIndex, float i_percision, int i_addFaceNum);
	static bool DivisionSegment(
		FProcMeshSection& o_out,
		FProcMeshVertex i_va,
		FProcMeshVertex i_vb,
		int32 i_ia,
		int32 i_ib,
		int32 i_ic,
		float i_percision);
	static void OutputMesh(const FProcMeshSection& i_mesh, UWorld* i_world, FColor i_color = FColor(0, 0, 0, 1), FVector i_offset = FVector(0, 0, 0));
};

class DVector
{
public:
	double X, Y, Z;
	DVector() { X = Y = Z = 0; }
	DVector(FVector i_f) { X = i_f.X; Y = i_f.Y; Z = i_f.Z; }
	DVector(const DVector& i_f) { X = i_f.X; Y = i_f.Y; Z = i_f.Z; }
	DVector(double i_x, double i_y, double i_z) : X(i_x), Y(i_y), Z(i_z) {}

	friend DVector operator + (const double& i_a, const DVector& i_b) { return DVector(i_a + i_b.X, i_a + i_b.Y, i_a + i_b.Z); }
	friend DVector operator - (const double& i_a, const DVector& i_b) { return DVector(i_a - i_b.X, i_a - i_b.Y, i_a - i_b.Z); }
	friend DVector operator * (const double& i_a, const DVector& i_b) { return DVector(i_a * i_b.X, i_a * i_b.Y, i_a * i_b.Z); }
	friend DVector operator / (const double& i_a, const DVector& i_b) { return DVector(i_a / i_b.X, i_a / i_b.Y, i_a / i_b.Z); }

	friend DVector operator + (const DVector& i_a, const double& i_b) { return DVector(i_a.X + i_b, i_a.Y + i_b, i_a.Z + i_b); }
	friend DVector operator - (const DVector& i_a, const double& i_b) { return DVector(i_a.X - i_b, i_a.Y - i_b, i_a.Z - i_b); }
	friend DVector operator * (const DVector& i_a, const double& i_b) { return DVector(i_a.X * i_b, i_a.Y * i_b, i_a.Z * i_b); }
	friend DVector operator / (const DVector& i_a, const double& i_b) { return DVector(i_a.X / i_b, i_a.Y / i_b, i_a.Z / i_b); }

	friend DVector operator + (const DVector& i_a, const DVector& i_b) { return DVector(i_a.X + i_b.X, i_a.Y + i_b.Y, i_a.Z + i_b.Z); }
	friend DVector operator - (const DVector& i_a, const DVector& i_b) { return DVector(i_a.X - i_b.X, i_a.Y - i_b.Y, i_a.Z - i_b.Z); }
	friend DVector operator * (const DVector& i_a, const DVector& i_b) { return DVector(i_a.X * i_b.X, i_a.Y * i_b.Y, i_a.Z * i_b.Z); }
	friend DVector operator / (const DVector& i_a, const DVector& i_b) { return DVector(i_a.X / i_b.X, i_a.Y / i_b.Y, i_a.Z / i_b.Z); }
	void operator = (const DVector& i_a) { X = i_a.X; Y = i_a.Y; Z = i_a.Z; }
	friend bool operator == (const DVector& i_a, const DVector& i_b) { return (GeometryUtility::eps(i_a.X - i_b.X) == 0) && (GeometryUtility::eps(i_a.Y - i_b.Y) == 0) && (GeometryUtility::eps(i_a.Z - i_b.Z) == 0); }

	static double DotProduct(const DVector& i_a, const DVector& i_b) { return i_a.X * i_b.X + i_a.Y * i_b.Y + i_a.Z * i_b.Z; }
	double Size() const { return sqrt(X * X + Y * Y + Z * Z); }
	static const DVector Zero;
	static const DVector Unit;
	static DVector CrossProduct(const DVector& i_a, const DVector& i_b)
	{
		return DVector(
			i_a.Y * i_b.Z - i_a.Z * i_b.Y,
			i_a.Z * i_b.X - i_a.X * i_b.Z,
			i_a.X * i_b.Y - i_a.Y * i_b.X);
	}
	void Normalize() { double size = Size(); *this = *this / size; }
	FVector FVectorConversion() { return FVector(X, Y, Z); }
};

class partitionPointST {
	DVector m_data;
	int m_intersetionIndex;
	int m_index;
};


class DVertex {
public:
	DVector m_Position;
	DVector m_normal;
	DVertex() : m_Position() {}
	DVertex(const DVertex& i_v) : m_Position(i_v.m_Position) {}
	DVertex(const FProcMeshVertex& i_v) { Equal(i_v); }
	void Equal(const FProcMeshVertex& i_v)
	{
		m_Position = i_v.Position;
		m_normal = i_v.Normal;

	}
	void operator = (const FProcMeshVertex& i_v) 
	{
		Equal(i_v);
	}
	FProcMeshVertex ToProcVertex()
	{
		auto a = FProcMeshVertex();
		a.Position = m_Position.FVectorConversion();
		a.Normal = m_normal.FVectorConversion();
		return a;
	}
};