#include "System.h"
#include "Forces.h"
#include "Renderer.h"

template<typename System>
void Particle<System>::draw(Renderer& renderer) {}

template<>
void Particle<ParticleSystem>::draw(Renderer& renderer)
{ 
	renderer.drawParticle(m_position);
}

template<>
void Particle<BoidSystem>::draw(Renderer& renderer)
{ 
	renderer.drawParticle(m_position);
}

template<typename System>
void TraingleEmitter<System>::evalF(const float, std::vector<Actor*>&) {}

template<>
void TraingleEmitter<ParticleSystem>::evalF(const float dt, std::vector<Actor*>& actors)
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
	m_acceleration = m_invertMass* (evalGravity(m_mass) + evalDrag(m_velocity, 0.01f));
}

template<>
void Particle<BoidSystem>::evalF(const float dt, std::vector<Actor*>& close)
{
	m_position += m_velocity * dt;
	m_velocity = boidEvalSpeed((m_velocity + m_acceleration * dt), close);
	m_acceleration = m_invertMass * (boidEvalCohersion(m_position, close, 0.001f) + boidEvalSepartion(m_position, close, 0.001f) + boidEvalToPointSpring(0.5f, 0.1f,m_position, FW::Vec3f()));
}

template<typename System>
void Deflecter<System>::evalF(const float dt, std::vector<Actor*>& close) {}

template<>
void Deflecter<BoidSystem>::evalF(const float dt, std::vector<Actor*>& close)
{
	for(auto i : close)
	{
		if(i->getActorType() == ActorType_Particle)
		{
			Particle<System>* particle = (Particle<System>*) i;
			FW::Vec3f particlePos = particle->getPos();
			FW::Vec3f push = boidEvalFromPointSpring(m_range, m_k, particlePos, m_position);
			float tmp = push.length();
			FW::Vec3f pull = getVortexSpringDirection(particlePos, m_position, m_range);
			pull = pull.normalized() * tmp;
			FW::Vec3f a = particle->getInverseMass() * (0.2f*push + 0.8f*pull);
			particle->setAcc(particle->getAcc() + a);
		}
	}
}
