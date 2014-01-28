#include "Integrator.h"
#include "Actor.h"


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

void EulerIntegrator::evalIntegrator(const float dt, Actor* actor, std::vector<Actor*>& actorlist)
{
	actor->setState(actor->evalF(dt, actor->getState(), actorlist));
	actor->setTime(dt);
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

void Runge_KuttaIntegrator::evalIntegrator(const float dt, Actor* actor, std::vector<Actor*>& actors)
{
	State current = actor->getState();
	State k1, k2, k3, k4;

	k1 = actor->evalF(dt, current, actors);
	k2 = State();
}

/*
        vector<osg::Vec3f> currentState;
        vector<osg::Vec3f> finalState;

        vector<osg::Vec3f> k1;
        vector<osg::Vec3f> k2;
        vector<osg::Vec3f> k3;
        vector<osg::Vec3f> k4;

        currentState = ps->getState();

        //1st step
        k1 = ps->evalF(currentState);

        //2nd step
        for (size_t i = 0; i < currentState.size(); i++)
        {
            k2.push_back(currentState[i]  + k1[i] * 0.5f * h);
        }
        k2 = ps->evalF(k2);

        //3rd step
        for (size_t i = 0; i < currentState.size(); i++)
        {
            k3.push_back(currentState[i]  + k2[i] * 0.5f * h);
        }
        k3 = ps->evalF(k3);

        //4th step
        for (size_t i = 0; i < currentState.size(); i++)
        {
            k4.push_back(currentState[i]  + k3[i] * h);
        }
        k4 = ps->evalF(k4);

        //getting final state
        for (size_t i = 0; i < currentState.size(); i++)
        {
            finalState.push_back(currentState[i]  + ( k1[i] + k2[i] * 2.0f + k3[i] * 2.0f + k4[i]) * (h/6.0f));
        }
        ps->setState(finalState);
        return ps->updateParticeles();
*/