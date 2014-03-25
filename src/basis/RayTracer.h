#pragma once

#include "base/String.hpp"
#include <vector>

#include "HelpFunctions.h"
#include "base/Random.hpp"

//
// The ray tracer uses its own, extremely simple interface for vertices and triangles.
// This will make it extremely simple for you to rip it out and reuse in subsequent projects!
// Here's what you do:
//  1. put all vertices in a big vector,
//  2. put all triangles in a big vector,
//  3. make the triangle structs vertex pointers point to the correct places in the big vertex chunk.
//

// The userpointer member can be used for identifying the triangle in the "parent" mesh representation.

// Main class for tracing rays using BVHs
class RayTracer
{
public:
						RayTracer				(void);
						~RayTracer				(void);

	void				constructHierarchy		(std::vector<Triangle>& triangles);

	// not required, but HIGHLY recommended!
	/*void				RayTracer::saveHierarchy			(const char* filename, const std::vector<Triangle>& triangles);
	void				RayTracer::loadHierarchy			(const char* filename, std::vector<Triangle>& triangles);
	*/
	//bool				rayCast					(Vec3f& orig, Vec3f& dir, Hit& closestHit);
	bool			    rayCastAny				(FW::Vec3f& orig, FW::Vec3f& dir);

	// This function computes an MD5 checksum of the input scene data,
	// WITH the assumption all vertices are allocated in one big chunk.
	static FW::String	computeMD5				(const std::vector<FW::Vec3f>& vertices);


	std::vector<Triangle>*		m_triangles;

private:
	void							constructTree(int startPrim, int endPrim, Node* node);
	void							quickSort(int left, int right, Axis axis);
	void							demolishTree(Node* node);
	bool							isLeaf(Node* node);
	void							createIndexList();
	void							createBBForTriangles();
	FW::Vec3f						getTriangleBBMaxPoint(int index);
	FW::Vec3f						getTriangleBBMinPoint(int index);
	FW::Vec3f						getTriangleBBCenPoint(int index);
	void							RayTracer::demolishBBForTriangles();
	/*void							RayTracer::saveRec(FILE* file, Node* node);
	void							RayTracer::loadRec(FILE* file, Node* node);*/

	int								m_numPrism;
	int								m_maxPrismBB;
	Node*							m_root;
	FW::Random						m_random;
	FW::Vec3f*						m_triangleBB;
	int*							m_indexList;
};