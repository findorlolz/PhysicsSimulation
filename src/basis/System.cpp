#include "System.h"
#include "Integrator.h"
#include "Actor.h"
#include "Renderer.h"

System::~System() 
{
	for(auto i : m_actors)
		delete i;
}

ParticleSystem::ParticleSystem() :
	System()
{
	m_actors.push_back(new TraingleEmitter<ParticleSystem>(FW::Vec3f(1,0,0),FW::Vec3f(0,0,0), FW::Vec3f(0,0,1), 4.0f, 6.0f, 0.2f, 1.0f, 4.0f));
}

BoidSystem::BoidSystem(const float closeDistance, const size_t numOfParticles) :
	System(),
	m_closeDistance(closeDistance),
	m_numberOfParticles(numOfParticles)
{
	FW::Random rnd = FW::Random();
	for(auto i = 0u; i < m_numberOfParticles; ++i)
	{
		const FW::Vec3f pos = rnd.getVec3f(-1.0f,1.0f);
		const FW::Vec3f vel = rnd.getVec3f(-0.5f, 0.5f);
		m_actors.push_back(new Particle<BoidSystem>(1.0f, pos, vel, 1.0f));
	}
	//m_actors.push_back(new Deflecter<BoidSystem>(FW::Vec3f(0.7f,0.0f,0.0f), .5, 10.0f)); 
	m_closeBuffer.reserve(m_numberOfParticles);
}

void System::evalSystem(const float dt)
{
	EulerIntegrator eulerIntegrator = EulerIntegrator::get();
	Runge_KuttaIntegrator rkIntegrator = Runge_KuttaIntegrator::get();
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
			if(actor->getActorType() == ActorType_Particle)
				rkIntegrator.evalIntegrator(dt, actor, m_actors);
			else
				eulerIntegrator.evalIntegrator(dt, actor, m_actors);
		}
	}
}

void System::draw(const float scale)
{
	Renderer renderer = Renderer::get();
	renderer.startFrame(scale);
	for(auto i : m_actors)
	{
		i->draw(renderer);
	}
	renderer.endFrame();
}

void BoidSystem::evalSystem(const float dt)
{
	EulerIntegrator integrator = EulerIntegrator::get();
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
	}
}