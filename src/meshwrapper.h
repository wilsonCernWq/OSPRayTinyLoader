#pragma once
#ifndef _MESH_WRAPPER_H_
#define _MESH_WRAPPER_H_

#include "common.h"
#include "helper.h"

namespace otv {

    //! one geometry contains a continious mesh plus one material index
    struct Geometry {
	std::vector<float> vertex;
	std::vector<float> normal;
	std::vector<float> texcoord;
	std::vector<unsigned int> index;
	unsigned int mtl_index = -1;
	unsigned int num_faces =  0;
	bool has_normal = false;
	bool has_texcoord = false;
    };
    struct OSPTextureData {
	ImageData map_Ks;
	ImageData map_Kd;
	ImageData map_Ns;
	ImageData map_d;
	ImageData map_Bump;
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
	    if (i != std::string::npos) { return str.substr(0, i + 1); }
	    else { return std::string(""); }
	}
    public:
	float bbox_min[3], bbox_max[3];
	std::string dirpath; // directory path to the mesh folder
	struct {
	    std::string                      err;
	    tinyobj::attrib_t                attributes;
	    std::vector<tinyobj::shape_t>    shapes;
	    std::vector<tinyobj::material_t> materials;
	    void clear() {
		err.clear();
		attributes.vertices.clear();
		attributes.normals.clear();
		attributes.texcoords.clear();
		shapes.clear();
		materials.clear();
	    }
	} tiny;
	std::vector<OSPTextureData> textures;
	std::vector<Geometry> geometries;
    public:
	/** \brief Accessors */
	const char* DirPath() { return dirpath == "" ? nullptr : dirpath.c_str(); }
	cyPoint3f GetBBoxMax() { return cyPoint3f(bbox_max[0], bbox_max[1], bbox_max[2]); }
	cyPoint3f GetBBoxMin() { return cyPoint3f(bbox_min[0], bbox_min[1], bbox_min[2]); }
	cyPoint3f GetMaterial(unsigned int i, std::string str);
	cyPoint3f GetCenter() 
	{
	    return 0.5 * (GetBBoxMax() + GetBBoxMin());
	}
	float GetDiagonalLength() 
	{
	    return (GetBBoxMax() - GetBBoxMin()).Length();
	}
	/** 
	 * \brief Overriding LoadFromFileObj function for TriMesh, force to triangulate
	 */
	bool LoadFromFileObj(const char* fname, bool loadMtl = true);
	/**
	 * \brief OSPRay helper
	 */
	void AddToModel(OSPModel& model, OSPRenderer& renderer);
    };
};

#endif//_MESH_WRAPPER_H_
