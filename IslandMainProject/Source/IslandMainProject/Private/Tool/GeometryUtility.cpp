// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryUtility.h"
#include "DrawDebugHelpers.h"

DVector const DVector::Zero(0, 0, 0);
DVector const DVector::Unit(1, 1, 1);
const double GeometryUtility::eps_const = 1e-3;
UWorld* GeometryUtility::m_world = nullptr;
TArray<TSet<int>> GeometryUtility::m_vertexBorder[2] = { {}, {}};
int GeometryUtility::m_currentVertexBorderID = 0;
int GeometryUtility::m_block = -1;
TMap<TPair<int, int>, int> GeometryUtility::m_lines;
GeometryUtility::GeometryUtility()
{
}

GeometryUtility::~GeometryUtility()
{
}

// 1 means in
// 2 means on the line
// 3 means on the point
// view point situation as line situation
int GeometryUtility::IsPointInTriangle(DVector i_point, DVector i_v0, DVector i_v1, DVector i_v2)
{
	DVector lineA = i_v1 - i_v0;
	DVector lineB = i_v2 - i_v0;
	i_point = i_point - i_v0;
	double paramA = 0;
	double paramB = 0;
	double resultC;
	if (eps(lineA.X * lineB.Y - lineA.Y * lineB.X) != 0)
	{
		paramA = (i_point.X * lineB.Y - i_point.Y * lineB.X) / (lineA.X * lineB.Y - lineA.Y * lineB.X);
		if (lineB.X != 0)
		{
			paramB = (i_point.X - lineA.X * paramA) / lineB.X;
		}
		else if (lineB.Y != 0)
		{
			paramB = (i_point.Y - lineA.Y * paramA) / lineB.Y;
		}
		resultC = paramA * lineA.Z + paramB * lineB.Z - i_point.Z;
	}
	else if (eps(lineA.X * lineB.Z - lineA.Z * lineB.X) != 0)
	{
		paramA = (i_point.X * lineB.Z - i_point.Z * lineB.X) / (lineA.X * lineB.Z - lineA.Z * lineB.X);
		if (lineB.X != 0)
		{
			paramB = (i_point.X - lineA.X * paramA) / lineB.X;
		}
		else if (lineB.Z != 0)
		{
			paramB = (i_point.Z - lineA.Z * paramA) / lineB.Z;
		}
		resultC = paramA * lineA.Y + paramB * lineB.Y - i_point.Y;
	}
	else
	{
		paramA = (i_point.Y * lineB.Z - i_point.Z * lineB.Y) / (lineA.Y * lineB.Z - lineA.Z * lineB.Y);
		if (lineB.Z != 0)
		{
			paramB = (i_point.Z - lineA.Z * paramA) / lineB.Z;
		}
		else if (lineB.Y != 0)
		{
			paramB = (i_point.Y - lineA.Y * paramA) / lineB.Y;
		}
		resultC = paramA * lineA.X + paramB * lineB.X - i_point.X;
	}

	
	
	if (eps(paramA - 0) > 0 &&  eps(paramB) > 0 && eps(paramA + paramB - 1) < 0 && eps(resultC) == 0)
	{
		return 1;
	}

	if (eps(paramA) >= 0 && eps(paramB) >= 0 && eps(paramB + paramA - 1) <= 0 && eps(resultC) == 0)
	{
		if (eps(paramA + paramB) > 0 && eps(paramA - 1) < 0 && eps(paramB - 1) < 0)
		{
			// include situations on lines
			return 2;
		}
		else
		{
			// include situations on points
			return 3;
		}
	}
	return 0;
}

bool GeometryUtility::IsPointInPolyhedron(DVector i_vertex, const FProcMeshSection& i_mesh)
{
	float maxZ = i_mesh.ProcVertexBuffer[0].Position.Z;
	float minZ = maxZ;
	for (int i = 1; i < i_mesh.ProcVertexBuffer.Num(); i++)
	{
		maxZ = FMath::Max(maxZ, i_mesh.ProcVertexBuffer[i].Position.Z);
		minZ = FMath::Min(minZ, i_mesh.ProcVertexBuffer[i].Position.Z);
	}
	DVector src(i_vertex.X, i_vertex.Y, i_vertex.Z);
	DVector dst(i_vertex.X, i_vertex.Y, maxZ + 1);
	DVector distLine = dst - src;
	int intersectFace = 0;
	int intersectLine = 0;
	for (int i = 0; i < i_mesh.ProcIndexBuffer.Num() - 2; i+=3)
	{
		DVector v0 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[i]].Position;
		DVector v1 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[i + 1]].Position;
		DVector v2 = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[i + 2]].Position;
		DVector normal = DVector::CrossProduct(v1 - v0, v2 - v0);
		normal.Normalize();
		float distSrc = DVector::DotProduct(src - v0, normal);
		float distDst = DVector::DotProduct(dst - v0, normal);
		if (!(eps(distSrc) > 0 && eps(distDst) > 0 || eps(distSrc) < 0 && eps(distDst) < 0))
		{
			DVector intersection = src + distLine * (-distSrc / (distDst - distSrc));
			int inFaceResult = IsPointInTriangle(intersection, v0, v1, v2);
			switch (inFaceResult)
			{
			case 1: intersectFace++; break;
			case 2: case 3: // currently ignore converge at one point
				intersectLine++; break;
			default:
				break;
			}

		}
	}
	return (intersectLine / 2 + intersectFace) % 2 == 1;
}

bool GeometryUtility::GetLineAndPlaneIntersectionPoint(const DVector& i_va, const DVector& i_vb, const DVector& i_planePoint, const DVector& i_planeNormal, DVector& o_intersection)
{
	DVector i_nPlane = i_planeNormal;
	i_nPlane.Normalize();
	double distA = DVector::DotProduct(i_va - i_planePoint, i_nPlane);
	double distB = DVector::DotProduct(i_vb - i_planePoint, i_nPlane);
	// currently ignore the situation that point on the plane
	
	if (distA > 0 && distB < 0 || distA < 0 && distB > 0)
	{
		o_intersection = i_va + (i_vb - i_va) * (-distA) / (distB - distA);
		return true;
	}
	// TO-DO:
	// two more sitautions:
	// one point on the plane
	// two point on the plane
	return false;
}

bool GeometryUtility::GetLineAndLineIntersectionPoint(const DVector& i_va, const DVector& i_vb, const DVector& i_linea, const DVector& i_lineb, DVector &o_intersection)
{
	DVector line1 = i_lineb - i_linea;
	DVector line2 = i_vb - i_va;
	line1.Normalize();
	line2.Normalize();
	double ParamA = DVector::DotProduct(i_va - i_linea, line1);
	double ParamB = DVector::DotProduct(i_va - i_linea, line2);
	

	double parallelParam = DVector::DotProduct(line1, line2);
	if (eps(parallelParam - 1) == 0)
	{
		return false;
	}
	double t1 = (-1 * ParamA + parallelParam * ParamB) / (parallelParam * parallelParam - 1);
	double t2 = (-parallelParam * ParamA + ParamB) / (parallelParam * parallelParam - 1);
	o_intersection = line1 * t1 + i_linea;
	DVector db = i_va + line2 * t2;
	auto s1 = DVector::DotProduct(i_lineb - i_linea, line1);
	auto s2 = DVector::DotProduct(i_vb - i_va, line2) - t2;
	if (o_intersection == db)
	{
		if (eps(t1) >= 0 && eps(DVector::DotProduct(i_lineb - i_linea, line1) - t1) >= 0 &&
			eps(t2) >= 0 && eps(DVector::DotProduct(i_vb - i_va, line2) - t2) >= 0)
		{
			return true;
		}
	}
	return false;
	/*DVector perp = sa - line * (DVector::DotProduct(sa, line) / line.Size() / line.Size());
	if (isnan(perp.X))
	{
		int i = 0;
	}
	if (perp.Size() == 0)
	{
		o_intersection = i_va;
		return true;
	}
	perp.Normalize();
	double da = DVector::DotProduct(sa, perp);
	double db = DVector::DotProduct(sb, perp);
	
	if (eps(da - db) == 0)
	{
		o_intersection = i_va;
		return true;
	}
	DVector intersection = (sa + (-da) / (db - da) * (sb - sa));
	if ((eps(da) >= 0 && eps(db) <= 0 || eps(da) <= 0 && eps(db) >= 0) &&  intersection.Size() <= line.Size())
	{
		o_intersection = i_linea + intersection;

		return true;
	}
	return false;*/
}


int GeometryUtility::AddNewVertex(TArray<DVertex>& o_vertices, DVertex newData)
{
	o_vertices.Add(newData);
	m_vertexBorder[m_currentVertexBorderID].Add(TSet<int>());
	return o_vertices.Num() - 1;
}

void GeometryUtility::TraingleIntersectPolyhedron(
	const FProcMeshSection& i_b,
	TArray<DVertex>& o_vertices,
	TArray<uint32> &o_indices)
{
	if (o_indices.Num() < 3)
	{
		return;
	}
	/*if (m_b == m_block - 1)
	{
		int i = 0;
	}*/
	DVector ova = o_vertices[o_indices[0]].m_Position;
	DVector ovb = o_vertices[o_indices[1]].m_Position;
	DVector ovc = o_vertices[o_indices[2]].m_Position;

	auto doubleSortexMethod = [](const sortVertex<DVector>& a, const sortVertex<DVector>& b) {
		if (GeometryUtility::eps(a.data.X - b.data.X) < 0)
		{
			return true;
		}
		if (GeometryUtility::eps(a.data.X - b.data.X) == 0 && GeometryUtility::eps(a.data.Y - b.data.Y) < 0)
		{
			return true;
		}
		if (GeometryUtility::eps(a.data.X - b.data.X) == 0 && GeometryUtility::eps(a.data.Y - b.data.Y) == 0 && GeometryUtility::eps(a.data.Z - b.data.Z) < 0)
		{
			return true;
		}

		if (GeometryUtility::eps(a.data.X - b.data.X) == 0 && GeometryUtility::eps(a.data.Y - b.data.Y) == 0 && GeometryUtility::eps(a.data.Z - b.data.Z) == 0 && a.index < b.index)
		{
			return true;
		}
		return false;
	};
	DVector currentNormal = DVector::CrossProduct(ovc - ova, ovb - ova);
	DVector intersection;
	TArray<DVector> planeIntersections;
	TArray<sortVertex<DVector>> partitionPoints;
	planeIntersections.Empty();
	partitionPoints.Empty();
	for (int i = 0; i + 2 < i_b.ProcIndexBuffer.Num(); i += 3)
	{
		DVertex va = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i]];
		DVertex vb = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i + 1]];
		DVertex vc = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i + 2]];
			
		planeIntersections.Empty();
		if (GetLineAndPlaneIntersectionPoint(va.m_Position, vb.m_Position, ova, currentNormal, intersection))
		{
			planeIntersections.Add(intersection);

		}
		if (GetLineAndPlaneIntersectionPoint(vb.m_Position, vc.m_Position, ova, currentNormal, intersection))
		{
			planeIntersections.Add(intersection);

		}
		if (GetLineAndPlaneIntersectionPoint(va.m_Position, vc.m_Position, ova, currentNormal, intersection))
		{
			planeIntersections.Add(intersection);
		}

		if (planeIntersections.Num() == 2)
		{
			partitionPoints.Empty();
			// point in triangle
			for (int k = 0; k < 2; k++)
			{
				if (IsPointInTriangle(planeIntersections[k], ova, ovb, ovc) == 1)
				{
					partitionPoints.Add(sortVertex<DVector>(planeIntersections[k], k));
				}
			}


			// the borders intersection
			for (int j = 0; j < o_indices.Num() - 2; j += 3)
			{
				uint32 ia = o_indices[j];
				uint32 ib = o_indices[j + 1];
				uint32 ic = o_indices[j + 2];
				DVector sva = o_vertices[ia].m_Position;
				DVector svb = o_vertices[ib].m_Position;
				DVector svc = o_vertices[ic].m_Position;

				if (GetLineAndLineIntersectionPoint(planeIntersections[0], planeIntersections[1], svb, sva, intersection))
				{
					partitionPoints.Add(sortVertex<DVector>(intersection, -1));
				}
				if (GetLineAndLineIntersectionPoint(planeIntersections[0], planeIntersections[1], svc, sva, intersection))
				{
					partitionPoints.Add(sortVertex<DVector>(intersection, -1));
				}
				if (GetLineAndLineIntersectionPoint(planeIntersections[0], planeIntersections[1], svc, svb, intersection))
				{
					partitionPoints.Add(sortVertex<DVector>(intersection, -1));
				}
			}
			partitionPoints.Sort(doubleSortexMethod);
			int pi = 1;
			while (pi < partitionPoints.Num()) {
				if (partitionPoints[pi].data == partitionPoints[pi - 1].data) {
					partitionPoints.RemoveAt(pi);
				}
				else {
					pi++;
				}
			}
			int lastIndex = -1;
			for (int partitionID = 0; partitionID < partitionPoints.Num(); partitionID++)
			{
				bool exist = false;
				int currentVertexID = -1;
				for (int j = (o_indices.Num() - 3) / 3 * 3; j >= 0; j -= 3)
				{
					auto result = IsPointInTriangle(partitionPoints[partitionID].data,
						o_vertices[o_indices[j]].m_Position,
						o_vertices[o_indices[j + 1]].m_Position,
						o_vertices[o_indices[j + 2]].m_Position);
					if (result > 0 && result < 3)
					{
						uint32 ia = o_indices[j];
						uint32 ib = o_indices[j + 1];
						uint32 ic = o_indices[j + 2];
						if (!exist)
						{
							
							auto newVertex = o_vertices[ia];
							newVertex.m_Position = partitionPoints[partitionID].data;
							currentVertexID = AddNewVertex(o_vertices, newVertex);
							m_vertexBorder[m_currentVertexBorderID][currentVertexID].Add(i / 3);
							exist = true;
						}
						
						auto sva = o_vertices[ia];
						auto svb = o_vertices[ib];
						auto svc = o_vertices[ic];
						auto p = partitionPoints[partitionID].data;
						if (!IsPointOnLineSegment(partitionPoints[partitionID].data, o_vertices[ia].m_Position, o_vertices[ib].m_Position))
						{
							o_indices.Add(ia);
							o_indices.Add(ib);
							o_indices.Add(currentVertexID);
						}
						if (!IsPointOnLineSegment(partitionPoints[partitionID].data, o_vertices[ic].m_Position, o_vertices[ia].m_Position))
						{
							o_indices.Add(ic);
							o_indices.Add(ia);
							o_indices.Add(currentVertexID);
						}
						if (!IsPointOnLineSegment(partitionPoints[partitionID].data, o_vertices[ib].m_Position, o_vertices[ic].m_Position))
						{
							o_indices.Add(ib);
							o_indices.Add(ic);
							o_indices.Add(currentVertexID);
						}
						o_indices.RemoveAt(j, 3);
					}
				}
				
			}

		}

	
	}
	/*auto b1 = DVector::CrossProduct(ovb - ova, ovc - ova);;

	double area1 = FMath::Sqrt(b1.X * b1.X + b1.Y * b1.Y + b1.Z * b1.Z);
	double area2 = 0;
	for (int i = 0; i < o_indices.Num() - 2; i += 3)
	{
		int j = i;
		uint32 ia = o_indices[j];
		uint32 ib = o_indices[j + 1];
		uint32 ic = o_indices[j + 2];
		

		auto sva = o_vertices[ia].m_Position;
		auto svb = o_vertices[ib].m_Position;
		auto svc = o_vertices[ic].m_Position;
		b1 = DVector::CrossProduct(svb - sva, svc - sva);
		area2 += FMath::Sqrt(b1.X * b1.X + b1.Y * b1.Y + b1.Z * b1.Z);
	}
	if (FMath::Abs(area1 - area2) > 1)
	{

		if (m_currentVertexBorderID != 1)
		{
			return;
		}
		m_a++;
		if ( m_a != m_block)
		{
			return;
		}
		
		for (int i = 0; i < o_indices.Num() - 2; i += 3)
		{
			int j = i;
			uint32 ia = o_indices[j];
			uint32 ib = o_indices[j + 1];
			uint32 ic = o_indices[j + 2];


			auto sva = o_vertices[ia].m_Position;
			auto svb = o_vertices[ib].m_Position;
			auto svc = o_vertices[ic].m_Position;
			{
				auto offset = FVector(2400, 0, 0);
				DrawDebugLine(m_world, (sva + offset).FVectorConversion(), (svb + offset).FVectorConversion(), FColor(0, 0, 0, 1), true, -1, 0, 10);
				DrawDebugLine(m_world, (svb + offset).FVectorConversion(), (svc + offset).FVectorConversion(), FColor(0, 0, 0, 1), true, -1, 0, 10);
				DrawDebugLine(m_world, (svc + offset).FVectorConversion(), (sva + offset).FVectorConversion(), FColor(0, 0, 0, 1), true, -1, 0, 10);
			}
		}
	}
	
	if (m_currentVertexBorderID == 1)
	{
		m_b++;
	}
	
	if (m_b < m_block)
	{
		return;
	}
	for (int i = 0; i < o_indices.Num() - 2; i += 3)
	{
		int j = i;
		uint32 ia = o_indices[j];
		uint32 ib = o_indices[j + 1];
		uint32 ic = o_indices[j + 2];


		auto sva = o_vertices[ia].m_Position;
		auto svb = o_vertices[ib].m_Position;
		auto svc = o_vertices[ic].m_Position;
		{
			auto offset = FVector(2400, 0, 0);
			DrawDebugLine(m_world, (sva + offset).FVectorConversion(), (svb + offset).FVectorConversion(), FColor(0, 0, 0, 1), true, -1, 0, 10);
			DrawDebugLine(m_world, (svb + offset).FVectorConversion(), (svc + offset).FVectorConversion(), FColor(0, 0, 0, 1), true, -1, 0, 10);
			DrawDebugLine(m_world, (svc + offset).FVectorConversion(), (sva + offset).FVectorConversion(), FColor(0, 0, 0, 1), true, -1, 0, 10);
		}
	}*/
}
int GeometryUtility::m_a = -1;
int GeometryUtility::m_b = -1;

int GeometryUtility::FindFather(TArray<int>& i_status, int i_a)
{
	int p = i_a;
	while (i_status[p] != p)
	{
		p = i_status[p];
	}
	while (i_a != p)
	{
		int pi = i_a;
		i_a = i_status[i_a];
		i_status[pi] = p;
	}
	return i_a;
}

void GeometryUtility::BorderLink(TArray<int>& i_disjointSetStatus, TMap<int, int>& i_indexPartitionPointMapping, TArray<int>& i_intersetctionsStatus, int i_indexA, int i_indexB)
{
	if (i_indexPartitionPointMapping.Contains(i_indexA) && i_indexPartitionPointMapping.Contains(i_indexB))
	{
		int ifa = FindFather(i_intersetctionsStatus, i_indexPartitionPointMapping[i_indexA]);
		int ifc = FindFather(i_intersetctionsStatus, i_indexPartitionPointMapping[i_indexB]);
		if (ifa == ifc)
		{
			DisjointSetLink(i_disjointSetStatus, i_indexB, i_indexA);
		}
	}
}

void GeometryUtility::DisjointSetLink(TArray<int> &i_status, int i_a, int i_b)
{
	int fa = FindFather(i_status, i_a);
	int fb = FindFather(i_status, i_b);
	i_status[fa] = fb;
}

bool GeometryUtility::IsPointOnLineSegment(const DVector &i_point, const DVector &i_v0, const DVector &i_v1)
{
	DVector lineA = i_point - i_v0;
	DVector standard = i_v1 - i_v0;
	if (eps(lineA.Size() - standard.Size()) <= 0)
	{
		lineA.Normalize();
		standard.Normalize();
		if (lineA == standard)
		{
			return true;
		}
	}
	return false;
}

void GeometryUtility::MeshSectionIntersection(const FProcMeshSection& i_a, const FProcMeshSection& i_b, FProcMeshSection& o_result, TArray<bool> &o_oldvertexIdentifier)
{
	m_a = -1;
	m_b = -1;
	// initialization
	TArray<bool> verticesInPlaneStatus;
	TArray<uint32> indexConvdersion;
	TArray<DVertex> addedVertices;
	TArray<uint32> addedIndices;
	verticesInPlaneStatus.Empty();
	addedVertices.Empty();
	addedIndices.Empty();
	o_result.ProcVertexBuffer.Empty();
	o_result.ProcIndexBuffer.Empty();

	for (int i = 0; i < i_a.ProcVertexBuffer.Num(); i++)
	{
		verticesInPlaneStatus.Add(IsPointInPolyhedron(i_a.ProcVertexBuffer[i].Position, i_b));
	}

	for (int i = 0; i < i_a.ProcVertexBuffer.Num(); i++)
	{
		AddNewVertex(addedVertices, i_a.ProcVertexBuffer[i]);
	}

	

	// generate intersection faces
	TArray<DVector> triangleVerticesArray;
	TArray<uint32> triangleIndicesArray, triangleNewIndicesArray;
	triangleIndicesArray.Init(0, 3);
	for (int i = 0; i < i_a.ProcIndexBuffer.Num() - 2; i += 3)
	{
		// for test
	/*	{
			if (m_block != -1 && i / 3 != m_block)
			{
				continue;
			}
			}
		}*/
		for (int j = 0; j < 3; j++)
		{
			triangleIndicesArray[j] = i_a.ProcIndexBuffer[i + j];
		}
		TArray<uint32> additionIndices = triangleIndicesArray;
		TraingleIntersectPolyhedron(i_b, addedVertices, additionIndices);
		addedIndices.Append(additionIndices);
	}

	indexConvdersion.Init(0, addedVertices.Num());
	o_oldvertexIdentifier.Empty();
	// fliter vertex that in the polyhedron
	int filteringVerticesNum = 0;
	for (int i = 0; i < addedVertices.Num(); i++) {
		indexConvdersion[i] = filteringVerticesNum;

		if (i >= i_a.ProcVertexBuffer.Num() || !verticesInPlaneStatus[i])
		{
			o_result.ProcVertexBuffer.Add(addedVertices[i].ToProcVertex());
			if (i >= i_a.ProcVertexBuffer.Num())
			{
				o_oldvertexIdentifier.Add(false);
			}
			else {
				o_oldvertexIdentifier.Add(true);
			}
			filteringVerticesNum++;
		}
	}

	bool isInMesh = false;
	int NeedNum = 0;
	for (int i = 0; i < addedIndices.Num() - 2; i += 3)
	{
		isInMesh = false;
		for (int j = 0; j < 3; j++)
		{
			if (addedIndices[i + j] < (uint32)i_a.ProcVertexBuffer.Num())
			{
				isInMesh = isInMesh || verticesInPlaneStatus[addedIndices[i + j]];
			}
		}
		
		if (!isInMesh)
		{
			o_result.ProcIndexBuffer.Add(indexConvdersion[addedIndices[i]]);
			o_result.ProcIndexBuffer.Add(indexConvdersion[addedIndices[i + 1]]);
			o_result.ProcIndexBuffer.Add(indexConvdersion[addedIndices[i + 2]]);
		} 
		else
		{
			int k = 0, l = 0;
			for (int j = 0; j < 3; j++)
			{
				if (addedIndices[i + j] < (uint32)i_a.ProcVertexBuffer.Num())
				{
					k++;
					if (verticesInPlaneStatus[addedIndices[i + j]])
					{
						l++;
					}
				}
			}
			if (k == 3 && l != 3)
			{
				int32 i1 = addedIndices[i];
				int32 i2 = addedIndices[i + 1];
				int32 i3 = addedIndices[i + 2];
				FProcMeshVertex v1 = addedVertices[i1].ToProcVertex();
				FProcMeshVertex v2 = addedVertices[i2].ToProcVertex();
				FProcMeshVertex v3 = addedVertices[i3].ToProcVertex();
				bool s1 = verticesInPlaneStatus[addedIndices[i]];
				bool s2 = verticesInPlaneStatus[addedIndices[i + 1]];
				bool s3 = verticesInPlaneStatus[addedIndices[i + 2]];
				int noChangeToNeedIntersection = 1;
				NeedNum++;
			}
		}
			
	}
	NeedNum += 0;
	
	return;
}

FProcMeshSection GeometryUtility::MeshCombination(FProcMeshSection i_meshA, FProcMeshSection i_meshB, int i_insertMode)
{
	FProcMeshSection resultA, resultB;
	FProcMeshSection resultCombine, finalResult;
	TArray<bool> vertexOldIdentifierA;
	TArray<bool> vertexOldIdentifierB;
	vertexOldIdentifierA.Init(false, i_meshA.ProcIndexBuffer.Num() / 3);
	vertexOldIdentifierB.Init(false, i_meshB.ProcIndexBuffer.Num() / 3);
	m_currentVertexBorderID = 0;
	GeometryUtility::MeshSectionIntersection(i_meshA, i_meshB, resultA, vertexOldIdentifierA);
	m_currentVertexBorderID = 1;
	GeometryUtility::MeshSectionIntersection(i_meshB, i_meshA, resultB, vertexOldIdentifierB);
	finalResult.ProcIndexBuffer.Empty();
	finalResult.ProcVertexBuffer.Empty();

	TArray<int32> reversedIndex, vertexPlaneOccupationStatus;
	TArray<bool> vertexOldIdentifier;
	TArray<FProcMeshVertex> verticesExcludingSamePoint;
	TArray<int32> advanceIndexMapping;
	TArray<int> mergeIndex, vertexBorderStatus, planeIntersectionMergeIndexA, planeIntersectionMergeIndexB;
	TArray<TSet<int>> vertexBorderEdgeLinkStatus;
	TArray<bool> planeStatus;
	// tackle final vertices of mesh
	{
		//for (int t = 0; t < resultB.ProcVertexBuffer.Num(); t++)
		//{
		//	resultB.ProcVertexBuffer[t].Position += FVector(1000, 1000, 0);
		//}
		resultCombine.ProcVertexBuffer.Empty();
		resultCombine.ProcVertexBuffer.Append(resultA.ProcVertexBuffer);
		resultCombine.ProcVertexBuffer.Append(resultB.ProcVertexBuffer);

		TArray<sortVertex<FVector>> p;
		p.Empty();
		p.Init(sortVertex<FVector>(), 0);

		//p.Init(0, finalMesh.ProcVertexBuffer.Num());
		for (int pi = 0; pi < resultCombine.ProcVertexBuffer.Num(); pi++)
		{
			p.Add(sortVertex<FVector>(
				resultCombine.ProcVertexBuffer[pi].Position,
				pi));
		}
		p.Sort([](const sortVertex<FVector>& a, const sortVertex<FVector>& b) {
			if (GeometryUtility::eps(a.data.X - b.data.X) < 0)
			{
				return true;
			}
			if (GeometryUtility::eps(a.data.X - b.data.X) == 0 && GeometryUtility::eps(a.data.Y - b.data.Y) < 0)
			{
				return true;
			}
			if (GeometryUtility::eps(a.data.X - b.data.X) == 0 && GeometryUtility::eps(a.data.Y - b.data.Y) == 0 && GeometryUtility::eps(a.data.Z - b.data.Z) < 0)
			{
				return true;
			}

			if (GeometryUtility::eps(a.data.X - b.data.X) == 0 && GeometryUtility::eps(a.data.Y - b.data.Y) == 0 && GeometryUtility::eps(a.data.Z - b.data.Z) == 0 && a.index < b.index)
			{
				return true;
			}
			return false;
		});
		reversedIndex.Init(0, p.Num());

		vertexPlaneOccupationStatus.Init(0, p.Num());
		vertexOldIdentifier.Init(false, p.Num());
		
		
		
		int32 verticesNumExcludingSamePoint = -1;
		for (int pi = 0; pi < p.Num(); pi++)
		{
			if (pi == 0 || !(p[pi].data == p[pi - 1].data))
			{
				verticesNumExcludingSamePoint++;
				verticesExcludingSamePoint.Add(resultCombine.ProcVertexBuffer[p[pi].index]);
			}
			vertexPlaneOccupationStatus[verticesNumExcludingSamePoint] |= 1 << ((int)(p[pi].index >= resultA.ProcVertexBuffer.Num()));

			if (p[pi].index < resultA.ProcVertexBuffer.Num())
			{
				vertexOldIdentifier[verticesNumExcludingSamePoint] |= vertexOldIdentifierA[p[pi].index];
			}
			else
			{
				vertexOldIdentifier[verticesNumExcludingSamePoint] |= vertexOldIdentifierB[p[pi].index - resultA.ProcVertexBuffer.Num()];
			}
			reversedIndex[p[pi].index] = verticesNumExcludingSamePoint;
		}

		// filter unnecessary border points
		{
			int32 finalMeshVerticesNum = -1;
			advanceIndexMapping.Init(-1, verticesExcludingSamePoint.Num());
			for (int pi = 0; pi < verticesExcludingSamePoint.Num(); pi++)
			{
				
				//if (vertexOldIdentifier[pi] || vertexPlaneOccupationStatus[pi] == 3)
				//{
					finalResult.ProcVertexBuffer.Add(verticesExcludingSamePoint[pi]);
					advanceIndexMapping[pi] = finalResult.ProcVertexBuffer.Num() - 1;
					
				//}

			}
		}
		planeStatus.Init(false, resultA.ProcIndexBuffer.Num() / 3 + resultB.ProcIndexBuffer.Num() / 3);
		mergeIndex.Init(-1, verticesExcludingSamePoint.Num());

		planeIntersectionMergeIndexA.Init(-1, i_meshB.ProcIndexBuffer.Num() / 3);
		planeIntersectionMergeIndexB.Init(-1, i_meshA.ProcIndexBuffer.Num() / 3);
		vertexBorderEdgeLinkStatus.Init(TSet<int>(), verticesExcludingSamePoint.Num());
		vertexBorderStatus.Init(-1, verticesExcludingSamePoint.Num());
		for (int i = 0; i < mergeIndex.Num(); i++)
		{
			mergeIndex[i] = i;
		}
		for (int i = 0; i < vertexBorderStatus.Num(); i++)
		{
			vertexBorderStatus[i] = i;
		}
		for (int i = 0; i < resultA.ProcVertexBuffer.Num(); i++)
		{
			vertexBorderEdgeLinkStatus[reversedIndex[i]].Append(m_vertexBorder[0][i]);
		}


		for (int i = 0; i < resultA.ProcIndexBuffer.Num() - 2; i += 3)
		{
			int ria = reversedIndex[resultA.ProcIndexBuffer[i]];
			int rib = reversedIndex[resultA.ProcIndexBuffer[i + 1]];
			int ric = reversedIndex[resultA.ProcIndexBuffer[i + 2]];
			DVector meshMiddlePoint = (verticesExcludingSamePoint[ria].Position + verticesExcludingSamePoint[rib].Position + verticesExcludingSamePoint[ric].Position) / 3;
			/*{
				auto offset = FVector(2400, 0, 0);
				DrawDebugLine(m_world, resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[i]].Position + offset, resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[i + 1]].Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 1);
				DrawDebugLine(m_world, resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[i + 1]].Position + offset, resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[i + 2]].Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 1);
				DrawDebugLine(m_world, resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[i + 2]].Position + offset, resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[i]].Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 1);
			}*/
			if (!(vertexOldIdentifier[ria] && vertexOldIdentifier[rib] && vertexOldIdentifier[ric]) && !IsPointInPolyhedron(meshMiddlePoint, i_meshB))
			{
				planeStatus[i / 3] = true;
				if (vertexPlaneOccupationStatus[ria] != 3 || vertexPlaneOccupationStatus[rib] != 3 || vertexPlaneOccupationStatus[ric] != 3)
				{

					updateNew(vertexBorderStatus, planeIntersectionMergeIndexA, vertexBorderEdgeLinkStatus, vertexOldIdentifier, vertexPlaneOccupationStatus, ria);
					updateNew(vertexBorderStatus, planeIntersectionMergeIndexA, vertexBorderEdgeLinkStatus, vertexOldIdentifier, vertexPlaneOccupationStatus, rib);
					updateNew(vertexBorderStatus, planeIntersectionMergeIndexA, vertexBorderEdgeLinkStatus, vertexOldIdentifier, vertexPlaneOccupationStatus, ric);
				}
				
				
			}
		}

		for (int i = 0; i < resultA.ProcIndexBuffer.Num() - 2; i += 3)
		{
			int ria = reversedIndex[resultA.ProcIndexBuffer[i]];
			int rib = reversedIndex[resultA.ProcIndexBuffer[i + 1]];
			int ric = reversedIndex[resultA.ProcIndexBuffer[i + 2]];

			if (planeStatus[i / 3])
			{
				// for plane status
				updateNew2(mergeIndex, vertexBorderStatus, vertexPlaneOccupationStatus, vertexOldIdentifier, ria, rib, ric);
				updateNew2(mergeIndex, vertexBorderStatus, vertexPlaneOccupationStatus, vertexOldIdentifier, rib, ric, ria);
				updateNew2(mergeIndex, vertexBorderStatus, vertexPlaneOccupationStatus, vertexOldIdentifier, ric, ria, rib);
				
			}
					
		}

		for (int i = 0; i < resultB.ProcIndexBuffer.Num() - 2; i += 3)
		{
			int ria = reversedIndex[resultB.ProcIndexBuffer[i] + resultA.ProcVertexBuffer.Num()];
			int rib = reversedIndex[resultB.ProcIndexBuffer[i + 1] + resultA.ProcVertexBuffer.Num()];
			int ric = reversedIndex[resultB.ProcIndexBuffer[i + 2] + resultA.ProcVertexBuffer.Num()];
			DVector meshMiddlePoint = (verticesExcludingSamePoint[ria].Position + verticesExcludingSamePoint[rib].Position + verticesExcludingSamePoint[ric].Position) / 3;
			//{
			//	auto offset = FVector(2400, 0, 0);
			//	DrawDebugLine(m_world, resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[i]].Position + offset, resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[i + 1]].Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			//	DrawDebugLine(m_world, resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[i + 1]].Position + offset, resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[i + 2]].Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			//	DrawDebugLine(m_world, resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[i + 2]].Position + offset, resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[i]].Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			//}
			if (!(vertexOldIdentifier[ria] && vertexOldIdentifier[rib] && vertexOldIdentifier[ric]) && !IsPointInPolyhedron(meshMiddlePoint, i_meshA))
			{
				planeStatus[resultA.ProcIndexBuffer.Num() / 3 + i / 3] = true;
				/*{
					auto offset = FVector(2400, 0, 0);
					DrawDebugLine(m_world, va.Position + offset, vb.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
					DrawDebugLine(m_world, vb.Position + offset, vc.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
					DrawDebugLine(m_world, vc.Position + offset, va.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
				}*/

				if (vertexPlaneOccupationStatus[ria] != 3 || vertexPlaneOccupationStatus[rib] != 3 || vertexPlaneOccupationStatus[ric] != 3)
				{
					updateNew(vertexBorderStatus, planeIntersectionMergeIndexB, vertexBorderEdgeLinkStatus, vertexOldIdentifier, vertexPlaneOccupationStatus, ria);
					updateNew(vertexBorderStatus, planeIntersectionMergeIndexB, vertexBorderEdgeLinkStatus, vertexOldIdentifier, vertexPlaneOccupationStatus, rib);
					updateNew(vertexBorderStatus, planeIntersectionMergeIndexB, vertexBorderEdgeLinkStatus, vertexOldIdentifier, vertexPlaneOccupationStatus, ric);
				}
				
			}
		}
		for (int i = 0; i < resultB.ProcIndexBuffer.Num() - 2; i += 3)
		{
			int ria = reversedIndex[resultB.ProcIndexBuffer[i] + resultA.ProcVertexBuffer.Num()];
			int rib = reversedIndex[resultB.ProcIndexBuffer[i + 1] + resultA.ProcVertexBuffer.Num()];
			int ric = reversedIndex[resultB.ProcIndexBuffer[i + 2] + resultA.ProcVertexBuffer.Num()];

			if (planeStatus[resultA.ProcIndexBuffer.Num() / 3 + i / 3])
			{
				// for plane status
				updateNew2(mergeIndex, vertexBorderStatus, vertexPlaneOccupationStatus, vertexOldIdentifier, ria, rib, ric);
				updateNew2(mergeIndex, vertexBorderStatus, vertexPlaneOccupationStatus, vertexOldIdentifier, rib, ric, ria);
				updateNew2(mergeIndex, vertexBorderStatus, vertexPlaneOccupationStatus, vertexOldIdentifier, ric, ria, rib);

			}

		}

	}
	// tackle final indices of mesh
	{
		if (i_insertMode != 1)
		{
			for (int j = 0; j < resultA.ProcIndexBuffer.Num(); j += 3)
			{
				auto ria = mergeIndex[reversedIndex[resultA.ProcIndexBuffer[j]]];
				auto rib = mergeIndex[reversedIndex[resultA.ProcIndexBuffer[j + 1]]];
				auto ric = mergeIndex[reversedIndex[resultA.ProcIndexBuffer[j + 2]]];
				auto va = verticesExcludingSamePoint[ria];
				auto vb = verticesExcludingSamePoint[rib];
				auto vc = verticesExcludingSamePoint[ric];
				
				if ((ria != rib && ria != ric) && ((vertexOldIdentifier[ria] ||
					vertexOldIdentifier[rib] ||
					vertexOldIdentifier[ric]) || planeStatus[j / 3]))
				{
					finalResult.ProcIndexBuffer.Add(advanceIndexMapping[ria]);
					finalResult.ProcIndexBuffer.Add(advanceIndexMapping[rib]);
					finalResult.ProcIndexBuffer.Add(advanceIndexMapping[ric]);
				
				}
				
			}
		}
		
		if (i_insertMode != 2)
		{
			for (int j = 0; j < resultB.ProcIndexBuffer.Num(); j += 3)
			{
				auto ria = mergeIndex[FindFather(vertexBorderStatus, reversedIndex[resultB.ProcIndexBuffer[j] + resultA.ProcVertexBuffer.Num()])];
				auto rib = mergeIndex[FindFather(vertexBorderStatus, reversedIndex[resultB.ProcIndexBuffer[j + 1] + resultA.ProcVertexBuffer.Num()])];
				auto ric = mergeIndex[FindFather(vertexBorderStatus, reversedIndex[resultB.ProcIndexBuffer[j + 2] + resultA.ProcVertexBuffer.Num()])];
				auto va = verticesExcludingSamePoint[ria];
				auto vb = verticesExcludingSamePoint[rib];
				auto vc = verticesExcludingSamePoint[ric];
				DVector meshMiddlePoint = (va.Position + vb.Position + vc.Position) / 3;
				if ((ria != rib && ria != ric) && ((vertexOldIdentifier[ria] ||
					vertexOldIdentifier[rib] ||
					vertexOldIdentifier[ric]) || planeStatus[resultA.ProcIndexBuffer.Num() / 3 + j / 3]))
				{
					finalResult.ProcIndexBuffer.Add(advanceIndexMapping[ria]);
					finalResult.ProcIndexBuffer.Add(advanceIndexMapping[rib]);
					finalResult.ProcIndexBuffer.Add(advanceIndexMapping[ric]);

				}
				else
				{
					//{
					//	auto offset = FVector(2400, 0, 0);
					//	DrawDebugLine(m_world, va.Position + offset, vb.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
					//	DrawDebugLine(m_world, vb.Position + offset, vc.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
					//	DrawDebugLine(m_world, vc.Position + offset, va.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
					//}
				}
				//if ((ria != rib && ria != ric) && (vertexOldIdentifier[ria] ||
				//	vertexOldIdentifier[rib] ||
				//	vertexOldIdentifier[ric]) || planeStatus[resultA.ProcIndexBuffer.Num() / 3 + j / 3])
				//{
				//	if (advanceIndexMapping[ria] == -1 || advanceIndexMapping[rib] == -1 || advanceIndexMapping[ric] == -1)
				//	{
				//		int b = 0;
				//	}
				//	else {
				//		/*DrawDebugLine(m_world, va.Position + offset, vb.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 1);
				//		DrawDebugLine(m_world, vb.Position + offset, vc.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 1);
				//		DrawDebugLine(m_world, vc.Position + offset, va.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 1);*/
				//		finalResult.ProcIndexBuffer.Add(advanceIndexMapping[ria]);
				//		finalResult.ProcIndexBuffer.Add(advanceIndexMapping[rib]);
				//		finalResult.ProcIndexBuffer.Add(advanceIndexMapping[ric]);
				//	}
				//}
	
			}
		}
	}
	return finalResult;
}

void  GeometryUtility::updateNew(TArray<int>& o_vertexBorderStatus, TArray<int> &o_planeIntersectionMergeIndex, const TArray<TSet<int>> i_vertexBorderEdgeLinkStatus, const TArray<bool>& i_vertexOldIdentifier, const TArray<int>& i_vertexPlaneOccupationStatus, const int& i_ria)
{
	if (!i_vertexOldIdentifier[i_ria] && i_vertexPlaneOccupationStatus[i_ria] != 3)
	{
		if (i_vertexBorderEdgeLinkStatus[i_ria].Num() > 1)
		{
			int j = 0;
		}
		int last = -1;
		for (auto& Elem : i_vertexBorderEdgeLinkStatus[i_ria])
		{

			if (o_planeIntersectionMergeIndex[Elem] == -1)
			{	
				o_planeIntersectionMergeIndex[Elem] = i_ria;
			}
			else
			{
				DisjointSetLink(o_vertexBorderStatus, o_planeIntersectionMergeIndex[Elem], i_ria);
			}
		
		}
	}

}

void GeometryUtility::updateNew2(TArray<int> &o_meshIndex, TArray<int> &i_vertexBoderStatus, const TArray<int> &i_vertexPlaneOccupationStatus, const TArray<bool> & i_vertexOldIdentifier, int i_ria, int i_rib, int i_ric)
{
	if (i_vertexPlaneOccupationStatus[i_ria] == 3)
	{
		if (!i_vertexOldIdentifier[i_rib] && i_vertexPlaneOccupationStatus[i_rib] != 3 && i_vertexPlaneOccupationStatus[i_ric] != 3)
		{
			o_meshIndex[FindFather(i_vertexBoderStatus, i_rib)] = i_ria;
			
		}
		if (!i_vertexOldIdentifier[i_ric] && i_vertexPlaneOccupationStatus[i_ric] != 3 && i_vertexPlaneOccupationStatus[i_rib] != 3)
		{
			o_meshIndex[FindFather(i_vertexBoderStatus, i_ric)] = i_ria;
		}
	}
}

void GeometryUtility::updateMergeIndex(TArray<int>& o_borderStatus, int& o_belonging, const TArray<bool>& i_vertexOldIdentifier, const TArray<int>& i_vertexPlaneOccupationStatus, const int& i_ria, const int& i_rib, const int i_ric)
{
	if (!i_vertexOldIdentifier[i_ria] && i_vertexPlaneOccupationStatus[i_ria] != 3)
	{
		if (i_vertexPlaneOccupationStatus[i_rib] == 3)
		{
			o_belonging = i_rib;
		}
		if (i_vertexPlaneOccupationStatus[i_ric] == 3)
		{
			o_belonging = i_ric;
		}
	}

}

void GeometryUtility::hcFilter(FProcMeshSection i_in, FProcMeshSection& o_out, float alpha, float beta)
{
	TArray<DVector> wv;
	TArray<DVector> bv;
	TArray<DVector> sv;
	wv.Empty();
	bv.Empty();
	sv.Empty();
	for (int i = 0; i < i_in.ProcVertexBuffer.Num(); i++)
	{ 
		sv.Add(i_in.ProcVertexBuffer[i].Position);
	}
	bv.Init(DVector(), sv.Num());

	//wv = centralFilter(sv, i_in.ProcIndexBuffer);
	wv = laplacianFilterWithDistance(sv, i_in.ProcIndexBuffer);
	for (int i = 0; i < wv.Num(); i++)
	{
		bv[i].X = wv[i].X - (alpha * sv[i].X + (1 - alpha) * sv[i].X);
		bv[i].Y = wv[i].Y - (alpha * sv[i].Y + (1 - alpha) * sv[i].Y);
		bv[i].Z = wv[i].Z - (alpha * sv[i].Z + (1 - alpha) * sv[i].Z);
	}
	TArray<DVector> adjacentVertices;
	TArray<uint32> adjacentIndexes;
	adjacentIndexes.Empty();
	double dx = 0.0f;
	double dy = 0.0f;
	double dz = 0.0f;

	for (int j = 0; j < bv.Num(); j++)
	{
		adjacentIndexes.Empty();

		findAdjacentNeighbors(sv, i_in.ProcIndexBuffer, sv[j], adjacentVertices, adjacentIndexes);
		if (adjacentIndexes.Num() != 0)
		{
			dx = 0.0f;
			dy = 0.0f;
			dz = 0.0f;

			for (int k = 0; k < adjacentIndexes.Num(); k++)
			{
				dx += bv[adjacentIndexes[k]].X;
				dy += bv[adjacentIndexes[k]].Y;
				dz += bv[adjacentIndexes[k]].Z;

			}

			wv[j].X -= beta * bv[j].X + ((1 - beta) / adjacentIndexes.Num()) * dx;
			wv[j].Y -= beta * bv[j].Y + ((1 - beta) / adjacentIndexes.Num()) * dy;
			wv[j].Z -= beta * bv[j].Z + ((1 - beta) / adjacentIndexes.Num()) * dz;
		}
	}
	o_out = i_in;
	for (int i = 0; i < wv.Num(); i++)
	{
		o_out.ProcVertexBuffer[i].Position = wv[i].FVectorConversion();
	}
}



TArray<DVector> GeometryUtility::centralFilter(TArray<DVector> i_vertices, TArray<uint32> i_indices)
{

	TArray<DVector> wv;
	wv.Init(DVector(), i_vertices.Num());


	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;
	TArray<uint32> adjacentFaceIndices;
	for (int vi = 0; vi < i_vertices.Num(); vi++)
	{
		wv[vi] = i_vertices[vi];
		//if (vi != m_block) {
		//	continue;
		//}
		// Find the sv neighboring vertices
		findAdjacentFace(i_vertices, i_indices, i_vertices[vi], adjacentFaceIndices);

		if (adjacentFaceIndices.Num() != 0)
		{
			dx = 0.0f;
			dy = 0.0f;
			dz = 0.0f;


			// Add the vertices and divide by the number of vertices
			for (int j = 0; j < adjacentFaceIndices.Num() - 2; j+=3)
			{
				dx += (i_vertices[adjacentFaceIndices[j]].X + i_vertices[adjacentFaceIndices[j + 1]].X + i_vertices[adjacentFaceIndices[j + 2]].X) / 3;
				dy += (i_vertices[adjacentFaceIndices[j]].Y + i_vertices[adjacentFaceIndices[j + 1]].Y + i_vertices[adjacentFaceIndices[j + 2]].Y) / 3;
				dz += (i_vertices[adjacentFaceIndices[j]].Z + i_vertices[adjacentFaceIndices[j + 1]].Z + i_vertices[adjacentFaceIndices[j + 2]].Z) / 3;
			}

			wv[vi].X = dx / (adjacentFaceIndices.Num() / 3);
			wv[vi].Y = dy / (adjacentFaceIndices.Num() / 3);
			wv[vi].Z = dz / (adjacentFaceIndices.Num() / 3);
		}

	}

	return wv;
}

TArray<DVector> GeometryUtility::laplacianFilterWithDistance(TArray<DVector> i_vertices, TArray<uint32> i_indices, float i_lambda)
{

	TArray<DVector> wv;
	wv.Init(DVector(), i_vertices.Num());


	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;
	TArray<DVector> adjacentVertices;
	TArray<uint32> adjacentIndices;
	for (int vi = 0; vi < i_vertices.Num(); vi++)
	{
		wv[vi] = i_vertices[vi];
		DVector origin = i_vertices[vi];
		//if (vi != m_block) {
		//	continue;
		//}
		// Find the sv neighboring vertices
		findAdjacentNeighbors(i_vertices, i_indices, i_vertices[vi], adjacentVertices, adjacentIndices);
		float sumWeight = 0.f;
		if (adjacentVertices.Num() != 0)
		{
			dx = 0.0f;
			dy = 0.0f;
			dz = 0.0f;


			// Add the vertices and divide by the number of vertices
			for (int j = 0; j < adjacentVertices.Num(); j++)
			{
				float weight = FVector::Distance(origin.FVectorConversion(), adjacentVertices[j].FVectorConversion());
				dx += weight * (adjacentVertices[j].X - origin.X);
				dy += weight * (adjacentVertices[j].Y - origin.Y);
				dz += weight * (adjacentVertices[j].Z - origin.Z);
				sumWeight += weight;
			}
			dx /= sumWeight;
			dy /= sumWeight;
			dz /= sumWeight;
			wv[vi].X = origin.X + i_lambda * dx;
			wv[vi].Y = origin.Y + i_lambda * dy;
			wv[vi].Z = origin.Z + i_lambda * dz;
			//wv[vi] = wv[vi] + FVector(10, 10, 10);
		}

	}


	return wv;
}


TArray<DVector> GeometryUtility::laplacianFilter(TArray<DVector> i_vertices, TArray<uint32> i_indices)
{

	TArray<DVector> wv;
	wv.Init(DVector(), i_vertices.Num());


	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;
	TArray<DVector> adjacentVertices;
	TArray<uint32> adjacentIndices;
	for (int vi = 0; vi < i_vertices.Num(); vi++)
	{
		wv[vi] = i_vertices[vi];
		//if (vi != m_block) {
		//	continue;
		//}
		// Find the sv neighboring vertices
		findAdjacentNeighbors(i_vertices, i_indices, i_vertices[vi], adjacentVertices, adjacentIndices);

		if (adjacentVertices.Num() != 0)
		{
			dx = 0.0f;
			dy = 0.0f;
			dz = 0.0f;


			// Add the vertices and divide by the number of vertices
			for (int j = 0; j < adjacentVertices.Num(); j++)
			{
				dx += adjacentVertices[j].X;
				dy += adjacentVertices[j].Y;
				dz += adjacentVertices[j].Z;
			}

			wv[vi].X = dx / adjacentVertices.Num();
			wv[vi].Y = dy / adjacentVertices.Num();
			wv[vi].Z = dz / adjacentVertices.Num();
			//wv[vi] = wv[vi] + FVector(10, 10, 10);
		}
	
	}

	return wv;
}

void GeometryUtility::findAdjacentFace(TArray<DVector> i_vertices, TArray<uint32> i_indices, DVector vertex, TArray<uint32>& adjacentFaceIndex)
{
	TSet<int32> indicesSet;
	adjacentFaceIndex.Empty();

	for (int i = 0; i < i_vertices.Num(); i++)
	{
		if (i_vertices[i] == vertex)
		{
			for (int k = 0; k < i_indices.Num(); k = k + 3)
			{
				if (i == i_indices[k])
				{
					adjacentFaceIndex.Add(i);
					adjacentFaceIndex.Add(i_indices[k + 1]);
					adjacentFaceIndex.Add(i_indices[k + 2]);
				}

				if (i == i_indices[k + 1])
				{
					adjacentFaceIndex.Add(i);
					adjacentFaceIndex.Add(i_indices[k + 2]);
					adjacentFaceIndex.Add(i_indices[k]);
				}

				if (i == i_indices[k + 2])
				{
					adjacentFaceIndex.Add(i);
					adjacentFaceIndex.Add(i_indices[k]);
					adjacentFaceIndex.Add(i_indices[k + 1]);
				}
			}
		}
	}
	
}
void GeometryUtility::findAdjacentNeighbors(TArray<DVector> i_vertices, TArray<uint32> i_indices, DVector vertex, TArray<DVector>& adjacentV, TArray<uint32>& adjacentI)
{
	TSet<int32> indicesSet;
	adjacentV.Empty();
	adjacentI.Empty();
	// Find matching vertices
	for (int i = 0; i < i_vertices.Num(); i++)
	{
		if (i_vertices[i] == vertex)
		{
			int32 v1 = 0;
			int32 v2 = 0;
			bool marker = false;
			for (int k = 0; k < i_indices.Num(); k = k + 3)
			{
				v1 = 0;
				v2 = 0;
				marker = false;

				if (i == i_indices[k])
				{
					v1 = i_indices[k + 1];
					v2 = i_indices[k + 2];
					marker = true;
				}

				if (i == i_indices[k + 1])
				{
					v1 = i_indices[k];
					v2 = i_indices[k + 2];
					marker = true;
				}

				if (i == i_indices[k + 2])
				{
					v1 = i_indices[k];
					v2 = i_indices[k + 1];
					marker = true;
				}

				if (marker)
				{
					indicesSet.Add(v1);
					indicesSet.Add(v2);
				}
			}
		}
	}
	for (auto& Elem : indicesSet)
	{
		adjacentI.Add(Elem);
		bool exist = false;
		for (int i = 0; i < adjacentV.Num(); i++)
		{
			if (adjacentV[i] == i_vertices[Elem])
			{
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			adjacentV.Add(i_vertices[Elem]);
		}
		
	}
}

void GeometryUtility::DivisionPointSegment(FProcMeshSection& o_out, TMap<int32, TPair<int32, int32>>& o_divisionMap, int i_index, const FProcMeshVertex& i_origin, int32& o_start, int32& o_num, float i_percision, int i_addedSegmentNum)
{
	if (o_divisionMap.Contains(i_index))
	{
		o_start = o_divisionMap[i_index].Key;
		o_num = o_divisionMap[i_index].Value;
	}
	else
	{
		FVector v1 = o_out.ProcVertexBuffer[i_index].Position;

		int32 di = -1;
		int32 dn = 0;
		FVector s = i_origin.Position;
		FVector dir = (v1 - s);
		FProcMeshVertex newV = i_origin;
		dir.Normalize();
		while (FVector::Distance(s, v1) > i_percision)
		{
			s += dir * i_percision;
			newV.Position = s;
			o_out.ProcVertexBuffer.Add(newV);
			dn++;
			if (di == -1)
			{
				di = o_out.ProcVertexBuffer.Num() - 1;
			}

			if (dn >= i_addedSegmentNum)
			{
				break;
			}
		}
		o_start = di;
		o_num = dn;
		o_divisionMap.Add(i_index, TPair<int32, int32>(di, dn));
	}
}

void GeometryUtility::DivisionPointFaces(FProcMeshSection& o_out, int32 i_centerPointIndex, float i_percision, int i_addFaceNum)
{
	int i = 0;
	FProcMeshVertex origin = o_out.ProcVertexBuffer[i_centerPointIndex];
	TMap<int32, TPair<int, int>> divisionMap;
	divisionMap.Empty();
	int originFaceNum = o_out.ProcIndexBuffer.Num();
	while (i < originFaceNum - 2)
	{
		int32 ia = o_out.ProcIndexBuffer[i];
		int32 ib = o_out.ProcIndexBuffer[i + 1];
		int32 ic = o_out.ProcIndexBuffer[i + 2];
		int32 pa = 0, pb = 0;
		bool exist = false;
		if (ia == i_centerPointIndex)
		{
			exist = true;
			pa = ib;
			pb = ic;
		}
		if (ib == i_centerPointIndex)
		{
			exist = true;
			pa = ic;
			pb = ia;
		}
		if (ic == i_centerPointIndex)
		{
			exist = true;
			pa = ia;
			pb = ib;
		}
		if (exist)
		{
			int startSegementA, startSegementB;
			int numSegementA, numSegementB;

			DivisionPointSegment(o_out, divisionMap, pa, origin, startSegementA, numSegementA, i_percision, i_addFaceNum);
			DivisionPointSegment(o_out, divisionMap, pb, origin, startSegementB, numSegementB, i_percision, i_addFaceNum);
			if (numSegementA != 0 || numSegementB != 0)
			{
				originFaceNum -= 3;
				o_out.ProcIndexBuffer.RemoveAt(i, 3);
				int jA = 0;
				int jB = 0;
				int lastA = i_centerPointIndex;
				int lastB = i_centerPointIndex;
				while (jA <= numSegementA || jB <= numSegementB)
				{
					int indexEnumerationOnSegementA = startSegementA + jA;
					if (jA >= numSegementA)
					{
						indexEnumerationOnSegementA = pa;
					}
					int indexEnumerationOnSegementB = startSegementB + jB;
					if (jB >= numSegementB)
					{
						indexEnumerationOnSegementB = pb;
					}
					jA++; jB++;
					if (lastA == lastB)
					{
						o_out.ProcIndexBuffer.Add(lastA);
						o_out.ProcIndexBuffer.Add(indexEnumerationOnSegementA);
						o_out.ProcIndexBuffer.Add(indexEnumerationOnSegementB);
					}
					else
					{
						if (lastA != indexEnumerationOnSegementA)
						{
							o_out.ProcIndexBuffer.Add(lastA);
							o_out.ProcIndexBuffer.Add(indexEnumerationOnSegementA);
							o_out.ProcIndexBuffer.Add(indexEnumerationOnSegementB);
						}
						if (lastB != indexEnumerationOnSegementB)
						{
							o_out.ProcIndexBuffer.Add(lastA);
							o_out.ProcIndexBuffer.Add(indexEnumerationOnSegementB);
							o_out.ProcIndexBuffer.Add(lastB);
						}
					}
					lastA = indexEnumerationOnSegementA;
					lastB = indexEnumerationOnSegementB;
				}
			}
			else
			{
				i += 3;
			}
		}
		else
		{
			i += 3;
		}
		
	}
}

bool GeometryUtility::DivisionSegment(
	FProcMeshSection& o_out,
	FProcMeshVertex i_va,
	FProcMeshVertex i_vb,
	int32 i_ia,
	int32 i_ib,
	int32 i_ic,
	float i_percision)
{
	float dist = FVector::Distance(i_va.Position, i_vb.Position);
	if (eps(dist - i_percision) > 0)
	{
		FVector unit = i_vb.Position - i_va.Position;
		unit.Normalize();
		int32 lastI = i_ia;
		for (int32 i = 0; i < FMath::FloorToInt(dist / i_percision); i++) 
		{
			FProcMeshVertex newv = i_va;
			newv.Position += (i + 1) * unit * i_percision;
			o_out.ProcIndexBuffer.Add(lastI);
			lastI = o_out.ProcVertexBuffer.Num();
			o_out.ProcIndexBuffer.Add(lastI);
			o_out.ProcIndexBuffer.Add(i_ic);
			o_out.ProcVertexBuffer.Add(newv);
			
		}
		o_out.ProcIndexBuffer.Add(lastI);
		o_out.ProcIndexBuffer.Add(i_ib);
		o_out.ProcIndexBuffer.Add(i_ic);
		return true;
	}
	return false;
}

void GeometryUtility::Division(FProcMeshSection i_in, FProcMeshSection& o_out, float i_percision)
{
	o_out = i_in;
	int i = 0;
	TSet<FString> needDivisionSegments;
	while (i < o_out.ProcIndexBuffer.Num() - 2)
	{
		auto ia = o_out.ProcIndexBuffer[i];
		auto ib = o_out.ProcIndexBuffer[i + 1];
		auto ic = o_out.ProcIndexBuffer[i + 2];
		auto va = o_out.ProcVertexBuffer[ia];
		auto vb = o_out.ProcVertexBuffer[ib];
		auto vc = o_out.ProcVertexBuffer[ic];
		bool exist = false;
		if (DivisionSegment(o_out, va, vb, ia, ib, ic, i_percision) ||
			DivisionSegment(o_out, vb, vc, ib, ic, ia, i_percision) ||
			DivisionSegment(o_out, vc, va, ic, ia, ib, i_percision))
		{
			o_out.ProcIndexBuffer.RemoveAt(i, 3);
		}
		else
		{
			i += 3;
		}
		
		//if ()

	}
}

void GeometryUtility::OutputMesh(const FProcMeshSection& i_mesh, UWorld* i_world, FColor i_color, FVector i_offset)
{
	for (int k = 0; k < i_mesh.ProcIndexBuffer.Num() - 2; k += 3)
	{
		//if (i_mesh.ProcIndexBuffer[k] == 10 || i_mesh.ProcIndexBuffer[k + 1] == 10 || i_mesh.ProcIndexBuffer[k + 2] == 10)
		//{ 
			FVector va = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[k]].Position;
			FVector vb = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[k + 1]].Position;
			FVector vc = i_mesh.ProcVertexBuffer[i_mesh.ProcIndexBuffer[k + 2]].Position;
			DrawDebugLine(i_world, va + i_offset, vb + i_offset, i_color, true, -1, 0, 10);
			DrawDebugLine(i_world, vb + i_offset, vc + i_offset, i_color, true, -1, 0, 10);
			DrawDebugLine(i_world, vc + i_offset, va + i_offset, i_color, true, -1, 0, 10);
	}
}