#include "FlowGrid.h"

FlowControl::FlowControl(const float interval, FW::Vec3f startPoint, const size_t l, const size_t h, const size_t w, const float gridSize, const float flowSpeed)
{
	m_interval = interval;
	m_tick = 0.0f;
	m_grid1 = new FlowGrid(startPoint, l, h, w, gridSize, flowSpeed);
	m_grid2 = new FlowGrid(startPoint, l, h, w, gridSize, flowSpeed);
}

FlowControl::~FlowControl()
{
	delete m_grid1;
	delete m_grid2;
}

FW::Vec3f FlowControl::getSpeed(const FW::Vec3f& pos, const float dt) 
{
	m_tick += dt;
	while(m_tick > m_interval)
		m_tick -= m_interval;
	float t = m_tick/m_interval;
	return t * m_grid1->getSpeed(pos) + (1-t) * m_grid2->getSpeed(pos);
}

FlowGrid::FlowGrid(FW::Vec3f startPoint, const size_t l, const size_t h, const size_t w, const float gridSize, const float flowSpeed)
{
	m_l = l - 1;
	m_h = h - 1;
	m_w = w - 1;
	m_1D = w;
	m_2D = h * w;
	m_3D = h * w * l;
	m_start = startPoint;
	m_flowSpeed = flowSpeed;
	m_gridSize = gridSize;
	m_data = std::vector<FW::Vec3f>(m_3D);
	init();
};

void FlowGrid::init()
{
	FW::Random randomGen = FW::Random();
	FW::Mat3f formBasisMatrix = formBasis(FW::Vec3f(1.0f, 0.0f, 0.0f));	
	
	for(auto i = 0u; i < m_3D; ++i)
	{
		FW::Vec2f rndUnitSquare = randomGen.getVec2f(0.0f,1.0f);
		FW::Vec2f rndUnitDisk = toUnitDisk(rndUnitSquare);
		FW::Vec3f rndToUnitHalfSphere = FW::Vec3f(rndUnitDisk.x, rndUnitDisk.y, FW::sqrt(1.0f-(rndUnitDisk.x*rndUnitDisk.x)-(rndUnitDisk.y*rndUnitDisk.y)));
		FW::Vec3f v = formBasisMatrix*rndToUnitHalfSphere;
		m_data[i] = v;
	}
}

FW::Vec3f FlowGrid::getSpeed(const FW::Vec3f& pos) const
{
	int l, h, w;
	float dl, dh, dw;
	FW::Vec3f d = pos-m_start;
	bool yFlag = false; bool zFlag = false;

	l = indexBasedOnFloat(m_gridSize, d.x, dl);
	h = indexBasedOnFloat(m_gridSize, d.y, dh);
	w = indexBasedOnFloat(m_gridSize, d.z, dw);
	
	FW::Vec3f total;
	size_t index = l * m_2D + h * m_1D + w;
	
	if(l >= m_l)
		return total;
	else
		total += dl * m_data[m_2D+index] + (1.0f - dl) *  m_data[index];
	
	if(h >= m_h || h < 0)
		yFlag = true;
	else
		total += dh * m_data[index + m_1D] + (1.0f - dh) *  m_data[index];

	if(w >= m_w || w < 0)
		zFlag = true;
	else
		total += dw * m_data[index + 1] + (1.0f - dw) *  m_data[index];

	if(yFlag)
	{
		FW::Vec3f centerDir = FW::Vec3f(.0f, -pos.y, .0f).normalized();
		total += dh * centerDir;
	}

	if(zFlag)
	{
		FW::Vec3f centerDir = FW::Vec3f(.0f, .0f, -pos.z).normalized();
		total += dw * centerDir;
	}

	return total.normalized() * m_flowSpeed;
}