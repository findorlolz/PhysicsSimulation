#pragma once

#include "AssetManager.h"
#include "Camera.h"

struct glGeneratedIndices
{
	GLuint static_vao, dynamic_vao;
	GLuint shader_program;
	GLuint static_vertex_buffer, dynamic_vertex_buffer;
	GLuint model_to_world_uniform, world_to_clip_uniform, shading_toggle_uniform, normal_transform;
};

enum VertexShaderAttributeLocations
{
	ATTRIB_POSITION = 0,
	ATTRIB_NORMAL = 1,
	ATTRIB_COLOR = 2
};

class Renderer
{
public:
        Renderer() {}
        ~Renderer() {}

        void startUp(FW::GLContext*, Camera*, AssetManager*);
        void shutDown();

        void render(const std::vector<FW::Vec3f>&);

private:
        void drawMesh(FW::MeshBase*, const FW::Mat4f&);
        FW::Mat4f rotationMatrix( const FW::Vec3f&, float) const;

private:
        FW::GLContext* m_context;
		glGeneratedIndices* m_glIndices;
		Camera* m_camera;
        AssetManager* m_assetManager;
        FW::GLContext::Program* m_shader;
};