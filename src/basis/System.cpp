#include "System.h"

void Emitter::evalF(const float dt)
{
	float tmp = dt + m_previousTick;
	while(tmp >= m_timeBetween)
	{
		float r = m_randomGen.getF32(0,1.0f);
		FW::Vec3f p = (m_posA + (r * m_posB) + ((1-r) * m_posC)) * 0.5f;
		r = m_randomGen.getF32(m_minSpeed, m_maxSpeed);
		FW::Vec3f v = r * m_normal;
		Particle* particle = new Particle(m_particleMass, p, v);
		m_particles.push_back(particle);
		std::cout << "New particle created! " << tmp << " - " << p.x << "/" << p.y << "/" << p.z << std::endl;
		tmp -= m_timeBetween;	
	}
	m_previousTick = tmp;
	EulerIntegrator integrator = EulerIntegrator::get();
	for(auto i : m_particles)
		i->setState(integrator.eval(dt, State(i->getPos(), i->getVel(), i->getAcc()), &ParticleSystem::evalFunc));
}

void Emitter::draw(const float scale)
{
	Renderer renderer = Renderer::get();
	renderer.startFrame(scale);
	for(auto i : m_particles)
	{
		renderer.drawParticle(i->getPos());
	}
	renderer.endFrame();
}

State ParticleSystem::evalFunc(const State& state, const float dt)
{
	FW::Vec3f dx = state.m_vec * dt;
	FW::Vec

}