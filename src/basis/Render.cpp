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

void Renderer::render(const std::vector<FW::Vec3f>& positions)
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	FW::Mat4f worldToCamera = m_camera->getWorldToCamera();
	m_projection = m_context->xformFitToView(FW::Vec2f(-1.0f, -1.0f), FW::Vec2f(2.0f, 2.0f)) * m_camera->getCameraToClip();
	for (auto pos : positions)
    {
		FW::Mat4f toCamera = FW::Mat4f::translate(pos);
		MeshType meshType = MeshType_Sphere;					
		FW::MeshBase* mesh = m_assetManager->getMesh(meshType);
		mesh->draw(m_context, worldToCamera * toCamera, m_projection);
	}	
	glBindVertexArray(0);
	glUseProgram(0);
}

