#define _CRT_SECURE_NO_WARNINGS

#include "base/Defs.hpp"
#include "base/Math.hpp"
#include "RayTracer.h"
#include <stdio.h>
#include "rtIntersect.inl"
#include <iostream>

RayTracer::RayTracer()
{
	m_numPrism = 0;
	m_maxPrismBB = 8;
	m_root = NULL;
	m_random = FW::Random();
}

RayTracer::~RayTracer()
{
	RayTracer::demolishTree(m_root);
	delete[] m_indexList;
	m_indexList = 0;
}
/*
void RayTracer::loadHierarchy( const char* filename, std::vector<RTTriangle>& triangles )
{
	m_triangles = &triangles;
	FILE* file = NULL;
	file = fopen(filename, "rb");
	if (!file) return; 
	m_indexList = new int[triangles.size()];
	fread(m_indexList, sizeof(int), triangles.size(), file);
	m_root = new Node();
	loadRec(file, m_root);
	fclose(file);
}

void RayTracer::loadRec(FILE* file, Node* node)
{
	bool isLeaf;
	fread(&(node->BBMin.x), sizeof(float), 3, file);
	fread(&(node->BBMax.x), sizeof(float), 3, file);
	fread(&(node->startPrim), sizeof(int), 1, file);
	fread(&(node->endPrim), sizeof(int), 1, file);
	fread(&(node->axis), sizeof(int), 1, file);
	fread(&isLeaf, sizeof(bool), 1, file);
	node->leftChild = NULL;
	node->rightChild = NULL;
	if (!isLeaf)
	{
		node->leftChild = new Node();
		node->rightChild = new Node();
		loadRec(file, node->leftChild);
		loadRec(file, node->rightChild);
	}
}

void RayTracer::saveHierarchy(const char* filename, const std::vector<RTTriangle>& triangles)
{
	//Testing
	//std::cout << "Saving Hierarchy" << std::endl;

	FILE* file = NULL;
	file = fopen(filename, "wb");
	if (!file) return; 
	fwrite(m_indexList, sizeof(int), triangles.size(), file);
	saveRec(file, m_root);
	fclose(file);
}

void RayTracer::saveRec(FILE* file, Node*node)
{
	bool isLeaf = (node->leftChild == NULL) ? true : false;
	fwrite(&(node->BBMin.x), sizeof(float), 3, file);
	fwrite(&(node->BBMax.x), sizeof(float), 3, file);
	fwrite(&(node->startPrim), sizeof(int), 1, file);
	fwrite(&(node->endPrim), sizeof(int), 1, file);
	fwrite(&(node->axis), sizeof(int), 1, file);
	fwrite(&isLeaf, sizeof(bool), 1, file);
	if (isLeaf) return;
	saveRec(file, node->leftChild);
	saveRec(file, node->rightChild);
}
*/
void RayTracer::constructHierarchy( std::vector<Triangle>& triangles)
{
	std::cout << "Creating RayTracer Hieracrhy... ";
	m_triangles = &triangles;
	m_numPrism = triangles.size() - 1;
	RayTracer::createIndexList();
	RayTracer::createBBForTriangles();
	m_root = new Node();
	constructTree(0, m_numPrism, m_root);
	RayTracer::demolishBBForTriangles();
	std::cout << "done!" << std::endl;
}

bool RayTracer::rayCastAny(FW::Vec3f& orig, FW::Vec3f& dir )
{
	float tHit = 1.0f;
	Node* current = m_root;
	
	//Basic recursion traverse test
	//RayTracer::basicTraverseTest(orig, dir, closestHit, current);

	FW::Vec3f dirInv = 1.0f/(dir);
	Node* stack[128];
	int stackPointer = 0;

	while (true)
	{
		bool intersection = intersect_bb(&orig.x, &dirInv.x, &(current->BBMin.x), &(current->BBMax.x), tHit);	
		if (intersection) // check if intersect with current
		{
			if(isLeaf(current)) // current is leaf node => process primitives
			{
				for ( size_t i = current->startPrim; i <= current->endPrim; ++i)
				{
					float t, u, v;
					if ( intersect_triangle1( &orig.x, &dir.x,
											  &(*m_triangles)[m_indexList[i]].m_vertices[0]->x,
											  &(*m_triangles)[m_indexList[i]].m_vertices[1]->x,
											  &(*m_triangles)[m_indexList[i]].m_vertices[2]->x,
											  t, u, v ) )
					{
						if ( t > 0.0f && t < tHit)
						{
								return true;
						}
					}
				}
				if (stackPointer == 0) // stack empty, break
				{
					break;
				}
				else // stack not empty, pop new current
				{
					stackPointer -=1;
					current = stack[stackPointer];
				}
			}
			else // isn't leaf
			{
				//std::cout << "Ray direction: " << dir[current->axis] << std::endl;
				if (dir[current->axis] < 0.0f)
				{
					stack[stackPointer] = current->leftChild;
					stackPointer = stackPointer + 1;
					current = current->rightChild;
					//std::cout << "Go left" << std::endl;
				}
				else
				{
					stack[stackPointer] = current->rightChild;
					stackPointer = stackPointer + 1;
					current = current->leftChild;
					//std::cout<< "Go right" << std::endl;
				}
			}		//isLeaf-if
		}
		else // doesnt intersect with current node's bb
		{
			if (stackPointer == 0)
			{
				break;
			}
			else
			{
				stackPointer -= 1;
				current = stack[stackPointer];
				//std::cout << "stack pop 1 " << std::endl;
			}
		}			//intesection if
	}				//while-loop
	
	//std::cout << "end" <<std::endl;
	return false;
}


void RayTracer::constructTree(int startPrim, int endPrim, Node* node)
{
	node->startPrim = startPrim;
	node->endPrim = endPrim;
	
	node->leftChild = NULL;
	node->rightChild = NULL;
	Axis axis;

	if (node == m_root)
	{
		node->BBMin = FW::Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
		node->BBMax = FW::Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		
		for (int prim = startPrim; prim <= endPrim; prim++)
		{
			node->BBMin = min(node->BBMin, RayTracer::getTriangleBBMinPoint(m_indexList[prim]));
			node->BBMax = max(node->BBMax, RayTracer::getTriangleBBMaxPoint(m_indexList[prim]));
		}
	}

	if ((endPrim - startPrim) + 1 > m_maxPrismBB)
	{
		const size_t bbSize = (endPrim - startPrim + 1);
		const size_t index = bbSize - 1;
		float tmpSAH = FLT_MAX;
		size_t tmpSAHAxis = 0;
		size_t tmpSAHindex = 0;

		Node* leftNode = new Node();
		Node* rightNode = new Node();

		std::vector<FW::Vec3i> sortedIndices = std::vector<FW::Vec3i>(bbSize);
		std::vector<BB> boundingBoxesR = std::vector<BB>(index);
		std::vector<BB> boundingBoxesL =  std::vector<BB>(index);

		for(auto i = 0u; i < 3; ++i)
		{
			quickSort(startPrim, endPrim, (Axis) i);
			FW::Vec3f maxPointRight = FW::Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			FW::Vec3f maxPointLeft = FW::Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			FW::Vec3f minPointRight = FW::Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
			FW::Vec3f minPointLeft = FW::Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
			for(auto j = 0u; j < index; ++j)
			{
				sortedIndices[j][i] = m_indexList[startPrim+j];
				minPointLeft = min(minPointLeft, getTriangleBBMinPoint(m_indexList[startPrim+j]));
				maxPointLeft = max(maxPointLeft, getTriangleBBMaxPoint(m_indexList[startPrim+j]));
				boundingBoxesL[j].set(minPointLeft, maxPointLeft);
				minPointRight = min(minPointRight, RayTracer::getTriangleBBMinPoint(m_indexList[endPrim-j]));
				maxPointRight = max(maxPointRight, RayTracer::getTriangleBBMaxPoint(m_indexList[endPrim-j]));
				boundingBoxesR[index-(j+1)].set(minPointRight, maxPointRight);
			}
			sortedIndices[index][i] = m_indexList[startPrim+index];
			for(auto j = 0u; j < index; ++j)
			{
				float SAH = boundingBoxesL[j].area * (float)(j+1)  + boundingBoxesR[j].area * (float)(index - j);
				if(SAH < tmpSAH)
				{
					leftNode->BBMin = boundingBoxesL[j].min;
					leftNode->BBMax = boundingBoxesL[j].max;
					rightNode->BBMin = boundingBoxesR[j].min;
					rightNode->BBMax = boundingBoxesR[j].max;
					tmpSAH = SAH;
					tmpSAHAxis = i;
					tmpSAHindex = j;
				}
			}
		}

		node->axis = (Axis)tmpSAHAxis;

		size_t t = 0;
		for (auto i = startPrim; i <= endPrim; i++)
		{
			m_indexList[i] = sortedIndices[t][tmpSAHAxis]; 
			t++;
		}

		size_t split = startPrim + tmpSAHindex;
			
		constructTree(startPrim, split, leftNode);
		constructTree(split + 1, endPrim, rightNode);
	}
}

void RayTracer::quickSort(int start, int end, Axis axis)
{
    if (start > end)
            return;

    // Randomize partition
    int rndm = m_random.getU32(start, end);
	std::swap(m_indexList[rndm], m_indexList[end]);

    // Partition
	float pivot = RayTracer::getTriangleBBCenPoint(m_indexList[end])[axis];
    int i = start - 1;
    for (int j = start; j < end; j++)
    {
            if (getTriangleBBCenPoint(m_indexList[j])[axis] < pivot)
            {
                    i++;
                    std::swap(m_indexList[i], m_indexList[j]);
            }
    }
    std::swap(m_indexList[i+1], m_indexList[end]);
    int middle = i+1;

    // Recursively sort partitions
    quickSort(start, middle-1, axis);
    quickSort(middle+1, end, axis);
}

void RayTracer::demolishTree(Node* node)
{
	
	if(isLeaf(node) == false)
	{
		RayTracer::demolishTree(node->leftChild);
		RayTracer::demolishTree(node->rightChild);
	}
	delete node;

}


bool RayTracer::rayCast(FW::Vec3f& orig, FW::Vec3f& dir, Hit& closestHit, const float d)
{
	Node* current = m_root;
	
	//Basic recursion traverse test
	//RayTracer::basicTraverseTest(orig, dir, closestHit, current);

	FW::Vec3f dirInv = 1.0f/(dir);
	Node* stack[128];
	size_t stackPointer = 0;

	while (true)
	{
		bool intersection = intersect_bb(&orig.x, &dirInv.x, &(current->BBMin.x), &(current->BBMax.x), closestHit.t);	
		if (intersection)
		{
			if(isLeaf(current))
			{
				for (size_t i = current->startPrim; i <= current->endPrim; ++i)
				{
					float t, u, v;
					if ( intersect_triangle1( &orig.x, &dir.x,
											  &(*m_triangles)[m_indexList[i]].m_vertices[0]->x,
											  &(*m_triangles)[m_indexList[i]].m_vertices[1]->x,
											  &(*m_triangles)[m_indexList[i]].m_vertices[2]->x,
											  t, u, v ) )
					{
						if ( t > 0.0f && t < closestHit.t )
						{
							closestHit.i = m_indexList[i];
							closestHit.t = t;
							closestHit.u = u;
							closestHit.v = v;
							closestHit.b = true;
						}
					}
				}
				if (stackPointer == 0) // stack empty, break
				{
					break;
				}
				else // stack not empty, pop new current
				{
					stackPointer -=1;
					current = stack[stackPointer];
				}
			}
			else // isn't leaf
			{
				//std::cout << "Ray direction: " << dir[current->axis] << std::endl;
				if (dir[current->axis] < 0.0f)
				{
					stack[stackPointer] = current->leftChild;
					stackPointer = stackPointer + 1;
					current = current->rightChild;
					//std::cout << "Go left" << std::endl;
				}
				else
				{
					stack[stackPointer] = current->rightChild;
					stackPointer = stackPointer + 1;
					current = current->leftChild;
					//std::cout<< "Go right" << std::endl;
				}
			}		//isLeaf-if
		}
		else // doesnt intersect with current node's bb
		{
			if (stackPointer == 0)
			{
				break;
			}
			else
			{
				stackPointer -= 1;
				current = stack[stackPointer];
				//std::cout << "stack pop 1 " << std::endl;
			}
		}			//intesection if
	}				//while-loop
	
	//std::cout << "end" <<std::endl;
	if(closestHit.b)
	{
		closestHit.intersectionPoint = orig + closestHit.t*dir;
		closestHit.triangle = (*m_triangles)[closestHit.i];
		return true;
	}
	else 
		return false;
}

bool RayTracer::isLeaf(Node* node)
{
	if (node->leftChild == NULL)
		return true;
	else
		return false;
}

//Simply creating indexList that at start is indexList[0] = 0 indeList[1] = 1 etc

void RayTracer::createIndexList()
{
	const int size = m_numPrism + 1; 
	int* list = new int[size];
	for (int i = 0; i < size; i++)
	{
		list[i] = i;
	}
	m_indexList = list;
}

void  RayTracer::createBBForTriangles()
{
	const size_t size = 3 * (m_numPrism + 1);
	FW::Vec3f* list = new FW::Vec3f[size];
	size_t i = 0;
	while (i <= m_numPrism)
	{

		list[3*i] = FW::Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
		list[3*i + 1] = FW::Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for(size_t vert = 0u; vert < 3; vert++)
		{
			FW::Vec3f tmp = *((*m_triangles)[m_indexList[i]].m_vertices[vert]);
			list[3*i] = min(tmp, list[3*i]);
			list[3*i + 1] = max(tmp, list[3*i + 1]);
		}
		list[3*i + 2] = (list[3*i] + list[3*i+1])*0.5f;
		i++;
	}
	m_triangleBB = list;
}

void RayTracer::demolishBBForTriangles()
{
	//Testing
	/*for (int i = 0; i <= m_numPrism; i++)
	{
		std::cout << "index: " << i << std::endl;
		m_triangleBB[i].print();
	}*/

	delete[] m_triangleBB;
	m_triangleBB = 0;
}

FW::Vec3f RayTracer::getTriangleBBMinPoint(int index)
{
	return m_triangleBB[3*index];
}

FW::Vec3f RayTracer::getTriangleBBMaxPoint(int index)
{
	return m_triangleBB[3*index + 1];
}

FW::Vec3f RayTracer::getTriangleBBCenPoint(int index)
{
	return m_triangleBB[3*index + 2];
}