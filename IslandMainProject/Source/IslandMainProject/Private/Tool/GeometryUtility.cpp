// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryUtility.h"
const double GeometryUtility::eps_const = 1e-4;
GeometryUtility::GeometryUtility()
{
}

GeometryUtility::~GeometryUtility()
{
}

// 1 means in
// 2 means on the line
// view point situation as line situation
int GeometryUtility::IsPointInTriangle(DVector i_point, DVector i_v0, DVector i_v1, DVector i_v2)
{
	DVector lineA = i_v1 - i_v0;
	DVector lineB = i_v2 - i_v0;
	i_point = i_point - i_v0;
	double paramA;
	double paramB;
	double resultC;
	if (lineA.X * lineB.Y - lineA.Y * lineB.X != 0)
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
	else if ((lineA.X * lineB.Z - lineA.Z * lineB.X) != 0)
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
	// include situations on lines
	if (eps(paramA + paramB) > 0 && eps(paramA) >= 0 && eps(paramB) >= 0 && eps(paramA - 1) < 0 && eps(paramB - 1) < 0 && eps(paramB + paramA - 1) <= 0 && eps(resultC) == 0)
	{
		return 2;
	}
	// include situations on points
	if (eps(paramA) >= 0 && eps(paramB) >= 0 && eps(paramB + paramA - 1) <= 0 && eps(resultC) == 0)
	{
		return 3;
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

bool GeometryUtility::GetLineAndPlaneIntersectionPoint(const DVector& i_va, const DVector& i_vb, const DVector& i_normal, DVector &o_intersection)
{
	double distA = DVector::DotProduct(i_va, i_normal) / i_normal.Size();
	double distB = DVector::DotProduct(i_vb, i_normal) / i_normal.Size();
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
	DVector line = i_lineb - i_linea;
	DVector sa = i_va - i_linea;
	DVector sb = i_vb - i_linea;
	DVector perp = sa - line * (DVector::DotProduct(sa, line) / line.Size() / line.Size());
	perp.Normalize();
	double da = DVector::DotProduct(sa, perp);
	double db = DVector::DotProduct(sb, perp);
	DVector intersection = (sa + (-da) / (db - da) * (sb - sa));
	if ((eps(da) >= 0 && eps(db) <= 0 || eps(da) <= 0 && eps(db) >= 0) && (eps(da + db) != 0) && intersection.Size() <= line.Size())
	{
		o_intersection = i_linea + intersection;

		return true;
	}
	return false;
}

void GeometryUtility::TraingleIntersectPolyhedron(
	TArray<DVector> i_vertices, 
	TArray<uint32> i_indices, 
	const FProcMeshSection& i_b, 
	TArray<DVector>& o_generateVertices,
	TArray<uint32>& o_generateIndices, 
	TArray<int>& t_planeBStatus,
	int phase)
{
	if (i_vertices.Num() < 3)
	{
		return;
	}
	o_generateVertices = i_vertices;
	o_generateIndices = i_indices;
	DVector ova = i_vertices[0];
	DVector ovb = i_vertices[1];
	DVector ovc = i_vertices[2];
	DVector currentNormal = DVector::CrossProduct(ovc - ova, ovb - ova);
	DVector intersection;
	TArray<DVector> planeIntersections;
	TArray<DVector> lineIntersections;
	TArray<DVector> partitionPoints;
	for (int i = 0; i + 2 < i_b.ProcIndexBuffer.Num(); i+=3)
	{
		FProcMeshVertex va = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i]];
		FProcMeshVertex vb = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i + 1]];
		FProcMeshVertex vc = i_b.ProcVertexBuffer[i_b.ProcIndexBuffer[i + 2]];
		planeIntersections.Empty();
		lineIntersections.Empty();
		partitionPoints.Empty();
		if (GetLineAndPlaneIntersectionPoint(va.Position - ova, vb.Position - ova, currentNormal, intersection))
		{
			planeIntersections.Add(intersection + ova);
		}
		if (GetLineAndPlaneIntersectionPoint(vb.Position - ova, vc.Position - ova, currentNormal, intersection))
		{
			planeIntersections.Add(intersection + ova);
		}
		if (GetLineAndPlaneIntersectionPoint(va.Position - ova, vc.Position - ova, currentNormal, intersection))
		{
			planeIntersections.Add(intersection + ova);
		}
		if (planeIntersections.Num() > 0)
		{
			if (planeIntersections.Num() == 2) // currently only tackle line situation
			{
				// if two point all in the triangle(even on the edges)
				for (int p_i = 0; p_i < planeIntersections.Num(); p_i++) {
					if (IsPointInTriangle(planeIntersections[p_i], ova, ovb, ovc) > 0)
					{
						partitionPoints.Add(planeIntersections[p_i]);
					}
				}
				if (partitionPoints.Num() < planeIntersections.Num())
				{
					if (GetLineAndLineIntersectionPoint(planeIntersections[0], planeIntersections[1], ovb, ova, intersection))
					{
						partitionPoints.Add(intersection);
					}
					if (GetLineAndLineIntersectionPoint(planeIntersections[0], planeIntersections[1], ovc, ova, intersection))
					{
						partitionPoints.Add(intersection);
					}
					if (GetLineAndLineIntersectionPoint(planeIntersections[0], planeIntersections[1], ovc, ovb, intersection))
					{
						partitionPoints.Add(intersection);
					}
				}
				// 
				if (phase == 0 && partitionPoints.Num() > 0 && t_planeBStatus[i / 3] == 0)
				{
					t_planeBStatus[i / 3] = 2;
				}
				if (i == 1605)
				{
					int k = 0;
				}
				for (int partitionID = 0; partitionID < partitionPoints.Num(); partitionID++)
				{
					bool exist = false;
					uint32 currentVertexID = o_generateVertices.Num();
					for (int j = 0; j < o_generateIndices.Num() - 2; j += 3)
					{
						auto result = IsPointInTriangle(partitionPoints[partitionID],
							o_generateVertices[o_generateIndices[j]],
							o_generateVertices[o_generateIndices[j + 1]],
							o_generateVertices[o_generateIndices[j + 2]]);
						if (result > 0 && result < 3)
						{
							exist = true;
							uint32 ia = o_generateIndices[j];
							uint32 ib = o_generateIndices[j + 1];
							uint32 ic = o_generateIndices[j + 2];
							if (!IsPointOnLineSegment(partitionPoints[partitionID], o_generateVertices[ia], o_generateVertices[ib]))
							{
								o_generateIndices.Add(ia);
								o_generateIndices.Add(ib);
								o_generateIndices.Add(currentVertexID);
							}
							if (!IsPointOnLineSegment(partitionPoints[partitionID], o_generateVertices[ic], o_generateVertices[ia]))
							{
								o_generateIndices.Add(ic);
								o_generateIndices.Add(ia);
								o_generateIndices.Add(currentVertexID);
							}
							if (!IsPointOnLineSegment(partitionPoints[partitionID], o_generateVertices[ib], o_generateVertices[ic]))
							{
								o_generateIndices.Add(ib);
								o_generateIndices.Add(ic);
								o_generateIndices.Add(currentVertexID);
							}
							o_generateIndices.RemoveAt(j, 3);
							break;
						}
					}
					if (exist)
					{
						o_generateVertices.Add(partitionPoints[partitionID]);
					}
				}
			}
		}
	}
}

bool GeometryUtility::IsPointOnLineSegment(const DVector &i_point, const DVector &i_v0, const DVector &i_v1)
{
	DVector lineA = i_point - i_v0;
	DVector standard = i_v1 - i_v0;
	if (lineA.Size() <= standard.Size())
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

void GeometryUtility::MeshSectionIntersection(const FProcMeshSection& i_a, const FProcMeshSection& i_b, FProcMeshSection& o_result, TArray<int> &t_planeAStatus, TArray<int> &t_planeBStatus, int phase)
{
	// Decide all vertices status for pointed mesh section
	TArray<bool> verticesStatus;
	TArray<uint32> indexConvdersion;
	TArray<FProcMeshVertex> addedVertices;
	TArray<uint32> addedIndices;
	verticesStatus.Empty();
	addedVertices.Empty();
	addedIndices.Empty();
	// initialization
	for (int i = 0; i < i_a.ProcVertexBuffer.Num(); i++)
	{
		verticesStatus.Add(IsPointInPolyhedron(i_a.ProcVertexBuffer[i].Position, i_b));
	}
	if (phase == 0)
	{
		for (int i = 0; i < i_a.ProcVertexBuffer.Num(); i++)
		{
			addedVertices.Add(i_a.ProcVertexBuffer[i]);
		}
	}
	else if (phase == 1)
	{
		for (int i = 0; i < o_result.ProcVertexBuffer.Num(); i++)
		{
			addedVertices.Add(o_result.ProcVertexBuffer[i]);
		}
		for (int i = 0; i < o_result.ProcIndexBuffer.Num(); i++)
		{
			addedIndices.Add(o_result.ProcIndexBuffer[i]);
		}
	}
	o_result.ProcVertexBuffer.Empty();
	o_result.ProcIndexBuffer.Empty();

	// generate intersection faces
	TArray<DVector> triangleVerticesArray;
	TArray<uint32> triangleIndicesArray, triangleNewIndicesArray;
	triangleVerticesArray.Init(DVector(0, 0, 0), 3);
	triangleIndicesArray.Init(0, 3);
	triangleNewIndicesArray.Init(0, 3);
	for (int i = 0; i < i_a.ProcIndexBuffer.Num() - 2; i += 3)
	{
		for (int j = 0; j < 3; j++)
		{
			triangleIndicesArray[j] = i_a.ProcIndexBuffer[i + j];
			triangleVerticesArray[j] = i_a.ProcVertexBuffer[i_a.ProcIndexBuffer[i + j]].Position;
			triangleNewIndicesArray[j] = j;
		}
		TArray<uint32> additionIndices = triangleNewIndicesArray;
		TArray<DVector> additionVertices = triangleVerticesArray;
		if (phase == 0) // if the triangle intersect the polythedreon
		{
			if (verticesStatus[i_a.ProcIndexBuffer[i]] || verticesStatus[i_a.ProcIndexBuffer[i + 1]] || verticesStatus[i_a.ProcIndexBuffer[i + 2]] &&
				!(verticesStatus[i_a.ProcIndexBuffer[i]] && verticesStatus[i_a.ProcIndexBuffer[i + 1]] && verticesStatus[i_a.ProcIndexBuffer[i + 2]]))
			{
				TraingleIntersectPolyhedron(triangleVerticesArray, triangleNewIndicesArray, i_b, additionVertices, additionIndices, t_planeBStatus, phase);
				t_planeAStatus[i / 3] = 1;
			}
			else
			{
				if (t_planeAStatus[i / 3] != 2)
				{
					t_planeAStatus[i / 3] = 0;
				}
			}
		}
		if (phase == 1)
		{
			if (t_planeAStatus[i / 3] == 2)
			{
				TraingleIntersectPolyhedron(triangleVerticesArray, triangleNewIndicesArray, i_b, additionVertices, additionIndices, t_planeBStatus, phase);
			}
		}
		if (phase == 0 && t_planeAStatus[i / 3] == 1 || phase == 1 && t_planeAStatus[i / 3] != 1)
		{
			int offset = addedVertices.Num();
			for (int a_i = 0; a_i < additionIndices.Num(); a_i++)
			{
				if (additionIndices[a_i] > 2)
				{
					// ignore first 3 point of initial triangle, becuase they are already be included.
					addedIndices.Add(additionIndices[a_i] + offset - 3);
				}
				else
				{
					addedIndices.Add(triangleIndicesArray[additionIndices[a_i]]);
				}
			}
			for (int a_i = 3; a_i < additionVertices.Num(); a_i++)
			{
				auto newProcMeshVertex = i_a.ProcVertexBuffer[i_a.ProcIndexBuffer[i]];
				newProcMeshVertex.Position = additionVertices[a_i].FVectorConversion();
				// TODO: set new vertex UV
				//newProcMeshVertex.UV0 = ;
				addedVertices.Add(newProcMeshVertex);

			}
		}
	}
	int NeedNum = 0;
	if (phase == 0)
	{
		for (int i = 0; i < addedVertices.Num(); i++) 
		{
			o_result.ProcVertexBuffer.Add(addedVertices[i]);
		}
		for (int i = 0; i < addedIndices.Num(); i++)
		{
			o_result.ProcIndexBuffer.Add(addedIndices[i]);
		}
	}
	else if (phase == 1)
	{
		indexConvdersion.Init(0, addedVertices.Num());
		t_planeAStatus.Empty();
		// fliter vertex that in the polyhedron
		int filteringVerticesNum = 0;
		for (int i = 0; i < addedVertices.Num(); i++) {
			indexConvdersion[i] = filteringVerticesNum;
			if (i >= i_a.ProcVertexBuffer.Num() || !verticesStatus[i])
			{
				o_result.ProcVertexBuffer.Add(addedVertices[i]);
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
		for (int i = 0; i < addedIndices.Num() - 2; i += 3)
		{
			UE_LOG(LogTemp, Log, TEXT("%s %s %s"),
				*addedVertices[addedIndices[i]].Position.ToString(),
				*addedVertices[addedIndices[i + 1]].Position.ToString(),
				*addedVertices[addedIndices[i + 2]].Position.ToString());
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
					FProcMeshVertex v1 = addedVertices[i1];
					FProcMeshVertex v2 = addedVertices[i2];
					FProcMeshVertex v3 = addedVertices[i3];
					bool s1 = verticesStatus[addedIndices[i]];
					bool s2 = verticesStatus[addedIndices[i + 1]];
					bool s3 = verticesStatus[addedIndices[i + 2]];
					int noChangeToNeedIntersection = 1;
					NeedNum++;
				}
			}
			
		}
		NeedNum += 0;
	}
	return;
}