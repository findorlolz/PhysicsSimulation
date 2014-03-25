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
		m_dynamicMesh = new FW::Mesh<FW::VertexPNC>;
		m_dynamicMesh->addSubmesh();
		m_scale = 1.0f;
		m_staticMesh = nullptr;
}

void Renderer::shutDown()
{
	delete m_dynamicMesh;
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
	m_scale = scale;
	if(m_drawAxis)
	{
		FW::MeshBase* mesh = m_assetManager->getMesh(MeshType_Axis);
		mesh->draw(m_context, m_worldToCamera, m_projection);
	}
	if(m_staticMesh)
		m_staticMesh->draw(m_context, m_worldToCamera, m_projection);
}

void Renderer::clearDynamicMesh()
{
	m_dynamicMesh->clear();
	m_dynamicMesh->addSubmesh();
}

void Renderer::exportDynamicMesh()
{
	m_assetManager->exportMesh("dynamicMesh.obj", m_dynamicMesh);
}

void Renderer::endFrame()
{
	m_dynamicMesh->draw(m_context, m_worldToCamera, m_projection);
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

void Renderer::drawTriangleToCamera(const FW::Vec3f& pos, const FW::Vec4f& color)
{
	const float particleSize = m_scale;
	FW::Vec3f n = (m_camera->getPosition() - pos).normalized();
	FW::Vec3f t = n.cross(m_camera->getUp());
	FW::Vec3f b = n.cross(t);

	FW::VertexPNC vertexArray[] =
    {
		FW::VertexPNC((pos + t * particleSize + b * particleSize), n, color),
        FW::VertexPNC((pos - t * particleSize + b * particleSize), n, color),
		FW::VertexPNC((pos + t * particleSize - b * particleSize), n, color),
		FW::VertexPNC((pos - t * particleSize - b * particleSize), n, color)
	};

	static const FW::Vec3i indexArray[] =
    {
        FW::Vec3i(0,1,2),  FW::Vec3i(1,3,2)
    };

	int base = m_dynamicMesh->numVertices();
    m_dynamicMesh->addVertices(vertexArray, FW_ARRAY_SIZE(vertexArray));

    FW::Array<FW::Vec3i>& indices = m_dynamicMesh->mutableIndices(0);
    for (int i = 0; i < (int)FW_ARRAY_SIZE(indexArray); i++)
        indices.add(indexArray[i] + base);
}

void Renderer::drawTriangleToCameraTest(const FW::Vec3f& pos)
{
	const float particleSize = 30.0f * m_scale;
	FW::Vec3f n = (m_camera->getPosition() - pos).normalized();
	FW::Vec3f t = n.cross(m_camera->getUp());
	FW::Vec3f b = n.cross(t);
	FW::Vec4f color = FW::Vec4f(1.0f,0.5f,0.0f,1.0f);

	FW::VertexPNC vertexArray[] =
    {
		FW::VertexPNC((pos + t * particleSize + b * particleSize), n, color),
        FW::VertexPNC((pos - t * particleSize + b * particleSize), n, color),
		FW::VertexPNC((pos + t * particleSize - b * particleSize), n, color),
		FW::VertexPNC((pos - t * particleSize - b * particleSize), n, color)
	};

	static const FW::Vec3i indexArray[] =
    {
        FW::Vec3i(0,1,2),  FW::Vec3i(1,3,2)
    };

	FW::Mesh<FW::VertexPNC> mesh = FW::Mesh<FW::VertexPNC>();
	int submesh = mesh.addSubmesh();

    int base = mesh.numVertices();
    FW::VertexPNC* vertexPtr = mesh.addVertices(vertexArray, FW_ARRAY_SIZE(vertexArray));

    FW::Array<FW::Vec3i>& indices = mesh.mutableIndices(submesh);
    for (int i = 0; i < (int)FW_ARRAY_SIZE(indexArray); i++)
        indices.add(indexArray[i]);
	mesh.draw(m_context, m_worldToCamera * m_meshScale, m_projection);
}

void Renderer::initStaticMeshRenderer(std::vector<FW::Vec3f>& vertices, std::vector<Triangle>& triangleList, std::vector<TriangleToMeshData>& data, MeshType type)
{
	m_staticMesh = m_assetManager->getMesh(type);

	vertices.clear();
	vertices.reserve( m_staticMesh->numVertices() );
	triangleList.clear();
	triangleList.reserve(m_staticMesh->numTriangles() );
	data.clear();
	data.reserve(m_staticMesh->numTriangles() );

	for ( int i = 0; i < m_staticMesh->numVertices(); ++i )
	{
		FW::Vec3f p = m_staticMesh->getVertexAttrib( i, FW::MeshBase::AttribType_Position ).getXYZ();
		vertices.push_back(p);
	}
	for ( int i = 0; i < m_staticMesh->numSubmeshes(); ++i )
	{
		const FW::Array<FW::Vec3i>& idx = m_staticMesh->indices( i );
		for ( int j = 0; j < idx.getSize(); ++j )
		{
			TriangleToMeshData m;
			m.submeshIndex = i;
			m.vertexIndex = j;
			data.push_back( m );

			Triangle t;
			t.m_userPointer = 0;
			t.m_vertices[0] = &vertices[0] + idx[j][0];
			t.m_vertices[1] = &vertices[0] + idx[j][1];
			t.m_vertices[2] = &vertices[0] + idx[j][2];
			triangleList.push_back( t );
		}
	}

	for ( size_t i = 0; i < triangleList.size(); ++i )
		triangleList[ i ].m_userPointer = &data[ i ];
}