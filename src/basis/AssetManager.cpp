#include "AssetManager.h"


void AssetManager::LoadAssets()
{
        m_meshes[MeshType_Pyramid] = importMesh("pyramid.obj");
		m_meshes[MeshType_Sphere] = importMesh("sphere.obj"); 
}

void AssetManager::ReleaseAssets()
{
        for (auto mesh : m_meshes)
                delete mesh;
}

FW::MeshBase* AssetManager::getMesh(MeshType meshType)
{
        if (meshType == MeshType_Count)
                return NULL;
        return m_meshes[meshType];
}

FW::MeshBase* AssetManager::importMesh(const std::string& fileName)
{
        std::string filePath = "assets/meshes/" + fileName;
        return FW::importMesh(filePath.c_str());
}