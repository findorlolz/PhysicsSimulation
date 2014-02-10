#include "System.h"
#include "Integrator.h"
#include "Renderer.h"
#include "Actor.h"
#include "Memory.h"

System::~System() 
{
}

DynamicSystem::DynamicSystem() :
	System()
{
		m_maxNumThreads = omp_get_max_threads();
		m_threadRemoveBuffers = new size_t*[m_maxNumThreads];
		for( auto i = 0u; i < m_maxNumThreads; i++ ) m_threadRemoveBuffers[i] = new size_t[threadBufferSize];
		m_threadCreateBuffers = new Actor**[m_maxNumThreads];
		for( auto i = 0u; i < m_maxNumThreads; i++ ) m_threadCreateBuffers[i] = new Actor*[threadBufferSize];
		m_threadCreateBufferIndex = new size_t[m_maxNumThreads];
		for( auto i = 0u; i < m_maxNumThreads; i++ ) m_threadCreateBufferIndex[i] = 0;
		m_threadRemoveBufferIndex = new size_t[m_maxNumThreads];
		for( auto i = 0u; i < m_maxNumThreads; i++ ) m_threadRemoveBufferIndex[i] = 0;
}

DynamicSystem::~DynamicSystem()
{
		for( auto i = 0u; i < m_maxNumThreads; i++ ) delete[] m_threadRemoveBuffers[i];
		delete[] m_threadRemoveBuffers;
		for( auto i = 0u; i < m_maxNumThreads; i++ ) delete[] m_threadCreateBuffers[i];
		delete[] m_threadCreateBuffers;
		delete[] m_threadCreateBufferIndex;
		delete[] m_threadRemoveBufferIndex;


		for(auto i : m_actors)
			if(i.activeFlag)
			{
				if(i.actor->getActorType() != ActorType_Particle && i.actor->getActorType() != ActorType_ParticleEmitter)
					delete i.actor;
				else
					i.actor->~Actor();
			}

		m_particleEmitterPool->shutDown();
		delete m_particleEmitterPool;
		m_particlePool->shutDown();
		delete m_particlePool;
}

ParticleSystem::ParticleSystem(const float minSpeed, const float maxSpeed, const float timeBetweenParticles,
							   const float particleLifeTime, const float particleMass) :
	DynamicSystem()
{
	m_numParticleEmitters = 0u;
	m_numParticles = 0u;
	float time = particleLifeTime;
	while(time >= 0.0f)
	{
		m_numParticleEmitters++;
		estimateParticleNumRec(time, timeBetweenParticles);
		time -= timeBetweenParticles;
	}
	time = particleLifeTime;
	size_t tmp = 0;
	while(time >= 0.0f)
	{
		m_numParticles += (m_numParticleEmitters - tmp);
		tmp++;
		time -= timeBetweenParticles;
	}
	m_actors.reserve(m_numParticles+m_numParticleEmitters);
	m_particlePool = new MemPool();
	m_particlePool->startUp(sizeof(Particle<ParticleSystem>), m_numParticles + (m_numParticles/10));
	m_particleEmitterPool = new MemPool();
	m_particleEmitterPool->startUp(sizeof(ParticleEmitter<ParticleSystem>), m_numParticleEmitters+(m_numParticleEmitters/10));
	std::cout << m_numParticles << "/" << m_numParticleEmitters << std::endl;
	std::cout << sizeof(ActorContainer) << std::endl;

	ActorContainer c = ActorContainer(m_particlePool, m_particleEmitterPool);
	c.actor = new TraingleEmitter<ParticleSystem>(FW::Vec3f(1,0,0),FW::Vec3f(0,0,0), FW::Vec3f(0,0,1), minSpeed, maxSpeed, timeBetweenParticles, particleMass, particleLifeTime);
	c.activeFlag = true;
	m_actors.push_back(c);
}

void ParticleSystem::estimateParticleNumRec(float lifetime, const float timeBetween)
{
	while (lifetime >= 0)
	{
		lifetime -= timeBetween;
		m_numParticles++;
	}
}

BoidSystem::BoidSystem(const float closeDistance, const size_t numOfParticles) :
	System(),
	m_closeDistance(closeDistance),
	m_numberOfParticles(numOfParticles)
{
	/*FW::Random rnd = FW::Random();
	for(auto i = 0u; i < m_numberOfParticles; ++i)
	{
		const FW::Vec3f pos = rnd.getVec3f(-1.0f,1.0f);
		const FW::Vec3f vel = rnd.getVec3f(-0.5f, 0.5f);
		m_actors.push_back(new Particle<BoidSystem>(1.0f, pos, vel, 1.0f));
	}
	//m_actors.push_back(new Deflecter<BoidSystem>(FW::Vec3f(0.7f,0.0f,0.0f), .5, 10.0f)); 
	m_closeBuffer.reserve(m_numberOfParticles);*/
}

void DynamicSystem::evalSystem(const float dt)
{
	EulerIntegrator eulerIntegrator  = EulerIntegrator::get();
	Runge_KuttaIntegrator rkIntegrator = Runge_KuttaIntegrator::get();
	#pragma omp parallel
	{
		#pragma omp for nowait
		for(int i = 0u; i < m_actors.size(); ++i)
		{
			ActorContainer& c = m_actors[i];
			if(!c.activeFlag)
				continue;
			size_t threadIndex = omp_get_thread_num();
			Actor* a = c.actor;
			if(a->isDone())
			{
				m_actors[i].activeFlag = false;
				m_threadRemoveBuffers[threadIndex][m_threadRemoveBufferIndex[threadIndex]] = i;
				m_threadRemoveBufferIndex[threadIndex] += 1;
			}
			else
			{
				if(a->getActorType() == ActorType_Particle)
					rkIntegrator.evalIntegrator(dt, c);
				else
				{
					eulerIntegrator.evalIntegrator(dt, c);
					while(!c.createdActors.empty())
					{
						m_threadCreateBuffers[threadIndex][m_threadCreateBufferIndex[threadIndex]] = c.createdActors.back();
						m_threadCreateBufferIndex[threadIndex] += 1;
						c.createdActors.pop_back();
					}
				}
			}
		}
	}

	for(size_t threadIndex = 0u; threadIndex < m_maxNumThreads; ++threadIndex)
	{
		for(size_t i = 0u; i < m_threadRemoveBufferIndex[threadIndex]; ++i)
		{
			size_t index = m_threadRemoveBuffers[threadIndex][i];
			Actor* a = m_actors[index].actor;
			if(a->getActorType() == ActorType_ParticleEmitter)
			{
				a->~Actor();
				m_particleEmitterPool->release((unsigned char*) a);
			}
			else
			{
				a->~Actor();
				m_particlePool->release((unsigned char*) a);
			}
			m_actors[index].actor = nullptr;
			m_freeContainerIndices.push_back(index);
		}
		m_threadRemoveBufferIndex[threadIndex] = 0;
		for(size_t i = 0u; i < m_threadCreateBufferIndex[threadIndex]; ++i)
		{
			Actor* a = m_threadCreateBuffers[threadIndex][i];
			if(!m_freeContainerIndices.empty())
			{
				ActorContainer& c = m_actors[m_freeContainerIndices.back()];
				m_freeContainerIndices.pop_back();
				c.activeFlag = true;
				c.actor = a;
			}
			else
			{
				ActorContainer c = ActorContainer(m_particlePool, m_particleEmitterPool);
				c.activeFlag = true;
				c.actor = a;
				m_actors.push_back(c);
			}
		}
		m_threadCreateBufferIndex[threadIndex] = 0;
	}
	
	#pragma omp parallel for
	for(int i = 0u; i < m_actors.size(); ++i)
	{
		if(m_actors[i].activeFlag)
		{
			m_actors[i].actor->updateStateFromBuffer();
		}
	}
}

void System::draw(const float scale)
{
	Renderer& renderer = Renderer::get();
	renderer.startFrame(scale);
	renderer.endFrame();
}

void System::updateMesh()
{
	Renderer& renderer = Renderer::get();
	renderer.clearDynamicMesh();
	for(auto& i : m_actors)
	{
		if(i.activeFlag)
			i.actor->draw(renderer);
	}
}

void BoidSystem::evalSystem(const float dt)
{
	/*EulerIntegrator integrator = EulerIntegrator::get();
	for(auto i = 0u; i < m_actors.size(); ++i)
	{
		auto actor = m_actors[i];
		if(actor->isDone())
		{
			Actor* a = actor;
			auto iter = std::next(m_actors.begin(), i);
			m_actors.erase(iter);
			delete a;
		}
		else
		{
			m_closeBuffer.clear();
			for(auto j = 0u; j < m_actors.size(); ++j)
			{
				auto close = m_actors[j];
				float d = (actor->getState().pos - close->getState().pos).length();
				if(close != actor && d <= m_closeDistance)
					m_closeBuffer.push_back(close);
			}
			integrator.evalIntegrator(dt, actor, m_closeBuffer);
		}
	}*/
}