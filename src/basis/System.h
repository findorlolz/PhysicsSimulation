#pragma once

#include "base/Random.hpp"
#include <vector>
#include <omp.h>

class Actor;
class ActorContainer;
class State;
class MemPool;

const size_t threadBufferSize = 256;

class ActorContainer
{
public:
	ActorContainer(MemPool*, MemPool*);
	~ActorContainer() {}

	std::vector<Actor*> createdActors;	//16
	Actor* actor;						//4
	MemPool* particleEmitterPool;		//4
	MemPool* particlePool;				//4
	bool activeFlag;					//4
};

class System
{
public:
	System() 
	{}
	virtual ~System();

	virtual void evalSystem(const float) {}
	virtual void draw(const float);
	virtual void updateMesh();

protected:
	std::vector<ActorContainer> m_actors;
};

class DynamicSystem : public System
{
public:
	DynamicSystem();
	~DynamicSystem();

	virtual void evalSystem(const float);

protected:
	std::vector<size_t> m_freeContainerIndices;
	Actor*** m_threadCreateBuffers;
	size_t** m_threadRemoveBuffers;
	size_t* m_threadCreateBufferIndex;
	size_t* m_threadRemoveBufferIndex;
	size_t m_maxNumThreads;

	size_t m_numParticles;
	size_t m_numParticleEmitters;
	MemPool* m_particlePool;
	MemPool* m_particleEmitterPool;
};

class ParticleSystem : public DynamicSystem
{
public:
	ParticleSystem(const float, const float, const float, const float, const float = 1.0f);
	virtual ~ParticleSystem() {}

private:
	void estimateParticleNumRec(float, const float);
};

class BoidSystem : public System
{
public:
	BoidSystem(const float, const size_t);
	virtual ~BoidSystem() {}
	virtual void evalSystem(const float);

private:
	std::vector<Actor*> m_closeBuffer;
	float m_closeDistance;
	size_t m_numberOfParticles;
};