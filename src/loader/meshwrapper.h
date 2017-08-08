#pragma once
#ifndef _MESH_WRAPPER_H_
#define _MESH_WRAPPER_H_

#include "common/helper.h"

// trying this obj loader https://github.com/syoyo/tinyobjloader
#include "tiny_obj_loader.h"

namespace otv {
  /** \brief structure for a triangular mesh */
  //! TODO: add support for more formats
  //! This structure is not very good for handling general mesh format
  //! I am thinking of adding a loader folder, which contains all
  //! importers handling various of formats. This requires modifications
  //! on this Mesh class
  class Mesh {
  private:
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
      float Ns = 99.0f; // 10 shininess (Phong exponent), usually in [2–104]
      float d  = 1.0f;  // opaque  opacity
      ImageData map_Ks;
      ImageData map_Kd;
      ImageData map_Ns;
      ImageData map_d;
      ImageData map_Bump;
      void LoadMtl(const tinyobj::material_t& tinymtl, std::string& dirpath);
    };
    struct TinyObjLoader
    {
      std::string                      err;
      tinyobj::attrib_t                attributes; // attributes
      std::vector<tinyobj::shape_t>    shapes; // shapes
      std::vector<tinyobj::material_t> materials; // materials
      void Clear();
    };
    TinyObjLoader tiny;
    /* geometric data */
    vec3f center; // mesh center coordinate in world
    bbox3f bbox;  // mesh bounding box in world
    affine3f transform;
    /* meta data */
    std::string dpath; // directory path to the mesh folder
    std::string fpath; // directory path to the mesh folder
    std::string fname; // filename of the mesh
    std::vector<Material> materials;
    std::vector<Geometry> geometries;
  private:
    void ComputePath(const std::string& str);
  public:
    /** \brief Accessors */
    std::string GetFullPath()
    {
      return fpath;
    }
    vec3f GetBBoxMax()
    {
      return bbox.upper - center;
    }
    vec3f GetBBoxMin()
    {
      return bbox.lower - center;
    }
    vec3f GetCenter() 
    {
      center = 0.5f * (GetBBoxMax() + GetBBoxMin());
      return center;
    }
    float GetDiagonalLength() 
    {
      return glm::length(GetBBoxMax() - GetBBoxMin());
    }
    affine3f GetTransform()
    {
      return transform;
    }
    void SetTransform(const otv::mat4f&);
    void SetTransform();
    /** 
     * \brief Overriding LoadFromFileObj function for TriMesh,
     *  force to triangulate
     */
    bool LoadFromFileObj(const char* filename, bool loadMtl = true);
    /**
     * \brief OSPRay helper     
     */
    void AddToModel(OSPModel& model, OSPRenderer& renderer);
  };  
};

#endif//_MESH_WRAPPER_H_
