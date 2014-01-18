#include "System.h"

void ParticleSystem::evalSystem(const float dt)
{
	EulerIntegrator integrator = EulerIntegrator::get();
	m_emitter->evalF(dt, m_actors);
	auto i = m_actors.begin();
	while(i != m_actors.end())
	{
		if((*i)->isDone())
		{
			Actor* a = *i;
			i = m_actors.erase(i);
			delete a;
		}
		else
		{
			integrator.evalIntegrator(dt, *i, m_actors);
			++i;
		}
	}
}

void ParticleSystem::draw(const float scale)
{
	Renderer renderer = Renderer::get();
	renderer.startFrame(scale);
	for(auto i : m_actors)
		i->draw(renderer);
	renderer.endFrame();
}