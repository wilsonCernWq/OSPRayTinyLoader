#pragma once
#ifndef _OTV_MESH_WRAPPER_H_
#define _OTV_MESH_WRAPPER_H_

#include "otv_common.h"
#include "otv_helper.h"

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
	/** Accessors */
	const char* DirPath() { return dirpath == "" ? nullptr : dirpath.c_str(); }
	cyPoint3f GetBBoxMax() { return cyPoint3f(bbox_max[0], bbox_max[1], bbox_max[2]); }
	cyPoint3f GetBBoxMin() { return cyPoint3f(bbox_min[0], bbox_min[1], bbox_min[2]); }
	cyPoint3f GetMaterial(unsigned int i, std::string str) {
	    if (str.compare("Ka") == 0) {
		return cyPoint3f(tiny.materials[i].ambient);
	    } 
	    else if (str.compare("Kd") == 0) {
		return cyPoint3f(tiny.materials[i].diffuse);
	    }
	    else if (str.compare("Ks") == 0) {
		return cyPoint3f(tiny.materials[i].specular);
	    }
	    else if (str.compare("Tf") == 0) {
		return cyPoint3f(tiny.materials[i].transmittance);
	    }
	}
	/** 
	 * Overriding LoadFromFileObj function for TriMesh, force to triangulate
	 */
	bool LoadFromFileObj(const char* fname, bool loadMtl = true)
	{
	    //! load mesh from file
	    dirpath = SplitPath(fname);
	    tiny.clear();
	    bool succeed = tinyobj::LoadObj(&(tiny.attributes), &(tiny.shapes), &(tiny.materials), &(tiny.err), fname, DirPath(), true);
	    if (!tiny.err.empty()) { std::cerr << tiny.err << std::endl; }
	    if (!succeed) { return false; }
	    //! parse loaded mesh
	    bbox_max[0] = std::numeric_limits<int>::min();
	    bbox_max[1] = std::numeric_limits<int>::min();
	    bbox_max[2] = std::numeric_limits<int>::min();
	    bbox_min[0] = std::numeric_limits<int>::max();
	    bbox_min[1] = std::numeric_limits<int>::max();
	    bbox_min[2] = std::numeric_limits<int>::max();
	    geometries.resize(tiny.materials.size()); //! the lase element is the default geometry
	    for (int i = 0; i < tiny.materials.size(); ++i) { geometries[i].mtl_index = 1; };
	    for (size_t s = 0; s < tiny.shapes.size(); s++) { // Loop over shapes
		size_t index_offset = 0;
		for (size_t f = 0; f < tiny.shapes[s].mesh.num_face_vertices.size(); f++) { // Loop over faces (polygon)
		    //! per-face material
		    int mtl_index = tiny.shapes[s].mesh.material_ids[f];
		    int geo_index = mtl_index;
		    if (geo_index < 0) { 
			std::cerr << "wrong index" << std::endl; exit(EXIT_FAILURE); 
		    }
		    int fv = tiny.shapes[s].mesh.num_face_vertices[f]; // number of vertices of this face
		    if (fv != 3) { 
			std::cerr << "non triangle" << std::endl; exit(EXIT_FAILURE); 
		    }
		    geometries[geo_index].num_faces++;
		    for (size_t v = 0; v < fv; v++) { // Loop over vertices in the face.
			tinyobj::index_t idx = tiny.shapes[s].mesh.indices[index_offset + v];
			geometries[geo_index].index.push_back(geometries[geo_index].index.size());
			float vx = tiny.attributes.vertices[3 * idx.vertex_index + 0];
			float vy = tiny.attributes.vertices[3 * idx.vertex_index + 1];
			float vz = tiny.attributes.vertices[3 * idx.vertex_index + 2];
			geometries[geo_index].vertex.push_back(vx);
			geometries[geo_index].vertex.push_back(vy);
			geometries[geo_index].vertex.push_back(vz);
			bbox_max[0] = vx > bbox_max[0] ? vx : bbox_max[0];
			bbox_max[1] = vy > bbox_max[1] ? vy : bbox_max[1];
			bbox_max[2] = vz > bbox_max[2] ? vz : bbox_max[2];
			bbox_min[0] = vx < bbox_min[0] ? vx : bbox_min[0];
			bbox_min[1] = vy < bbox_min[1] ? vy : bbox_min[1];
			bbox_min[2] = vz < bbox_min[2] ? vz : bbox_min[2];
			if (idx.normal_index >= 0) {
			    geometries[geo_index].has_normal = true;
			    float nx = tiny.attributes.normals[3 * idx.normal_index + 0];
			    float ny = tiny.attributes.normals[3 * idx.normal_index + 1];
			    float nz = tiny.attributes.normals[3 * idx.normal_index + 2];
			    geometries[geo_index].normal.push_back(nx);
			    geometries[geo_index].normal.push_back(ny);
			    geometries[geo_index].normal.push_back(nz);
			} 
			else {
			    std::cerr << "wrong normal" << std::endl; exit(EXIT_FAILURE);
			}
			if (idx.texcoord_index >= 0) {
			    geometries[geo_index].has_texcoord = true;
			    float tx = tiny.attributes.texcoords[2 * idx.texcoord_index + 0];
			    float ty = tiny.attributes.texcoords[2 * idx.texcoord_index + 1];
			    geometries[geo_index].texcoord.push_back(tx);
			    geometries[geo_index].texcoord.push_back(ty);
			}
			else {
			    std::cerr << "wrong texcoord" << std::endl; exit(EXIT_FAILURE);
			}
		    }
		    index_offset += fv;
		}
	    }
	    //! load images
	    for (int i = 0; i < tiny.materials.size(); ++i) {
		textures.emplace_back();
		loadimg(textures[i].map_Kd, tiny.materials[i].diffuse_texname, dirpath);
		loadimg(textures[i].map_Ks, tiny.materials[i].specular_texname, dirpath);
		loadimg(textures[i].map_Ns, tiny.materials[i].specular_highlight_texname, dirpath);
		loadimg(textures[i].map_d, tiny.materials[i].alpha_texname, dirpath);
		loadimg(textures[i].map_Bump, tiny.materials[i].bump_texname, dirpath);
	    }
	    return true;
	}
    };
};

#endif//_OTV_MESH_WRAPPER_H_
