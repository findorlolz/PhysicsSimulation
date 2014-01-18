#pragma once

#include "base/Math.hpp"
#include "base/Random.hpp"
#include "HelpFunctions.h"
#include "Renderer.h"
#include <vector>

class System;

class Actor
{
public:
	Actor() {}
	virtual ~Actor() {}

	virtual void evalF(const float, std::vector<Actor*>&) {}
	virtual FW::Vec3f getPos() const { return FW::Vec3f(); }
	virtual bool isDone() const { return false; }
	virtual void draw(Renderer&) {}
};

template<typename System>
class Particle : public Actor
{
public:
	Particle(const float mass, const FW::Vec3f p, const FW::Vec3f v, const float duration) :
		m_mass(mass),
		m_invertMass(1.0f/mass),
		m_position(p),
		m_velocity(v),
		m_acceleration(0.0f),
		m_duration(duration),
		m_timeElapsed(0.0f)
	{}
	~Particle() {}

	FW::Vec3f getAcc() const { return m_acceleration; }
	FW::Vec3f getVel() const { return m_velocity; }
	FW::Vec3f getPos() const { return m_position; }
	float getMass() const { return m_mass; }
	float getDuration() const { return m_duration; }
	float getTimeElapset() const {return m_timeElapsed; }
	float getInverseMass()  const { return m_invertMass; }

	void setState(const FW::Vec3f& p, const FW::Vec3f& v, const FW::Vec3f& a) { m_position = p; m_velocity = v; m_acceleration = a; }
	void addToTimer(const float d) { m_timeElapsed += d;}
	
	virtual bool isDone() const { return (m_timeElapsed > m_duration); }
	virtual void evalFunc(const float, std::vector<Actor*>&);
	virtual void draw(Renderer& renderer) { renderer.drawParticle(m_position); }

private:
	FW::Vec3f m_acceleration;
	FW::Vec3f m_velocity;
	FW::Vec3f m_position;
	float m_mass;
	float m_invertMass;
	float m_duration;
	float m_timeElapsed;
};

template<typename System>
class Emitter : public Actor
{
public:
	Emitter(const FW::Vec3f& posa, const FW::Vec3f& posb, const FW::Vec3f& posc, const float minSpeed,
		const float maxSpeed, const float timeBetweenParticles, const float particleMass, const float particleDuration) :
		m_posA(posa),
		m_posB(posb),
		m_posC(posc),
		m_minSpeed(minSpeed),
		m_maxSpeed(maxSpeed),
		m_timeBetween(timeBetweenParticles),
		m_particleMass(particleMass),
		m_particleDuration(particleDuration),
		m_previousTick(0.0f)
	{
		m_normal = FW::cross((m_posB - m_posA), (m_posC - m_posA));
		m_normal = m_normal.normalized();
		m_formBasis = formBasis(m_normal);
		m_randomGen = FW::Random();
	}
	~Emitter() 
	{
	}
	
	void evalF(const float, std::vector<Actor*>&);

private:
	FW::Random m_randomGen;
	FW::Mat3f m_formBasis;
	FW::Vec3f m_posA;
	FW::Vec3f m_posB;
	FW::Vec3f m_posC;
	FW::Vec3f m_normal;
	float m_timeBetween;
	float m_previousTick;
	float m_minSpeed;
	float m_maxSpeed;
	float m_particleMass;
	float m_particleDuration;
};