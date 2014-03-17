#include "System.h"
#include "Forces.h"
#include "Renderer.h"
#include "Integrator.h"
#include "FlowGrid.h"

ActorContainer::ActorContainer(void* d1 = nullptr, void* d2 = nullptr) :
	status(ActorContainer::ActiveStatus_Unactive),
		actor(nullptr),
		data1(d1),
		data2(d2)
	{
	}

template<typename System>
StateEval Particle<System>::evalF(const float, const State&, ActorContainer&) {}

template<>
StateEval Particle<ParticleSystem>::evalF(const float dt, const State& current, ActorContainer&)
{
	StateEval next = StateEval();
	next.dx = current.vel * dt;
	next.dv = (m_invertMass* (evalGravity(m_mass) + evalDrag(current.vel, 1.0f))) * dt;
	return next;
}

template<>
StateEval Particle<BoidSystem>::evalF(const float, const State&, ActorContainer&)
{
	StateEval next = StateEval();
	//next.dv = boidEvalSpeed((current.vel + current.acc * dt), close) - current.vel;
	//next.dx = m_invertMass * (boidEvalCohersion(current.pos, close, 0.01f) + boidEvalSepartion(current.pos, close, 0.01f) + boidEvalToPointStatic(2.0f, 10.0f, current.pos, origin));
	return next;
}

template<>
StateEval Particle<FlowSystem>::evalF(const float dt, const State& current, ActorContainer& c)
{
	StateEval next = StateEval();
	FW::Vec3f speed = ((FlowControl*) c.data2)->getSpeed(current.pos, dt) + FW::Vec3f(.0f, -.1, .0f);
	if (speed == FW::Vec3f())
		m_lifetime = 0.0f;
	next.dx = speed * dt;
	next.dv = speed;
	return next;
}