#include "Renderer.h"
#include "base/Main.hpp"

void Renderer::startUp(FW::GLContext* gl, FW::CameraControls* camera, AssetManager* assetManager)
{
		m_context = gl; 
        m_assetManager = assetManager;
		m_camera = camera;
		m_projection = FW::Mat4f();
		m_meshScale = FW::Mat4f();
		m_camera->setPosition(FW::Vec3f(0.0f, 0.0f, 3.0f));
		m_camera->setFar(5.0f);
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

void Renderer::renderTest(const float scale)
{
	MeshType meshType = MeshType_Sphere;					
	FW::MeshBase* mesh = m_assetManager->getMesh(meshType);
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	FW::Mat4f worldToCamera = m_camera->getWorldToCamera();
	FW::Mat4f scaling = FW::Mat4f::scale(FW::Vec3f(scale, scale, scale));
	m_projection = m_context->xformFitToView(FW::Vec2f(-1.0f, -1.0f), FW::Vec2f(2.0f, 2.0f)) * m_camera->getCameraToClip();
	if (!m_context->getConfig().isStereo)
	{
		mesh->draw(m_context, worldToCamera * scaling, m_projection);
		return;
	}
	FW::Mat4f worldToCameraLeft = m_camera->getCameraToLeftEye();
	FW::Mat4f worldToCameraRight = m_camera->getCameraToRightEye();
	glDrawBuffer(GL_BACK_LEFT);
	mesh->draw(m_context, worldToCameraLeft, m_projection);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_BACK_RIGHT);
	mesh->draw(m_context, worldToCameraRight, m_projection);
	glDrawBuffer(GL_BACK);
	glBindVertexArray(0);
	glUseProgram(0);
}
