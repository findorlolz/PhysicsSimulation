#include "System.h"
#include "Forces.h"
#include "Renderer.h"
#include "Integrator.h"

template<typename System>
StateEval TraingleEmitter<System>::evalF(const float, const State&,std::vector<Actor*>&) {}

template<>
StateEval TraingleEmitter<ParticleSystem>::evalF(const float dt, const State&, std::vector<Actor*>& actors)
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

		Actor* particle = new ParticleEmitter<ParticleSystem>(m_particleMass,p,v,m_lifetimeOfParticles, m_lifetimeOfParticles * 0.5f, m_timeBetween, m_minSpeed, m_maxSpeed);
		//Actor* particle = new Particle<ParticleSystem>(m_particleMass,p,v,m_lifetimeOfParticles);
		actors.push_back(particle);
		tmp -= m_timeBetween;
	}
	m_previousTick = tmp;
	return StateEval();
}

template<typename System>
StateEval Particle<System>::evalF(const float, const State&,std::vector<Actor*>&) {}

template<>
StateEval Particle<ParticleSystem>::evalF(const float dt, const State& current, std::vector<Actor*>&)
{
	StateEval next = StateEval();
	next.dx = current.vel * dt;
	next.dv = (m_invertMass* (evalGravity(m_mass) + evalDrag(current.vel, 1.0f))) * dt;
	return next;
}

template<>
StateEval Particle<BoidSystem>::evalF(const float dt, const State& current, std::vector<Actor*>& close)
{
	StateEval next = StateEval();
	//next.dv = boidEvalSpeed((current.vel + current.acc * dt), close) - current.vel;
	next.dx = m_invertMass * (boidEvalCohersion(current.pos, close, 0.01f) + boidEvalSepartion(current.pos, close, 0.01f) + boidEvalToPointStatic(2.0f, 10.0f, current.pos, origin));
	return next;
}

template<typename System>
StateEval ParticleEmitter<System>::evalF(const float, const State&, std::vector<Actor*>&) {}

template<>
StateEval ParticleEmitter<ParticleSystem>::evalF(const float dt, const State&, std::vector<Actor*>& actors)
{
	float tmp = dt + m_previousTick;
	while(tmp >= m_timeBetween)
	{

		float r3 = m_randomGen.getF32(m_minSpeed*0.5f, m_maxSpeed*0.5f);
		FW::Vec2f rndUnitSquare = m_randomGen.getVec2f(0.0f,1.0f);
		FW::Vec2f rndUnitDisk = toUnitDisk(rndUnitSquare);
		FW::Mat3f formBasisMat = formBasis(FW::Vec3f(0.0f, 1.0f, 0.0f));
		FW::Vec3f rndToUnitHalfSphere = FW::Vec3f(rndUnitDisk.x, rndUnitDisk.y, FW::sqrt(1.0f-(rndUnitDisk.x*rndUnitDisk.x)-(rndUnitDisk.y*rndUnitDisk.y)));
		FW::Vec3f v = r3*(formBasisMat*rndToUnitHalfSphere);	

		Actor* particle = new Particle<ParticleSystem>(m_carrier->getMass(), m_carrier->getState().pos, v, m_lifetimeOfParticles);
		actors.push_back(particle);
		tmp -= m_timeBetween;
	}
	m_previousTick = tmp;
	Runge_KuttaIntegrator integrator = Runge_KuttaIntegrator::get();
	integrator.evalIntegrator(dt, m_carrier, actors);
	m_state = m_carrier->getState();
	return StateEval();
}

template<typename System>
StateEval Deflecter<System>::evalF(const float, const State&,std::vector<Actor*>&) {}

template<>
StateEval Deflecter<BoidSystem>::evalF(const float, const State&,std::vector<Actor*>& close)
{
	for(auto i : close)
	{
		if(i->getActorType() == ActorType_Particle)
		{
			Particle<System>* particle = (Particle<System>*) i;
			FW::Vec3f particlePos = particle->getPos();
			FW::Vec3f push = boidEvalFromPointSpring(m_range, m_k, particlePos, m_state.pos);
			float tmp = push.length();
			FW::Vec3f pull = getVortexSpringDirection(particlePos, m_state.pos, m_range);
			pull = pull.normalized() * tmp;
			FW::Vec3f a = particle->getInverseMass() * (0.2f*push + 0.8f*pull);
			//particle->setAcc(particle->getState().acc + a);
		}
	}
	return StateEval();
}
