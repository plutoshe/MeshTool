// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
/**
 * 
 */

class DVector;
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
		TArray<DVector> i_vertex, 
		TArray<uint32> i_indices, 
		const FProcMeshSection& i_b, 
		TArray<DVector> &o_generateVertices,
		TArray<uint32> &o_generateIndices);	
	static void MeshSectionIntersection(
		const FProcMeshSection& i_a, 
		const FProcMeshSection& i_b, 
		FProcMeshSection& o_result, 
		TArray<int>& t_planeAStatus, 
		TArray<int>& t_planeBStatus);
	static bool GetLineAndPlaneIntersectionPoint(const DVector& i_va, const DVector& i_vb, const DVector& i_planePoint, const DVector& i_planeNormal, DVector&o_intersection);
	static bool GetLineAndLineIntersectionPoint(const DVector& i_va, const DVector& i_vb, const DVector& i_linea, const DVector& i_lineb, DVector&o_intersection);
	static FProcMeshSection MeshCombination(FProcMeshSection i_finalMesh, FProcMeshSection i_addedMesh, int i_insertMode);
	static TArray<int> m_vertexBorder[2];
	static int m_currentVertexBorderID;
	static int FindFather(TArray<int>& i_status, int i_a);
	static void BorderLink(TArray<int>& i_status, int i_a, int i_b);
	static int m_block;

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