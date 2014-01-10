#include "Render.h"
#include "base/Main.hpp"

void Renderer::startUp(FW::GLContext* gl, FW::CameraControls* camera, AssetManager* assetManager)
{
		m_context = gl; 
        m_assetManager = assetManager;
		m_camera = camera;
		m_projection = FW::Mat4f();

		m_camera->setFar(100);
}

void Renderer::shutDown()
{
}

void Renderer::render(const std::vector<FW::Vec3f>& positions, const float scale)
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	FW::Mat4f scaling = FW::Mat4f::scale(FW::Vec3f(scale, scale, scale));
	FW::Mat4f worldToCameraLeft = m_camera->getCameraToLeftEye();
	FW::Mat4f worldToCamaraRight = m_camera->getCameraToRightEye();
	m_projection = m_context->xformFitToView(FW::Vec2f(-1.0f, -1.0f), FW::Vec2f(2.0f, 2.0f)) * m_camera->getCameraToClip();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_BACK_LEFT);
	for (auto pos : positions)
    {
		FW::Mat4f toCamera = FW::Mat4f::translate(pos);
		MeshType meshType = MeshType_Sphere;					
		FW::MeshBase* mesh = m_assetManager->getMesh(meshType);
		mesh->draw(m_context, worldToCameraLeft * toCamera * scaling, m_projection);
	}
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_BACK_RIGHT);
	for (auto pos : positions)
    {
		FW::Mat4f toCamera = FW::Mat4f::translate(pos);
		MeshType meshType = MeshType_Sphere;					
		FW::MeshBase* mesh = m_assetManager->getMesh(meshType);
		mesh->draw(m_context, worldToCamaraRight * toCamera * scaling, m_projection);
	}
	glDrawBuffer(GL_BACK);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Renderer::renderTest()
{
	MeshType meshType = MeshType_Sphere;					
	FW::MeshBase* mesh = m_assetManager->getMesh(meshType);
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	FW::Mat4f worldToCamera = m_camera->getWorldToCamera();
	m_projection = m_context->xformFitToView(FW::Vec2f(-1.0f, -1.0f), FW::Vec2f(2.0f, 2.0f)) * m_camera->getCameraToClip();
	if (!m_context->getConfig().isStereo)
	{
		mesh->draw(m_context, worldToCamera, m_projection);
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
