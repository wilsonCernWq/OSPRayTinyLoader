#pragma once
#ifndef _MESH_WRAPPER_H_
#define _MESH_WRAPPER_H_

#include "common/helper.h"

namespace otv { 
  //! one geometry contains a continious mesh plus one material index
  struct Geometry {
    std::vector<float> vertex;
    std::vector<float> normal;
    std::vector<float> texcoord;
    std::vector<unsigned int> index;
    int mtl_index = -1;
    int num_faces =  0;
    bool has_normal   = false;
    bool has_texcoord = false;
  };
  struct Material {
    vec3f Kd = vec3f(0.7f); // 0.8     diffuse color
    vec3f Ks = vec3f(0.3f); // black   specular color
    float Ns = 99.0f; // 10      shininess (Phong exponent), usually in [2–104]
    float d  = 1.0f;  // opaque  opacity
    ImageData map_Ks;
    ImageData map_Kd;
    ImageData map_Ns;
    ImageData map_d;
    ImageData map_Bump;
    void LoadMtl(const tinyobj::material_t& tinymtl, std::string& dirpath);
  };
  //! TODO: add support for more formats
  //! This structure is not very good for handling general mesh format
  //! I am thinking of adding a loader folder, which contains all
  //! importers handling various of formats. This requires modifications
  //! on this Mesh class
  class Mesh {
  private:
    std::string SplitPath(const std::string& str);
  public:
    struct TinyObjLoader
    {
      std::string                      err;
      tinyobj::attrib_t                attributes; // attributes
      std::vector<tinyobj::shape_t>    shapes; // shapes
      std::vector<tinyobj::material_t> materials; // materials
      void Clear();
    };
    TinyObjLoader tiny;
    bbox3f bbox; // mesh bounding box
    std::string dirpath; // directory path to the mesh folder
    std::vector<Material> materials;
    std::vector<Geometry> geometries;
  public:
    /** \brief Accessors */
    const char* DirPath()
    {
      return dirpath == "" ? nullptr : dirpath.c_str();
    }
    vec3f GetBBoxMax()
    {
      return bbox.upper;
    }
    vec3f GetBBoxMin()
    {
      return bbox.lower;
    }
    vec3f GetCenter() 
    {
      return 0.5f * (GetBBoxMax() + GetBBoxMin());
    }
    float GetDiagonalLength() 
    {
      return glm::length(GetBBoxMax() - GetBBoxMin());
    }
    /** 
     * \brief Overriding LoadFromFileObj function for TriMesh,
     *  force to triangulate
     */
    bool LoadFromFileObj(const char* fname, bool loadMtl = true);
    /**
     * \brief OSPRay helper     
     */
    void AddToModel(OSPModel& model, OSPRenderer& renderer);
  };
};

#endif//_MESH_WRAPPER_H_
