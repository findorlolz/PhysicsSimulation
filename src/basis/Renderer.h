#pragma once

#include "AssetManager.h"
#include "3d/CameraControls.hpp"
#include "HelpFunctions.h"

class Renderer
{
public:
        Renderer() {}
        ~Renderer() {}

        static Renderer& get()
        {
			static Renderer* gpSingleton = nullptr;
			if (gpSingleton == nullptr)
			{
					gpSingleton = new Renderer();
			}
			FW_ASSERT(gpSingleton != nullptr && "Failed to create Renderer");
			return *gpSingleton;
        }

		void startUp(FW::GLContext*, FW::CameraControls*, AssetManager*, bool);
        void shutDown();

		void startFrame(const float );
		void endFrame();
		
		void toggleAxis() { m_drawAxis = !m_drawAxis; }

		void drawParticle(const FW::Vec3f);
		void drawPyramid(const FW::Vec3f pos);
		void drawTriangleToCamera(const FW::Vec3f&, const FW::Vec4f&);
		void drawTriangleToCameraTest(const FW::Vec3f&);

		void initStaticMeshRenderer(std::vector<FW::Vec3f>&, std::vector<Triangle>&, std::vector<TriangleToMeshData>&, MeshType);
		void clearStaticMeshRenderer() { m_staticMesh = nullptr; }

		void clearDynamicMesh();
		void exportDynamicMesh();

private:
		FW::Mesh<FW::VertexPNC>* m_dynamicMesh;
		FW::MeshBase* m_staticMesh;
		FW::Mat4f m_projection;
		FW::Mat4f m_worldToCamera;
		FW::Mat4f m_meshScale;
        FW::GLContext* m_context;
		FW::CameraControls* m_camera;
        AssetManager* m_assetManager;
        FW::GLContext::Program* m_shader;
		float m_scale;
		bool m_drawAxis;

};