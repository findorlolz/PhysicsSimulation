#pragma once

#include "base/Math.hpp"
#include "base/Random.hpp"
#include "base/Timer.hpp"
#include "HelpFunctions.h"
#include <vector>

class FlowGrid
{
public:
	FlowGrid(FW::Vec3f, const size_t, const size_t, const size_t, const float, const float);
		~FlowGrid() {}

		FW::Vec3f getSpeed(const FW::Vec3f&) const;

private:
	void init();

	enum DirectionFlag
	{
		DirectionFlag_X,
		DirectionFlag_Y,
		DirectionFlag_Z,
		DirectionFlag_None,
	};

	std::vector<FW::Vec3f> m_data;
	FW::Vec3f m_start;
	float m_flowSpeed;
	float m_gridSize;
	int m_l;
	int m_h;
	int m_w;
	size_t m_1D;
	size_t m_2D;
	size_t m_3D;
};

class FlowControl
{
public:
	FlowControl(const float,FW::Vec3f, const size_t, const size_t, const size_t, const float, const float);
		~FlowControl();
	
	FW::Vec3f getSpeed(const FW::Vec3f&,const float);

private:
	float m_tick;
	float m_interval;
	FlowGrid* m_grid1;
	FlowGrid* m_grid2;
};