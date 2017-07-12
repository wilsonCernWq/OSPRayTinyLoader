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
    unsigned int mtl_index = -1;
    unsigned int num_faces =  0;
    bool has_normal   = false;
    bool has_texcoord = false;
  };
  struct Material {
    vec3f Kd; // 0.8     diffuse color
    vec3f Ks; // black   specular color
    float Ns; // 10      shininess (Phong exponent), usually in [2–104]
    float d;  // opaque  opacity
    ImageData map_Ks;
    ImageData map_Kd;
    ImageData map_Ns;
    ImageData map_d;
    ImageData map_Bump;
    void LoadMtl(const tinyobj::material_t& tinymtl, std::string& dirpath)
    {
      // load constants
      Kd = vec3f(tinymtl.diffuse[0],
		 tinymtl.diffuse[1],
		 tinymtl.diffuse[2]); // default 0.f
      Ks = vec3f(tinymtl.specular[0],
		 tinymtl.specular[1],
		 tinymtl.specular[2]);
      Ns = tinymtl.shininess;
      d  = tinymtl.dissolve;
      // load textures
      loadimg(map_Kd, tinymtl.diffuse_texname, dirpath);
      loadimg(map_Ks, tinymtl.specular_texname, dirpath);
      loadimg(map_Ns, tinymtl.specular_highlight_texname, dirpath);
      loadimg(map_d, tinymtl.alpha_texname, dirpath);
      loadimg(map_Bump, tinymtl.bump_texname, dirpath);
    }

  };
  //! TODO: add support for more formats
  //! This structure is not very good for handling general mesh format
  //! I am thinking of adding a loader folder, which contains all
  //! importers handling various of formats. This requires modifications
  //! on this Mesh class
  class Mesh {
  private:
    std::string SplitPath(const std::string& str)
      {
	size_t i = str.find_last_of("/\\");
	if (i != std::string::npos) {
	  return str.substr(0, i + 1);
	}
	else {
	  return std::string("");
	}
      }
  public:
    struct TinyObjLoader
    {
      std::string                      err;
      tinyobj::attrib_t                attributes; // attributes
      std::vector<tinyobj::shape_t>    shapes; // shapes
      std::vector<tinyobj::material_t> materials; // materials
      void Clear() {
	err.clear();
	attributes.vertices.clear();
	attributes.normals.clear();
	attributes.texcoords.clear();
	shapes.clear();
	materials.clear();
      }
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
    // cyPoint3f GetMaterial(unsigned int i, std::string str);
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
