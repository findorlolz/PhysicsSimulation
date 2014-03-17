#pragma once

#include "base/Random.hpp"
#include <vector>
#include <omp.h>
#include "Vector.h"

class Actor;
class ActorContainer;
class State;
class MemPool;
class FlowControl;

const size_t threadBufferSize = 10240;

class ActorContainer
{
public:

	enum ActiveStatus
	{
		ActiveStatus_Active,
		ActiveStatus_Unactive,
		ActiveStatus_Disable
	};

	ActorContainer(void*, void*);
	~ActorContainer() {}

	bool isActive() const { return status == ActiveStatus_Active; }
	bool isDisabled() const { return status == ActiveStatus_Disable; }

	std::vector<Actor*> createdActors;	
	Actor* actor;						
	void* data1;						
	void* data2;					
	ActiveStatus status;
	unsigned int* _pad[1];
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

class ParticleSystem : public System
{
public:
	ParticleSystem(const float, const float, const float, const float, const float = 1.0f);
	virtual ~ParticleSystem();

	virtual void evalSystem(const float dt);

private:
	void estimateParticleNumRec(float, const float);

	std::vector<size_t> m_freeContainerIndices;
	Actor*** m_threadCreateBuffers;
	size_t** m_threadRemoveBuffers;
	size_t* m_threadCreateBufferIndex;
	size_t* m_threadRemoveBufferIndex;
	MemPool* m_particlePool;
	MemPool* m_particleEmitterPool;
	size_t m_maxNumThreads;
	size_t m_numParticles;
	size_t m_numParticleEmitters;
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

class FlowSystem : public System
{
public:
	FlowSystem();
	virtual  ~FlowSystem();

	virtual void evalSystem(const float dt);

private:
	std::vector<size_t> m_freeContainerIndices;
	FlowControl* m_flow;
	MemPool* m_particlePool;
	size_t** m_threadRemoveBuffers;
	size_t* m_threadRemoveBufferIndex;
	size_t m_maxNumThreads;
	size_t m_numParticles;
};