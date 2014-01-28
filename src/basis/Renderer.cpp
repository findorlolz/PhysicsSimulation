#include "Renderer.h"
#include "base/Main.hpp"

void Renderer::startUp(FW::GLContext* gl, FW::CameraControls* camera, AssetManager* assetManager, bool axis)
{
		m_context = gl; 
        m_assetManager = assetManager;
		m_camera = camera;
		m_drawAxis = axis;
		m_projection = FW::Mat4f();
		m_meshScale = FW::Mat4f();
		m_camera->setPosition(FW::Vec3f(0.0f, 3.0f, 5.0f));
		m_camera->setFar(20.0f);
		m_camera->setSpeed(4.0f);
}

void Renderer::shutDown()
{
	delete &get();
}

void Renderer::startFrame(const float scale)
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	m_projection = m_context->xformFitToView(FW::Vec2f(-1.0f, -1.0f), FW::Vec2f(2.0f, 2.0f)) * m_camera->getCameraToClip();
	m_worldToCamera = m_camera->getWorldToCamera();
	m_meshScale = FW::Mat4f::scale(FW::Vec3f(scale, scale, scale));
	if(m_drawAxis)
	{
		FW::MeshBase* mesh = m_assetManager->getMesh(MeshType_Axis);
		mesh->draw(m_context, m_worldToCamera, m_projection);
	}
}

void Renderer::endFrame()
{
	glDrawBuffer(GL_BACK);
	glBindVertexArray(0);
	glUseProgram(0);
}


void Renderer::drawParticle(const FW::Vec3f pos)
{

	FW::Mat4f toCamera = FW::Mat4f::translate(pos);					
	FW::MeshBase* mesh = m_assetManager->getMesh(MeshType_Sphere);
	mesh->draw(m_context, m_worldToCamera * toCamera * m_meshScale, m_projection);
}

void Renderer::drawPyramid(const FW::Vec3f pos)
{

	FW::Mat4f toCamera = FW::Mat4f::translate(pos);					
	FW::MeshBase* mesh = m_assetManager->getMesh(MeshType_Pyramid);
	mesh->draw(m_context, m_worldToCamera * toCamera * m_meshScale, m_projection);
}
