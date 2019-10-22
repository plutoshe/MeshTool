// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryUtility.h"
#include "DrawDebugHelpers.h"

DVector const DVector::Zero(0, 0, 0);
DVector const DVector::Unit(1, 1, 1);
const double GeometryUtility::eps_const = 1e-4;
UWorld* GeometryUtility::m_world = nullptr;
TArray<int> GeometryUtility::m_vertexBorder[2] = { {}, {}};
int GeometryUtility::m_currentVertexBorderID = 0;
int GeometryUtility::m_block = -1;
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
	double paramA;
	double paramB;
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
	m_vertexBorder[m_currentVertexBorderID].Add(o_vertices.Num() - 1);
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
	TArray<DVector> lineIntersections;
	TArray<sortVertex<DVector>> partitionPoints;
	TArray<sortVertex<DVector>> setDivision;
	TArray<int> intersetctionsStatus;
	setDivision.Init(sortVertex<DVector>(), 0);
	intersetctionsStatus.Init(0, 0);
	planeIntersections.Empty();
	partitionPoints.Empty();
	// get intersection set and status
	{
		for (int i = 0; i + 2 < i_b.ProcIndexBuffer.Num(); i += 3)
		{
			DVertex va = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i]];
			DVertex vb = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i + 1]];
			DVertex vc = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i + 2]];
			
			lineIntersections.Empty();
			
			if (GetLineAndPlaneIntersectionPoint(va.m_Position, vb.m_Position, ova, currentNormal, intersection))
			{
				lineIntersections.Add(intersection);
			}
			if (GetLineAndPlaneIntersectionPoint(vb.m_Position, vc.m_Position, ova, currentNormal, intersection))
			{
				lineIntersections.Add(intersection);
			}
			if (GetLineAndPlaneIntersectionPoint(va.m_Position, vc.m_Position, ova, currentNormal, intersection))
			{
				lineIntersections.Add(intersection);
			}
			if (lineIntersections.Num() > 0)
			{
				if (lineIntersections.Num() == 2) // currently only tackle line situation
				{
					planeIntersections.Add(lineIntersections[0]);
					planeIntersections.Add(lineIntersections[1]);
				}
			}
		}
		for (int i = 0; i < planeIntersections.Num(); i++)
		{
			setDivision.Add(sortVertex<DVector>(planeIntersections[i], i));
			intersetctionsStatus.Add(i);
		}
		setDivision.Sort(doubleSortexMethod);
		for (int i = 1; i < setDivision.Num(); i++)
		{
			if (setDivision[i].data == setDivision[i - 1].data)
			{
				DisjointSetLink(intersetctionsStatus, setDivision[i - 1].index, setDivision[i].index);
			}
		}
		for (int i = 0; i + 1 < planeIntersections.Num(); i += 2)
		{
			DisjointSetLink(intersetctionsStatus, i, i + 1);
		}
	}

	// get partition set for current plane
	{
		{

			TMap<int, int> originalBorderIndexMapping;
			originalBorderIndexMapping.Empty();
			for (int p_i = 0; p_i + 1 < planeIntersections.Num(); p_i+=2)
			{
				partitionPoints.Empty();
				int fpi = FindFather(intersetctionsStatus, p_i);
				// point in triangle
				for (int k = 0; k < 2; k++)
				{
					if (IsPointInTriangle(planeIntersections[p_i + k], ova, ovb, ovc) == 1)
					{
						partitionPoints.Add(sortVertex<DVector>(planeIntersections[p_i + k], fpi));
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

					if (GetLineAndLineIntersectionPoint(planeIntersections[p_i], planeIntersections[p_i + 1], svb, sva, intersection))
					{
						partitionPoints.Add(sortVertex<DVector>(intersection, fpi));
					}
					if (GetLineAndLineIntersectionPoint(planeIntersections[p_i], planeIntersections[p_i + 1], svc, sva, intersection))
					{
						partitionPoints.Add(sortVertex<DVector>(intersection, fpi));
					}
					if (GetLineAndLineIntersectionPoint(planeIntersections[p_i], planeIntersections[p_i + 1], svc, svb, intersection))
					{
						partitionPoints.Add(sortVertex<DVector>(intersection, fpi));
					}
				}
				partitionPoints.Sort(doubleSortexMethod);
				// 
				int pi = 1;
				while (pi < partitionPoints.Num()) {
					if (partitionPoints[pi].data == partitionPoints[pi - 1].data) {
						partitionPoints.RemoveAt(pi);
					}
					else {
						pi++;
					}
				}

				int linkPartition = -1;
				for (int partitionID = 0; partitionID < partitionPoints.Num(); partitionID++)
				{
					if (isnan(partitionPoints[partitionID].data.X))
					{
						UE_LOG(LogTemp, Log, TEXT("NAN Float"));
					}
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
								originalBorderIndexMapping.Add(currentVertexID, partitionPoints[partitionID].index);
							}
							exist = true;

							auto sva = o_vertices[ia];
							auto svb = o_vertices[ib];
							auto svc = o_vertices[ic];
							auto p = partitionPoints[partitionID].data;
							if (!IsPointOnLineSegment(partitionPoints[partitionID].data, o_vertices[ia].m_Position, o_vertices[ib].m_Position))
							{
								o_indices.Add(ia);
								o_indices.Add(ib);
								o_indices.Add(currentVertexID);
								BorderLink(m_vertexBorder[m_currentVertexBorderID], originalBorderIndexMapping, intersetctionsStatus, ia, currentVertexID);
								BorderLink(m_vertexBorder[m_currentVertexBorderID], originalBorderIndexMapping, intersetctionsStatus, ib, currentVertexID);

								
							}
							if (!IsPointOnLineSegment(partitionPoints[partitionID].data, o_vertices[ic].m_Position, o_vertices[ia].m_Position))
							{
								o_indices.Add(ic);
								o_indices.Add(ia);
								o_indices.Add(currentVertexID);
								BorderLink(m_vertexBorder[m_currentVertexBorderID], originalBorderIndexMapping, intersetctionsStatus, ia, currentVertexID);
								BorderLink(m_vertexBorder[m_currentVertexBorderID], originalBorderIndexMapping, intersetctionsStatus, ic, currentVertexID);
							}
							if (!IsPointOnLineSegment(partitionPoints[partitionID].data, o_vertices[ib].m_Position, o_vertices[ic].m_Position))
							{
								o_indices.Add(ib);
								o_indices.Add(ic);
								o_indices.Add(currentVertexID);
								BorderLink(m_vertexBorder[m_currentVertexBorderID], originalBorderIndexMapping, intersetctionsStatus, ib, currentVertexID);
								BorderLink(m_vertexBorder[m_currentVertexBorderID], originalBorderIndexMapping, intersetctionsStatus, ic, currentVertexID);
							}
							o_indices.RemoveAt(j, 3);
						}
					}
					if (currentVertexID != -1)
					{
						if (linkPartition != -1)
						{
							DisjointSetLink(m_vertexBorder[m_currentVertexBorderID], currentVertexID, linkPartition);
						}
						else {
							linkPartition = currentVertexID;
						}
					}
				}

			}

		}
	}
}

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

void GeometryUtility::MeshSectionIntersection(const FProcMeshSection& i_a, const FProcMeshSection& i_b, FProcMeshSection& o_result, TArray<int> &t_planeAStatus, TArray<int> &t_planeBStatus)
{
	// Decide all vertices status for pointed mesh section
	TArray<bool> verticesStatus;
	TArray<uint32> indexConvdersion;
	TArray<DVertex> addedVertices;
	TArray<uint32> addedIndices;
	verticesStatus.Empty();
	addedVertices.Empty();
	addedIndices.Empty();
	// initialization
	for (int i = 0; i < i_a.ProcVertexBuffer.Num(); i++)
	{
		verticesStatus.Add(IsPointInPolyhedron(i_a.ProcVertexBuffer[i].Position, i_b));
	}

	m_vertexBorder[m_currentVertexBorderID].Empty();
	for (int i = 0; i < i_a.ProcVertexBuffer.Num(); i++)
	{
		AddNewVertex(addedVertices, i_a.ProcVertexBuffer[i]);
	}

	o_result.ProcVertexBuffer.Empty();
	o_result.ProcIndexBuffer.Empty();

	// generate intersection faces
	TArray<DVector> triangleVerticesArray;
	TArray<uint32> triangleIndicesArray, triangleNewIndicesArray;
	triangleVerticesArray.Init(DVector(0, 0, 0), 3);
	triangleIndicesArray.Init(0, 3);
	for (int i = 0; i < i_a.ProcIndexBuffer.Num() - 2; i += 3)
	{ 
		if (m_block != -1 && i / 3 != m_block)
		{
			continue;
		}
		for (int j = 0; j < 3; j++)
		{
			triangleIndicesArray[j] = i_a.ProcIndexBuffer[i + j];
		}
		TArray<uint32> additionIndices = triangleIndicesArray;

		TraingleIntersectPolyhedron(i_b, addedVertices, additionIndices);
		addedIndices.Append(additionIndices);
	}
	int NeedNum = 0;

	indexConvdersion.Init(0, addedVertices.Num());
	t_planeAStatus.Empty();
	// fliter vertex that in the polyhedron
	int filteringVerticesNum = 0;
	for (int i = 0; i < addedVertices.Num(); i++) {
		indexConvdersion[i] = filteringVerticesNum;
		if (i >= i_a.ProcVertexBuffer.Num() || !verticesStatus[i])
		{
				
			o_result.ProcVertexBuffer.Add(addedVertices[i].ToProcVertex());
			if (i >= i_a.ProcVertexBuffer.Num())
			{
				t_planeAStatus.Add(4);
			}
			else {
				t_planeAStatus.Add(3);
			}
				
			filteringVerticesNum++;
		}
	}
	int pi = 0;
	for (int i = 0; i < addedVertices.Num(); i++)
	{
		if (i == 0 || indexConvdersion[i] != indexConvdersion[i - 1])
		{
			m_vertexBorder[m_currentVertexBorderID][indexConvdersion[i]] = indexConvdersion[m_vertexBorder[m_currentVertexBorderID][i]];
		}
	}
	for (int i = 0; i < addedIndices.Num() - 2; i += 3)
	{
		UE_LOG(LogTemp, Log, TEXT("%s %s %s"),
			*addedVertices[addedIndices[i]].m_Position.FVectorConversion().ToString(),
			*addedVertices[addedIndices[i + 1]].m_Position.FVectorConversion().ToString(),
			*addedVertices[addedIndices[i + 2]].m_Position.FVectorConversion().ToString());
	}
	// fliter face that is in the polyhedron or face that intersects two polyhedrons
	// currently only filter faces that are in the polyhedron
	// TODO: faces intersecting polyhedrons
	bool isInMesh = false;

	for (int i = 0; i < addedIndices.Num() - 2; i += 3)
	{
		isInMesh = false;
		for (int j = 0; j < 3; j++)
		{
			if (addedIndices[i + j] < (uint32)i_a.ProcVertexBuffer.Num())
			{
				isInMesh = isInMesh || verticesStatus[addedIndices[i + j]];
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
					if (verticesStatus[addedIndices[i + j]])
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
				bool s1 = verticesStatus[addedIndices[i]];
				bool s2 = verticesStatus[addedIndices[i + 1]];
				bool s3 = verticesStatus[addedIndices[i + 2]];
				int noChangeToNeedIntersection = 1;
				NeedNum++;
			}
		}
			
	}
	NeedNum += 0;
	
	return;
}

FProcMeshSection GeometryUtility::MeshCombination(FProcMeshSection i_finalMesh, FProcMeshSection i_addedMesh, int i_insertMode)
{
	FProcMeshSection resultA, resultB;
	
	TArray<int> planeAStatus;
	TArray<int> planeBStatus;
	TArray<int> planeStatus;
	planeAStatus.Init(0, i_addedMesh.ProcIndexBuffer.Num() / 3);
	planeBStatus.Init(0, i_finalMesh.ProcIndexBuffer.Num() / 3);
	m_currentVertexBorderID = 0;
	GeometryUtility::MeshSectionIntersection(i_addedMesh, i_finalMesh, resultA, planeAStatus, planeBStatus);
	m_currentVertexBorderID = 1;
	GeometryUtility::MeshSectionIntersection(i_finalMesh, i_addedMesh, resultB, planeBStatus, planeAStatus);
	/*m_currentVertexBorderID = 0;
	GeometryUtility::MeshSectionIntersection(i_addedMesh, i_finalMesh, resultA, planeAStatus, planeBStatus, 1);
	m_currentVertexBorderID = 1;
	GeometryUtility::MeshSectionIntersection(i_finalMesh, i_addedMesh, resultB, planeBStatus, planeAStatus, 1);*/
	i_finalMesh.ProcIndexBuffer.Empty();
	i_finalMesh.ProcVertexBuffer.Empty();
	i_addedMesh.ProcVertexBuffer.Empty();
	i_addedMesh.ProcIndexBuffer.Empty();
	i_addedMesh.ProcVertexBuffer.Append(resultA.ProcVertexBuffer);
	i_addedMesh.ProcVertexBuffer.Append(resultB.ProcVertexBuffer);
	

	TArray<sortVertex<FVector>> p;
	p.Empty();
	p.Init(sortVertex<FVector>(), 0);
	planeStatus.Init(0, 0);
	planeStatus.Append(planeAStatus);
	planeStatus.Append(planeBStatus);
	//p.Init(0, finalMesh.ProcVertexBuffer.Num());
	for (int pi = 0; pi < i_addedMesh.ProcVertexBuffer.Num(); pi++)
	{
		p.Add(sortVertex<FVector>(
			i_addedMesh.ProcVertexBuffer[pi].Position,
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
	TArray<int32> rIndex, vertexPlaneStatus;
	rIndex.Init(0, p.Num());
	vertexPlaneStatus.Init(0, p.Num());
	int32 newIndexNum = 0;
	rIndex[p[0].index] = 0;
	i_finalMesh.ProcVertexBuffer.Add(i_addedMesh.ProcVertexBuffer[p[0].index]);
	vertexPlaneStatus[0] |= 1 << (int)((p[0].index >= resultA.ProcVertexBuffer.Num()));
	for (int pi = 1; pi < p.Num(); pi++)
	{
		if (GeometryUtility::eps(p[pi].data.X - p[pi - 1].data.X) != 0 ||
			GeometryUtility::eps(p[pi].data.Y - p[pi - 1].data.Y) != 0 ||
			GeometryUtility::eps(p[pi].data.Z - p[pi - 1].data.Z) != 0)
		{
			newIndexNum++;
			i_finalMesh.ProcVertexBuffer.Add(i_addedMesh.ProcVertexBuffer[p[pi].index]);
			
		}
		else
		{
			if (p[pi].index >= resultA.ProcVertexBuffer.Num())
			{
				if (p[pi - 1].index >= resultA.ProcVertexBuffer.Num())
				{
					DisjointSetLink(m_vertexBorder[1], p[pi - 1].index - resultA.ProcVertexBuffer.Num(), p[pi].index - resultA.ProcVertexBuffer.Num());
				}
			}
			else {
				DisjointSetLink(m_vertexBorder[0], p[pi].index, p[pi - 1].index);
			}
		}
		int status = (int)((p[pi].index >= resultA.ProcVertexBuffer.Num()));
		vertexPlaneStatus[newIndexNum] |= 1 << status;
		rIndex[p[pi].index] = newIndexNum;
	}
	for (int pi = 1; pi < p.Num(); pi++)
	{
		//int ff = FindFather(m_vertexBorder[2], p[pi].index);
		int status = (int)((p[pi].index >= resultA.ProcVertexBuffer.Num()));
		int ff = FindFather(m_vertexBorder[status], p[pi].index - status * resultA.ProcVertexBuffer.Num()) + status * resultA.ProcVertexBuffer.Num();

		vertexPlaneStatus[rIndex[ff]] = FMath::Max(vertexPlaneStatus[rIndex[ff]], vertexPlaneStatus[rIndex[p[pi].index]]);
	}
	int last = -1;
	for (int j = 0; j < resultA.ProcVertexBuffer.Num(); j++)
	{

			int fa = FindFather(m_vertexBorder[0], j);

			UE_LOG(LogTemp, Log, TEXT("%d %d %s"), j, fa, *(resultA.ProcVertexBuffer[j].Position.ToString()));


		
	}
	for (int j = 0; j < resultB.ProcVertexBuffer.Num(); j++)
	{

			int fa = FindFather(m_vertexBorder[1], j );

			UE_LOG(LogTemp, Log, TEXT("%d %d %s"), j , fa, *(resultB.ProcVertexBuffer[j].Position.ToString()));



	}

	FVector offset(1000, 0, 0);
	if (i_insertMode != 1)
	{
		for (int j = 0; j < resultA.ProcIndexBuffer.Num(); j += 3)
		{
			/*if (j / 3 == m_block)
			{
				continue;
			}*/
			auto va = resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[j]];
			auto vb = resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[j + 1]];
			auto vc = resultA.ProcVertexBuffer[resultA.ProcIndexBuffer[j + 2]];

			DrawDebugLine(m_world, va.Position + offset, vb.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			DrawDebugLine(m_world, vb.Position + offset, vc.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			DrawDebugLine(m_world, vc.Position + offset, va.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			int fa = FindFather(m_vertexBorder[0], resultA.ProcIndexBuffer[j]);
			int fb = FindFather(m_vertexBorder[0], resultA.ProcIndexBuffer[j + 1]);
			int fc = FindFather(m_vertexBorder[0], resultA.ProcIndexBuffer[j + 2]);
			if ((planeAStatus[resultA.ProcIndexBuffer[j]] == 3 ||
				planeAStatus[resultA.ProcIndexBuffer[j + 1]] == 3 ||
				planeAStatus[resultA.ProcIndexBuffer[j + 2]] == 3) ||
				(vertexPlaneStatus[rIndex[fa]] == 3 &&
					vertexPlaneStatus[rIndex[fb]] == 3 &&
					vertexPlaneStatus[rIndex[fc]] == 3))
			{
				

				if ((fa != fb || fb != fc))
				{
					int i0 = resultA.ProcIndexBuffer[j];
					int i1 = resultA.ProcIndexBuffer[j + 1];
					int i2 = resultA.ProcIndexBuffer[j + 2];
					auto v0 = resultA.ProcVertexBuffer[i0];
					auto v1 = resultA.ProcVertexBuffer[i1];
					auto v2 = resultA.ProcVertexBuffer[i2];
					i_finalMesh.ProcIndexBuffer.Add(rIndex[resultA.ProcIndexBuffer[j]]);
					i_finalMesh.ProcIndexBuffer.Add(rIndex[resultA.ProcIndexBuffer[j + 1]]);
					i_finalMesh.ProcIndexBuffer.Add(rIndex[resultA.ProcIndexBuffer[j + 2]]);
				}
			}
		}
	}
	if (i_insertMode != 2)
	{
		for (int j = 0; j < resultB.ProcIndexBuffer.Num(); j += 3)
		{
			int fa = FindFather(m_vertexBorder[1], resultB.ProcIndexBuffer[j]);
			int fb = FindFather(m_vertexBorder[1], resultB.ProcIndexBuffer[j + 1]);
			int fc = FindFather(m_vertexBorder[1], resultB.ProcIndexBuffer[j + 2]);
			auto va = resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[j]];
			auto vb = resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[j + 1]];
			auto vc = resultB.ProcVertexBuffer[resultB.ProcIndexBuffer[j + 2]];

			DrawDebugLine(m_world, va.Position + offset, vb.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			DrawDebugLine(m_world, vb.Position + offset, vc.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			DrawDebugLine(m_world, vc.Position + offset, va.Position + offset, FColor(0, 0, 0, 1), true, -1, 0, 10);
			if ((planeBStatus[resultB.ProcIndexBuffer[j]] == 3 ||
				planeBStatus[resultB.ProcIndexBuffer[j + 1]] == 3 ||
				planeBStatus[resultB.ProcIndexBuffer[j + 2]] == 3) ||
				(vertexPlaneStatus[rIndex[fa + resultA.ProcVertexBuffer.Num()]] == 3 &&
					vertexPlaneStatus[rIndex[fb + resultA.ProcVertexBuffer.Num()]] == 3 &&
					vertexPlaneStatus[rIndex[fc + resultA.ProcVertexBuffer.Num()]] == 3 ))
			{
				if ((fa != fb || fb != fc))
				{
					i_finalMesh.ProcIndexBuffer.Add(rIndex[resultB.ProcIndexBuffer[j] + resultA.ProcVertexBuffer.Num()]);
					i_finalMesh.ProcIndexBuffer.Add(rIndex[resultB.ProcIndexBuffer[j + 1] + resultA.ProcVertexBuffer.Num()]);
					i_finalMesh.ProcIndexBuffer.Add(rIndex[resultB.ProcIndexBuffer[j + 2] + resultA.ProcVertexBuffer.Num()]);
				}
			}
		}
	}
	return i_finalMesh;
}
