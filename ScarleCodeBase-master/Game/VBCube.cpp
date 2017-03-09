#include "VBCube.h"

void VBCube::init(int _size, ID3D11Device* GD)
{
	// this is most certinaly not the most efficent way of doing most of this
	//but it does give you a very clear idea of what is actually going on

	m_size = _size;

	//calculate number of vertices and primatives
	int numVerts = 6 * 6 * (m_size - 1) * (m_size - 1);
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One; 
	}

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
			
			//back			
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 0.0f);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);

			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(1.0f, 4.0f, 1.0f);			

			//front
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 1.0f);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 1.0f);

			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 1.0f);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(1.0f, 4.0f, 1.0f);					

			//side3
			m_vertices[vert].Color = Color(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 0.0f);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 1.0f);			

			m_vertices[vert].Color = Color(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 1.0f);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 1.0f);			

			//top
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 1.0f);
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(1.0f, 4.0f, 1.0f);

			//bottom
			m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 1.0f);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 0.0f);			

	//carry out some kind of transform on these vertices to make this object more interesting
	Transform();

	//calculate the normals for the basic lighting in the base shader
	for (int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}


	BuildIB(GD, indices);
	BuildVB(GD, numVerts, m_vertices);

	delete[] indices;    //this is no longer needed as this is now in the index Buffer
	delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
	m_vertices = nullptr;
}