#pragma once

#include "base/Random.hpp"
#include <vector>

class Actor;

class System
{
public:
	System() {}
	virtual ~System() 
	{
		for(auto i : m_actors)
			delete i;
	}

	virtual void evalSystem(const float);
	virtual void draw(const float);

protected:
	std::vector<Actor*> m_actors;

};

class ParticleSystem : public System
{
public:
	ParticleSystem();
	virtual ~ParticleSystem() {}
};

class BoidSystem : public System
{
public:
	BoidSystem(const float, const size_t);
	virtual ~BoidSystem() {}
	virtual void evalSystem(const float);

private:
	std::vector<Actor*> m_closeBuffer;
	float m_closeDistance;
	size_t m_numberOfParticles;
};