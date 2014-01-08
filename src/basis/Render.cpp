#include "Render.h"
#include "base/Main.hpp"
#include "Shaders.h"

void Renderer::startUp(FW::GLContext* gl, Camera* camera, AssetManager* assetManager)
{
		m_context = gl; 
        m_assetManager = assetManager;
		m_camera = camera;
		m_shader = getBasicShader();
}

void Renderer::shutDown()
{
		if (m_shader)
                delete m_shader;
}

void Renderer::render(const std::vector<FW::Vec3f>& positions)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	m_shader->use();
	Mat4f posToClip;
	Mat4f rotation = m_camera->GetRotation();
	rotation.setCol(3, Vec4f(0, 0, m_camera->GetDistance(), 1));
	Mat4f perspective = m_camera->GetPerspective();
	posToClip = perspective * rotation;
	posToClip = posToClip.transposed();
	//m_context->setUniform(m_shader->getUniformLoc("posToClip"), posToClip);
	for (auto pos : positions)
    {
		FW::Mat4f transform;
		transform.setIdentity();
		MeshType meshType = MeshType_Pyramid;
        FW::MeshBase* mesh = m_assetManager->getMesh(meshType);
		drawMesh(mesh, transform);
	}	
	glBindVertexArray(0);
	glUseProgram(0);
}


void Renderer::drawMesh(FW::MeshBase* mesh, const FW::Mat4f& transform)
{
    if (mesh == nullptr)
		return;
	int posAttrib = mesh->findAttrib(FW::MeshBase::AttribType_Position);
	if (posAttrib == -1)
		return;

	mesh->setGLAttrib(m_context, posAttrib, m_shader->getAttribLoc("positionAttrib"));
	for (int i = 0; i < mesh->numSubmeshes(); i++)
	{
		const FW::MeshBase::Material& mat = mesh->material(i);
		m_context->setUniform(m_shader->getUniformLoc("diffuseUniform"), mat.diffuse);
		glDrawElements(GL_TRIANGLES, mesh->vboIndexSize(i), GL_UNSIGNED_INT, (void*)(FW::UPTR)mesh->vboIndexOffset(i));
	}
    m_context->resetAttribs();
}

