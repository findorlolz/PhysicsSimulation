#pragma once

#include "Actor.h"

static const FW::Vec3f gAcc = FW::Vec3f(0.0f, -9.81f, 0.0f);
static const FW::Vec3f origin = FW::Vec3f();

inline static FW::Vec3f evalGravity(const float mass) 
{
	return mass * gAcc;
}

inline static FW::Vec3f evalDrag(const FW::Vec3f& velocity, const float dragConstant) 
{
	FW::Vec3f dragForce;
	dragForce = -dragConstant * velocity;
    return dragForce;
}

inline static FW::Vec3f evalSpring(const float dist0, const float k, const FW::Vec3f& node, const FW::Vec3f& neighbor) 
{
	FW::Vec3f springForce;
	FW::Vec3f d;
	d = node - neighbor;
	springForce = (-k * (d.length() - dist0)) * d.normalized();  
    return springForce;
}

inline static FW::Vec3f boidEvalToPointSpring(const float dist0, const float k, const FW::Vec3f& node, const FW::Vec3f& neighbor)
{
	FW::Vec3f springForce;
	FW::Vec3f d = node - neighbor;
	float tmp = d.length() - dist0;

	if(tmp < 0.0f)
		return springForce;
	
	springForce = -k * tmp * d.normalized();  
    return springForce;
}

inline static FW::Vec3f boidEvalFromPointSpring(const float dist0, const float k, const FW::Vec3f& node, const FW::Vec3f& neighbor)
{
	FW::Vec3f springForce;
	FW::Vec3f d = node - neighbor;
	float tmp = d.length() - dist0;

	if(tmp > 0.0f)
		return springForce;
	
	springForce = (-k * tmp) * d.normalized();  
    return springForce;
}

inline static FW::Vec3f boidEvalToPointStatic(const float dist0, const float k, const FW::Vec3f& node, const FW::Vec3f& neighbor)
{
	FW::Vec3f f;
	FW::Vec3f d = node - neighbor;
	float tmp = d.length() - dist0;
	if(tmp <= 0.0f)
		return f;
	f = -1.0f*k*d.normalized();
	return f;

}

inline static FW::Vec3f boidEvalCohersion(const FW::Vec3f& node, const std::vector<Actor*>& neighbors, const float k)
{
	FW::Vec3f centerMass;
	if (neighbors.size() == 0)
		return FW::Vec3f();
	for (auto i = 0u; i <neighbors.size(); i++)
	{
		centerMass += neighbors[i]->getPos();
	}
	centerMass = centerMass/ static_cast<float>(neighbors.size());
	FW::Vec3f f = ((centerMass - node) * k);
	return f;
}

inline static FW::Vec3f boidEvalSpeed(const FW::Vec3f& speed, const std::vector<Actor*>& neighbors)
{
		
	FW::Vec3f boidSpeed = FW::Vec3f();
	if (neighbors.size() == 0)
		return speed;
	for (auto i = 0u; i <neighbors.size(); i++)
	{
		boidSpeed += neighbors[i]->getPos();
	}
	boidSpeed = boidSpeed/((float) neighbors.size());
	float d = boidSpeed.length();
	FW::Vec3f f = (speed * 0.85f + boidSpeed * 0.15f).normalized();
	return f*d;
}

inline static FW::Vec3f boidEvalSepartion(const FW::Vec3f& node, std::vector<Actor*>& neighbors, float k)
{
	FW::Vec3f f;
	for (size_t i = 0; i <neighbors.size(); i++)
	{
		f += (node - neighbors[i]->getPos()) * k;
	}
	return f;
}

inline static bool compareDistance (const FW::Vec3f& a, const FW::Vec3f& b, const float distance)
{
	float n = (a-b).length();
	if (n <= distance && n != 0.0f)
		return true;
	else 
		return false;
}

inline static float neg(float num)
{
	int i = rand()%2;
	if (i == 0)
		return num * (-1.0f);
	if (i == 1)
		return num;
}

inline static FW::Vec3f getVortexSpringDirection(const FW::Vec3f& particlePos, const FW::Vec3f& vortexPos, const float range)
{
	size_t s = 10u;
	float tmp = FLT_MAX;
	for(auto i = 0u; i < 3; ++i)
	{
		float value = FW::abs(particlePos[i]);
		if(value < tmp)
		{
			s = i;
			tmp = value;
		}
	}

	FW::Vec3f r = FW::Vec3f();
	for(auto i = 0u; i < 3; ++i)
	{
		if(i == s)
			continue;
		float value = particlePos[i];
		if(value >= 0.0f)
			r[i] = range + vortexPos[i];
		else
			r[i] = -1.0f * range + vortexPos[i];
	}
	return r;
}

/*
inline static FW::Vec3f createRandomForce(int k)
>>>>>>> Boids started
{
	FW::Vec3f v = FW::Vec3f(neg((float) (rand()%k)), neg((float) (rand()%k)), neg((float) (rand()%k)));
	return v;
}

static FW::Vec3f evalWind(FW::Vec3f wind, FW::Vec3f surfNormal)
{
	srand ( GetTickCount() );
	float randomizer = (float)rand() / 10000.0f;
	randomizer = FW::sin(randomizer) * FW::sin(randomizer);
	float normal = surfNormal.dot(wind.normalized());
	return surfNormal * randomizer * normal * 20.0f;
<<<<<<< HEAD
}
=======
}
*/
