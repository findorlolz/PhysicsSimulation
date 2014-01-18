#pragma once

#include "Integrator.h"

class System
{
public:
	System() {}
	virtual ~System() 
	{
		for(auto i : m_actors)
			delete i;
	}

	virtual void evalSystem(const float) {}
	virtual void draw(const float) {}

protected:
	std::vector<Actor*> m_actors;

};

class ParticleSystem : public System
{
public:
	ParticleSystem() : 
		System()
	{
		m_emitter = new Emitter<ParticleSystem>(FW::Vec3f(1,0,0),FW::Vec3f(0,0,0), FW::Vec3f(0,0,1), 2.0f, 3.0f, 0.0005f, 0.10f, 0.5f);
	}
	virtual ~ParticleSystem() {}

	virtual void evalSystem(const float dt);
	virtual void draw(const float scale);

private:
	Emitter<ParticleSystem>* m_emitter;
};