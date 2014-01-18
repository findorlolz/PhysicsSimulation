#pragma once

#include "base/Math.hpp"
#include <vector>

static const FW::Vec3f gAcc= FW::Vec3f(0.0f, -9.81f, 0.0f);

// Gravity divided by mass.
inline static FW::Vec3f evalGravity(const float mass) 
{
	return mass * FW::Vec3f(0,-9.8,0);
}

inline static FW::Vec3f evalDrag(const FW::Vec3f& velocity, const float invertMass, const float dragConstant) 
{
	FW::Vec3f dragForce;
	dragForce = -dragConstant * invertMass * velocity;
    return dragForce;
}

static FW::Vec3f evaluateSpring(const float dist0, const float k, const float inverseMass, const FW::Vec3f& node, const FW::Vec3f& neighbor) 
{
	FW::Vec3f springForce;
	FW::Vec3f d;
	d = node - neighbor;
	springForce = (-k * (d.length() - dist0)) * d.normalized();  
    return springForce * inverseMass;
}

static FW::Vec3f boidEvalCohersion(const FW::Vec3f& node, const std::vector<FW::Vec3f>& neighbors, const float k)
{
	FW::Vec3f centerMass;
	if (neighbors.size() == 0)
		return FW::Vec3f();
	for (auto i = 0u; i <neighbors.size(); i++)
	{
		centerMass += neighbors[i];
	}
	centerMass = centerMass/ static_cast<float>(neighbors.size());
	FW::Vec3f f = ((centerMass - node) * k);
	return f;
}

static FW::Vec3f boidEvalSpeed(FW::Vec3f speed, std::vector<FW::Vec3f> neighbors)
{
		
	FW::Vec3f boidSpeed = speed;
	if (neighbors.size() == 0)
	return speed;
	for (auto i = 0u; i <neighbors.size(); i++)
	{
		boidSpeed += neighbors[i];
	}
	boidSpeed = boidSpeed/ ((float) neighbors.size());
	FW::Vec3f f = speed * 0.85f + boidSpeed * 0.15f;
	return f;
}
static FW::Vec3f boidEvalSepartion(FW::Vec3f node, std::vector<FW::Vec3f> neighbors, float k)
{
	FW::Vec3f f;
	for (size_t i = 0; i <neighbors.size(); i++)
	{
		f += (node - neighbors[i]) * k;
	}
	return f;
}

static bool compareDistance (FW::Vec3f a, FW::Vec3f b, float distance)
{
	float n = (a-b).length();
	if (n <= distance && n != 0.0f)
		return true;
	else 
		return false;
}

static float neg( float num)
{
	int i = rand()%2;
	if (i == 0)
		return num * (-1.0f);
	if (i == 1)
		return num;
}

static FW::Vec3f createRandomForce(int k)
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
}