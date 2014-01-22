#pragma once

#include "base/Math.hpp"
#include "base/Random.hpp"
#include "HelpFunctions.h"
#include <vector>

enum ActorType
{
	ActorType_Emitter,
	ActorType_Particle,
	ActorType_Deflecter
};

class System;
class Renderer;

class Actor
{
public:
	Actor(ActorType type) :
	m_type(type)
	{}
	virtual ~Actor() {}

	virtual ActorType getActorType() const { return m_type; }
	virtual void evalF(const float, std::vector<Actor*>&) {}
	virtual FW::Vec3f getPos() const { return FW::Vec3f(); }
	virtual bool isDone() const { return false; }
	virtual void draw(Renderer&) {}

protected:
	ActorType m_type;
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
		m_timeElapsed(0.0f),
		Actor(ActorType_Particle)
	{}
	~Particle() {}

	FW::Vec3f getAcc() const { return m_acceleration; }
	FW::Vec3f getVel() const { return m_velocity; }
	FW::Vec3f getPos() const { return m_position; }
	float getMass() const { return m_mass; }
	float getDuration() const { return m_duration; }
	float getTimeElapset() const {return m_timeElapsed; }
	float getInverseMass()  const { return m_invertMass; }

	void setAcc(const FW::Vec3f a) { m_acceleration = a; }
	void addToTimer(const float d) { m_timeElapsed += d; }
	
	virtual bool isDone() const { return (m_timeElapsed > m_duration); }
	virtual void evalF(const float, std::vector<Actor*>&);
	virtual void draw(Renderer& renderer);

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
class TraingleEmitter : public Actor
{
public:
	TraingleEmitter(const FW::Vec3f& posa, const FW::Vec3f& posb, const FW::Vec3f& posc, const float minSpeed,
		const float maxSpeed, const float timeBetweenParticles, const float particleMass, const float particleDuration) :
		m_posA(posa),
		m_posB(posb),
		m_posC(posc),
		m_minSpeed(minSpeed),
		m_maxSpeed(maxSpeed),
		m_timeBetween(timeBetweenParticles),
		m_particleMass(particleMass),
		m_particleDuration(particleDuration),
		m_previousTick(0.0f),
		Actor(ActorType_Emitter)
	{
		m_normal = FW::cross((m_posB - m_posA), (m_posC - m_posA));
		m_normal = m_normal.normalized();
		m_formBasis = formBasis(m_normal);
		m_randomGen = FW::Random();
	}
	~TraingleEmitter() 
	{
	}

	virtual void evalF(const float, std::vector<Actor*>&);

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

template<typename System>
class Deflecter : public Actor
{
public:
	Deflecter(const FW::Vec3f pos, const float range, const float k) :
		m_position(pos),
		Actor(ActorType_Deflecter)
	{}
	virtual ~Deflecter() {}

	virtual void evalF(const float, std::vector<Actor*>&);

private:
	FW::Vec3f m_position;
	float m_range;
	float m_k;
};