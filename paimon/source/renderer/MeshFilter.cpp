#include "MeshFilter.h"

#include <fstream>

using namespace Paimon;

MeshFilter::MeshFilter()
    : m_mesh(std::make_shared<Mesh>())
{

}

void MeshFilter::LoadMesh(const std::string &path)
{
    std::ifstream inputFileStream(path, std::ios::in | std::ios::binary);

    MeshFileHead meshFIleHead;
    inputFileStream.read((char *)&meshFIleHead, sizeof(MeshFileHead));

    m_mesh->vertices.resize(meshFIleHead.numVertex);
    m_mesh->indices.resize(meshFIleHead.numIndex);

    inputFileStream.read((char *)m_mesh->vertices.data(), meshFIleHead.numVertex * sizeof(Vertex));
    inputFileStream.read((char *)m_mesh->indices.data(), meshFIleHead.numIndex * sizeof(unsigned short));
}
