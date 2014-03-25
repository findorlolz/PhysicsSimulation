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
	FlowControl* flow = (FlowControl*) c.data2;
	FW::Vec3f speed = flow->getSpeed(current.pos, dt);
	if (speed == FW::Vec3f())
		m_lifetime = 0.0f;
	for(auto iter : c.createdActors)
	{
		speed += (((Vortex<FlowSystem>*) iter)->getVortexSpeed(this) * flow->getFlowSpeed());
	}
	next.dx = speed * dt;
	next.dv = speed;
	return next;
}

void Actor::collisionEffect(const Hit& hit)
{
	FW::Vec3f normal = FW::cross((*(hit.triangle.m_vertices[1]) - *(hit.triangle.m_vertices[0])), (*(hit.triangle.m_vertices[2]) - *(hit.triangle.m_vertices[0])));
	normal = normal.normalized();
	m_stateBuffer.pos = hit.intersectionPoint + normal * .001f;
	FW::Vec3f L = m_stateBuffer.vel * -1.f;
	FW::Vec3f newVel = normal * 2 * FW::dot(normal, L) - L;
	m_stateBuffer.vel = newVel; 
}