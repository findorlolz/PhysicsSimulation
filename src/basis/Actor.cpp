#include "Actor.h"
#include "Renderer.h"
#include "System.h"
#include "base/Random.hpp"
#include "Forces.h"

template<typename System>
void Emitter<System>::evalF(const float, std::vector<Actor*>&) {}

template<>
void Emitter<ParticleSystem>::evalF(const float dt, std::vector<Actor*>& actors)
{
	
	float tmp = dt + m_previousTick;
	while(tmp >= m_timeBetween)
	{
		//Position
		float sqr_r1 = FW::sqrt(m_randomGen.getF32(0,1.0f));
		float r2 = m_randomGen.getF32(0,1.0f);
		FW::Vec3f p = (1-sqr_r1)*m_posA + sqr_r1*(1-r2)*m_posB + sqr_r1*r2*m_posC;
		//Speed
		float r3 = m_randomGen.getF32(m_minSpeed, m_maxSpeed);
		FW::Vec2f rndUnitSquare = m_randomGen.getVec2f(0.0f,1.0f);
		FW::Vec2f rndUnitDisk = toUnitDisk(rndUnitSquare);
		FW::Vec3f rndToUnitHalfSphere = FW::Vec3f(rndUnitDisk.x, rndUnitDisk.y, FW::sqrt(1.0f-(rndUnitDisk.x*rndUnitDisk.x)-(rndUnitDisk.y*rndUnitDisk.y)));
		FW::Vec3f v = r3 * (m_formBasis*rndToUnitHalfSphere);	

		Actor* particle = new Particle<ParticleSystem>(m_particleMass, p, v, m_particleDuration);
		actors.push_back(particle);
		tmp -= m_timeBetween;
	}
	m_previousTick = tmp;
}

template<typename System>
void Particle<System>::evalF(const float, std::vector<Actor*>&)
{
	std::cout << "Using default evalF" << std::endl;
}

template<>
void Particle<ParticleSystem>::evalF(const float dt, std::vector<Actor*>& actors)
{
	addToTimer(dt);
	m_position += m_velocity * dt;
	m_velocity += m_acceleration * dt;
	m_acceleration = m_invertMass* (evalGravity(m_mass) + evalDrag(m_velocity, m_invertMass, 0.01f));
}

