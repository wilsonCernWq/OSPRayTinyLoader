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

	class Mesh {
	private:
		std::string SplitPath(const std::string& str)
		{
			size_t i = str.find_last_of("/\\");
			if (i != std::string::npos) { return str.substr(0, i + 1); }
			else { return std::string(""); }
		}
	private:
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
		std::string dirpath; // directory path to the mesh folder
	public:
		std::vector<Geometry> geometries;
	public:
		/** Get Mesh Path */
		const char* DirPath() { return dirpath == "" ? nullptr : dirpath.c_str(); }

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
			geometries.resize(tiny.materials.size()); //! the lase element is the default geometry
			for (int i = 0; i < tiny.materials.size(); ++i) { geometries[i].mtl_index = 1; };
			for (size_t s = 0, index_offset = 0; s < tiny.shapes.size(); s++) { // Loop over shapes
				for (size_t f = 0; f < tiny.shapes[s].mesh.num_face_vertices.size(); f++) { // Loop over faces (polygon)
					//! per-face material
					int mtl_index = tiny.shapes[s].mesh.material_ids[f];
					int geo_index = mtl_index;
					int fv = tiny.shapes[s].mesh.num_face_vertices[f]; // number of vertices of this face
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
						if (idx.normal_index >= 0) {
							geometries[geo_index].has_normal = true;
							float nx = tiny.attributes.normals[3 * idx.normal_index + 0];
							float ny = tiny.attributes.normals[3 * idx.normal_index + 1];
							float nz = tiny.attributes.normals[3 * idx.normal_index + 2];
							geometries[geo_index].normal.push_back(nx);
							geometries[geo_index].normal.push_back(ny);
							geometries[geo_index].normal.push_back(nz);
						}
						if (idx.texcoord_index >= 0) {
							geometries[geo_index].has_texcoord = true;
							float tx = tiny.attributes.texcoords[2 * idx.texcoord_index + 0];
							float ty = tiny.attributes.texcoords[2 * idx.texcoord_index + 1];
							geometries[geo_index].texcoord.push_back(tx);
							geometries[geo_index].texcoord.push_back(ty);
						}
					}
					index_offset += fv;
				}
			}
			return true;
		}
	};
};

#endif//_OTV_MESH_WRAPPER_H_