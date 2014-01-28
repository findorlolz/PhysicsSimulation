#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "3d/Mesh.hpp"
#include "gui/Image.hpp"


enum MeshType
{
        MeshType_Pyramid,
		MeshType_Sphere,
		MeshType_Axis,

        MeshType_Count
};

class AssetManager
{
public:
        AssetManager() {}
        ~AssetManager() {}

        void LoadAssets();
        void ReleaseAssets();

        FW::MeshBase* getMesh(MeshType meshType);

private:
        FW::MeshBase* importMesh(const std::string& fileName);

private:
        FW::MeshBase* m_meshes[MeshType_Count];

};