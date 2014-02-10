#include "Integrator.h"
#include "Actor.h"
#include "System.h"


EulerIntegrator& EulerIntegrator::get()
{
	static EulerIntegrator* gpSingleton = nullptr;
	if (gpSingleton == nullptr)
	{
			gpSingleton = new EulerIntegrator();
	}
	FW_ASSERT(gpSingleton != nullptr && "Failed to create EulerIntegrator");
	return *gpSingleton;
}

void EulerIntegrator::evalIntegrator(const float dt, ActorContainer& c)
{
	Actor* a = c.actor;
	State current = a->getState();
	StateEval tmp = a->evalF(dt, current, c);
	current.updateState(tmp);
	a->setTime(dt);
	a->setStateBuffer(current);
}

Runge_KuttaIntegrator& Runge_KuttaIntegrator::get()
{
	static Runge_KuttaIntegrator* gpSingleton = nullptr;
	if (gpSingleton == nullptr)
	{
			gpSingleton = new Runge_KuttaIntegrator();
	}
	FW_ASSERT(gpSingleton != nullptr && "Failed to create Runge_KuttaIntegrator");
	return *gpSingleton;
}

void Runge_KuttaIntegrator::evalIntegrator(const float dt, Actor* actor, ActorContainer& c)
{
	State current = actor->getState();
	StateEval tmp;
	const float f = 1.0f/6.0f;

	State k1 = current;
	State k2 = current;
	State k4 = current;
	State k3 = current;

	tmp = actor->evalF(dt, current, c);
	k1.updateState(tmp);
	current.updateState(tmp, f);
	tmp = actor->evalF(dt*0.5f, k1, c);
	k2.updateState(tmp);
	current.updateState(tmp,2.0*f);
	tmp = actor->evalF(dt*0.5f, k2, c);
	k3.updateState(tmp);
	current.updateState(tmp,2.0*f);
	tmp = actor->evalF(dt, k3, c);
	k4.updateState(tmp);
	current.updateState(tmp, f);
	actor->setStateBuffer(current);
	actor->setTime(dt);
}

void Runge_KuttaIntegrator::evalIntegrator(const float dt, ActorContainer& c)
{
	Actor* actor = c.actor;
	State current = actor->getState();
	StateEval tmp;
	const float f = 1.0f/6.0f;

	State k1 = current;
	State k2 = current;
	State k4 = current;
	State k3 = current;

	tmp = actor->evalF(dt, current, c);
	k1.updateState(tmp);
	current.updateState(tmp, f);
	tmp = actor->evalF(dt*0.5f, k1, c);
	k2.updateState(tmp);
	current.updateState(tmp,2.0*f);
	tmp = actor->evalF(dt*0.5f, k2, c);
	k3.updateState(tmp);
	current.updateState(tmp,2.0*f);
	tmp = actor->evalF(dt, k3, c);
	k4.updateState(tmp);
	current.updateState(tmp, f);
	actor->setStateBuffer(current);
	actor->setTime(dt);
}