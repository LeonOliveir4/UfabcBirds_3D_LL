#include "modelObj.hpp"

#include <iostream>
#include <filesystem>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    auto const h2{std::hash<glm::vec3>()(vertex.normal)};
    auto const h3{std::hash<glm::vec2>()(vertex.texCoord)};
    return abcg::hashCombine(h1, h2, h3);
  }
};

void ModelObj::loadDiffuseTexture(std::string_view path) {
  if (!std::filesystem::exists(path))
    return;

  abcg::glDeleteTextures(1, &m_diffuseTexture);
  m_diffuseTexture = abcg::loadOpenGLTexture({.path = path});
}

void ModelObj::loadObj() {
  auto const basePath{std::filesystem::path{m_path}.parent_path().string() + "/"};

  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = basePath; // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(m_path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", m_path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", m_path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attrib{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};
  auto const &materials{reader.GetMaterials()};

  m_vertices.clear();
  m_indices.clear();

  m_hasNormals = false;
  m_hasTexCoords = false;

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Position
      auto const startIndex{3 * index.vertex_index};
      glm::vec3 position{attrib.vertices.at(startIndex + 0),
                         attrib.vertices.at(startIndex + 1),
                         attrib.vertices.at(startIndex + 2)};

      // Normal
      glm::vec3 normal{};
      if (index.normal_index >= 0) {
        m_hasNormals = true;
        auto const normalStartIndex{3 * index.normal_index};
        normal = {attrib.normals.at(normalStartIndex + 0),
                  attrib.normals.at(normalStartIndex + 1),
                  attrib.normals.at(normalStartIndex + 2)};
      }

      // Texture coordinates
      glm::vec2 texCoord{};
      if (index.texcoord_index >= 0) {
        m_materialMode = 1; //Usa textura UV
        m_hasTexCoords = true;
        auto const texCoordsStartIndex{2 * index.texcoord_index};
        texCoord = {attrib.texcoords.at(texCoordsStartIndex + 0),
                    attrib.texcoords.at(texCoordsStartIndex + 1)};
      }

      Vertex const vertex{
          .position = position, .normal = normal, .texCoord = texCoord};

      // If hash doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }

  // Use properties of first material, if available
  if (!materials.empty()) {
    auto const &mat{materials.at(0)}; // First material
    m_material.m_Ka = {mat.ambient[0], mat.ambient[1], mat.ambient[2], 1};
    m_material.m_Kd = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1};
    m_material.m_Ks = {mat.specular[0], mat.specular[1], mat.specular[2], 1};
    m_material.m_shininess = mat.shininess;

    if (!mat.diffuse_texname.empty()){
      std::cout<<"Tem textura\n";
      std::cout<<basePath + mat.diffuse_texname<<"\n";
      loadDiffuseTexture(basePath + mat.diffuse_texname);
    } else {
      std::cout<<"nao tem, textura\n";
    }
  } else {
    // Default values
    m_material.m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    m_material.m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    m_material.m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    m_material.m_shininess = 25.0f;
  }

  if (!m_hasNormals) {
    computeNormals();
  }

  createBuffers();
}

void ModelObj::create(GLuint program,std::string_view path, bool standardize){
  m_program = program;
  m_path = path;
  m_standardize = standardize;
  loadObj();
  createBuffers();

}
