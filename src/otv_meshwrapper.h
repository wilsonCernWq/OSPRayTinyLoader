#pragma once
#ifndef _OTV_MESH_WRAPPER_H_
#define _OTV_MESH_WRAPPER_H_

#include "otv_common.h"
#include "otv_helper.h"

namespace otv {

	class MeshWrapper : public cy::TriMesh {
	private:
		// reference http://stackoverflow.com/questions/3071665/getting-a-directory-name-from-a-filename
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
		struct {
			std::vector<cy::Point3f> vtptr;
			std::vector<cy::Point3f> nmptr;
			std::vector<cy::Point3f> txptr;
			unsigned int face_num;
		} arraydata;
		struct {
			std::vector<cy::Point3f> nmptr;
			std::vector<cy::Point3f> txptr;
			std::vector<int> nmcount, txcount;
		} indexdata;
		struct {
			tinyobj::attrib_t attributes;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
		} tiny;
		std::vector<std::vector<unsigned int>>mtlData;
		std::string dirpath; // directory path to the mesh folder
	public:

		/** Constructor */
		MeshWrapper() : TriMesh() {}

		/** Get Mesh Path */
		std::string DirPath() { return dirpath; }

		/** 
		 * Overriding LoadFromFileObj function for TriMesh, force to triangulate
		 */
		bool LoadFromFileObj(const char* fname, bool loadMtl = true) {
			std::cout << "Using TinyObjLoader" << std::endl;
			std::string err;
			dirpath = SplitPath(fname);
			bool succeed = tinyobj::LoadObj(
				&(tiny.attributes),
				&(tiny.shapes),
				&(tiny.materials),
				&err,
				fname,
				dirpath == "" ? nullptr : dirpath.c_str(),
				true);
			if (!err.empty()) { std::cerr << err << std::endl; }
			if (!succeed) { return false; }
			else {
				// retrieve information from TinyObjLoader to TriMesh
				std::vector<cy::Point3f> _vx_;
				std::vector<cy::TriMesh::TriFace> _fx_;
				std::vector<cy::Point3f> _vn_;
				std::vector<cy::TriMesh::TriFace> _fn_;
				std::vector<cy::Point3f> _vt_;
				std::vector<cy::TriMesh::TriFace> _ft_;
				mtlData = std::vector<std::vector<unsigned int>>(tiny.materials.size() + 1, std::vector<unsigned int>());
				// -- reference https://github.com/syoyo/tinyobjloader
				TriMesh::v = new cy::Point3f[tiny.attributes.vertices.size() / 3];
				TriMesh::vn = new cy::Point3f[tiny.attributes.normals.size() / 3];
				TriMesh::vt = new cy::Point3f[tiny.attributes.texcoords.size() / 2];
				if (tiny.attributes.vertices.size() <= 0) { return false; }
				bool has_normals = (tiny.attributes.normals.size() > 0);
				bool has_texcoords = (tiny.attributes.texcoords.size() > 0);
				for (size_t s = 0; s < tiny.shapes.size(); s++) { // Loop over shapes (each shape means a continious mesh)
					size_t index_offset = 0;
					for (size_t f = 0; f < tiny.shapes[s].mesh.num_face_vertices.size(); f++) { // Loop over faces (polygon)
						int fv = tiny.shapes[s].mesh.num_face_vertices[f]; // number of vertices of this face
						if (fv != 3) { break; return false; }
						cy::TriMesh::TriFace _fx_idx_, _fn_idx_, _ft_idx_;
						for (size_t v = 0; v < fv; v++) { // Loop over vertices in the face.
							tinyobj::index_t idx = tiny.shapes[s].mesh.indices[index_offset + v];
							float vx = tiny.attributes.vertices[3 * idx.vertex_index + 0];
							float vy = tiny.attributes.vertices[3 * idx.vertex_index + 1];
							float vz = tiny.attributes.vertices[3 * idx.vertex_index + 2];
							_vx_.emplace_back(vx, vy, vz);
							_fx_idx_.v[v] = _vx_.size() - 1;
							if (has_normals) {
								float nx = tiny.attributes.normals[3 * idx.normal_index + 0];
								float ny = tiny.attributes.normals[3 * idx.normal_index + 1];
								float nz = tiny.attributes.normals[3 * idx.normal_index + 2];
								_vn_.emplace_back(nx, ny, nz);
								_fn_idx_.v[v] = _vn_.size() - 1;
							}
							if (has_texcoords) {
								if (idx.texcoord_index >= 0) {
									float tx = tiny.attributes.texcoords[2 * idx.texcoord_index + 0];
									float ty = tiny.attributes.texcoords[2 * idx.texcoord_index + 1];
									_vt_.emplace_back(tx, ty, 1.0f);
								}
								else {
									_vt_.emplace_back(0.0f, 0.0f, -1.0f); // use the last coordinate to indicate valid texture
								}
								_ft_idx_.v[v] = _vt_.size() - 1;
							}
						}
						_fx_.push_back(_fx_idx_);
						if (has_normals) { _fn_.push_back(_fn_idx_); }
						if (has_texcoords) { _ft_.push_back(_ft_idx_); }
						index_offset += fv;
						// per-face material
						int mtl_index = tiny.shapes[s].mesh.material_ids[f];
						if (mtl_index >= 0) {
							mtlData[mtl_index].push_back(_fx_.size() - 1);
						}
						else {
							mtlData.back().push_back(_fx_.size() - 1);
						}
					}
				}
				//
				// set attribute sizes
				SetNumFaces((unsigned int)_fx_.size());
				SetNumVertex((unsigned int)_vx_.size());
				SetNumNormals((unsigned int)_vn_.size());
				SetNumTexVerts((unsigned int)_vt_.size());
				if (loadMtl) SetNumMtls((unsigned int)mtlData.size());
				//
				// copy attribute data
				memcpy(v, _vx_.data(), sizeof(cy::Point3f) * _vx_.size());
				if (_vt_.size() > 0) memcpy(vt, _vt_.data(), sizeof(cy::Point3f)*_vt_.size());
				if (_vn_.size() > 0) memcpy(vn, _vn_.data(), sizeof(cy::Point3f)*_vn_.size());
				//
				// compute mcfc
				if (loadMtl && mtlData.size() > 0) {
					unsigned int fid = 0;
					for (int m = 0; m < (int)mtlData.size(); m++) {
						for (auto i = mtlData[m].begin(); i != mtlData[m].end(); i++) {
							f[fid] = _fx_[*i];
							if (fn) fn[fid] = _fn_[*i];
							if (ft) ft[fid] = _ft_[*i];
							fid++;
						}
						mcfc[m] = fid;
					}
				}
				else {
					memcpy(f, _fx_.data(), sizeof(TriFace) * _fx_.size());
					if (ft) memcpy(ft, _ft_.data(), sizeof(TriFace) * _ft_.size());
					if (fn) memcpy(fn, _fn_.data(), sizeof(TriFace) * _fn_.size());
				}
				//
				// Load the .mtl files
				if (loadMtl) { // use default value for the last material
					for (int m = 0; m < (int)mtlData.size() - 1; m++)
					{
						// constant integers
						TriMesh::m[m].illum = tiny.materials[m].illum;
						TriMesh::m[m].Ns = tiny.materials[m].shininess;
						TriMesh::m[m].Ni = tiny.materials[m].ior;
						// copy vectors
						memcpy(TriMesh::m[m].Ka, tiny.materials[m].ambient, sizeof(float) * 3);
						memcpy(TriMesh::m[m].Kd, tiny.materials[m].diffuse, sizeof(float) * 3);
						memcpy(TriMesh::m[m].Ks, tiny.materials[m].specular, sizeof(float) * 3);
						memcpy(TriMesh::m[m].Tf, tiny.materials[m].transmittance, sizeof(float) * 3);
						// copy texture filename
						helper::copychar(TriMesh::m[m].name.data, tiny.materials[m].name);
						helper::copychar(TriMesh::m[m].map_d.data, tiny.materials[m].alpha_texname);
						helper::copychar(TriMesh::m[m].map_Ka.data, tiny.materials[m].ambient_texname);
						helper::copychar(TriMesh::m[m].map_Kd.data, tiny.materials[m].diffuse_texname);
						helper::copychar(TriMesh::m[m].map_Ks.data, tiny.materials[m].specular_texname);
						helper::copychar(TriMesh::m[m].map_Ns.data, tiny.materials[m].specular_highlight_texname);
						helper::copychar(TriMesh::m[m].map_bump.data, tiny.materials[m].bump_texname);
						helper::copychar(TriMesh::m[m].map_disp.data, tiny.materials[m].displacement_texname);
					}
				}
				return true;
			}
		}

		void ComputeArrayData() {
			for (auto i = 0; i < NF(); ++i) {
				auto vIdx = F(i);
				auto nIdx = HasNormals()         ? FN(i) : cy::TriMesh::TriFace();
				auto tIdx = HasTextureVertices() ? FT(i) : cy::TriMesh::TriFace();
				for (auto j = 0; j < 3; ++j) {
					auto v = vIdx.v[j];
					auto n = HasNormals()         ? nIdx.v[j] : -1;
					auto t = HasTextureVertices() ? tIdx.v[j] : -1;
					arraydata.vtptr.push_back(V(v));
					if (HasNormals()) { arraydata.nmptr.push_back(VN(n)); }
					if (HasTextureVertices()) { arraydata.txptr.push_back(VT(t)); }
				}
			}
		}

		void ComputeIndexData() {
			indexdata.nmcount = std::vector<int>(NV(), 0);
			indexdata.txcount = std::vector<int>(NV(), 0);
			for (auto i = 0; i < NF(); ++i) {
				auto vIdx = F(i);
				auto nIdx = HasNormals()         ? FN(i) : cy::TriMesh::TriFace();
				auto tIdx = HasTextureVertices() ? FT(i) : cy::TriMesh::TriFace();
				for (auto j = 0; j < 3; ++j) {
					auto v = vIdx.v[j];
					auto n = HasNormals()         ? nIdx.v[j] : -1;
					auto t = HasTextureVertices() ? tIdx.v[j] : -1;
					// compute for normal
					if (HasNormals()) {
						if (indexdata.nmcount[v] == 0) {
							indexdata.nmptr.push_back(VN(n));
						}
						else {
							indexdata.nmptr[v] =
								(indexdata.nmptr[v] * indexdata.nmcount[v] + VN(n)) / (indexdata.nmcount[v] + 1);
						}
						++indexdata.nmcount[v];
					}
					// compute for texture coordinate
					if (HasTextureVertices()) {
						if (indexdata.txcount[v] == 0) {
							indexdata.txptr.push_back(VT(n));
						}
						else {
							indexdata.txptr[v] =
								(indexdata.txptr[v] * indexdata.txcount[v] + VT(n)) / (indexdata.txcount[v] + 1);
						}
						++indexdata.txcount[v];
					}
				}
			}
			indexdata.nmcount.clear();
			indexdata.txcount.clear();
		}

		void GetArrayData(void* *vptr, void* *nptr, void* *tptr, void* *iptr, unsigned int *vsz, unsigned int *nsz, unsigned int *tsz,unsigned int *isz, unsigned int *num) {
			*num = NF() * 3;
			*vptr = arraydata.vtptr.data();
			*vsz = sizeof(cy::Point3f) * NF() * 3;
			*nptr = HasNormals() ? arraydata.nmptr.data() : nullptr;
			*nsz =  HasNormals() ? sizeof(cy::Point3f) * NF() * 3 : 0;
			*tptr = HasTextureVertices() ? arraydata.txptr.data() : nullptr;
			*tsz =  HasTextureVertices() ? sizeof(cy::Point3f) * NF() * 3 : 0;
			*iptr = NULL;
			*isz = 0;
		}

		void GetIndexData(void* *vptr, void* *nptr, void* *tptr, void* *iptr, unsigned int *vsz, unsigned int *nsz, unsigned int *tsz, unsigned int *isz, unsigned int *num) {
			*num = NV();
			*vptr = &(V(0));
			*vsz = sizeof(cy::Point3f) * NV();
			*nptr = HasNormals() ? indexdata.nmptr.data() : nullptr;
			*nsz = HasNormals() ? sizeof(cy::Point3f) * NV() : 0;
			*tptr = HasTextureVertices() ? indexdata.txptr.data() : nullptr;
			*tsz = HasTextureVertices() ? sizeof(cy::Point3f) * NV() : 0;
			*iptr = &(F(0));
			*isz = sizeof(cy::TriMesh::TriFace) * NF();
		}

		void GetBufferVertex(void* &vptr, unsigned int &vsz, bool use_element_draw = false) {
			if (use_element_draw) {
				vptr = &(V(0));
				vsz = sizeof(cy::Point3f) * NV();
			}
			else {
				vptr = arraydata.vtptr.data();
				vsz = sizeof(cy::Point3f) * NF() * 3;
			}
		}

		cy::Point3f* GetBufferVertexPtr(bool use_element_draw = false) {
			return use_element_draw ? &(V(0)) : arraydata.vtptr.data();
		}

		unsigned int GetBufferVertexSize(bool use_element_draw = false) {
			return use_element_draw ? sizeof(cy::Point3f) * NV() : sizeof(cy::Point3f) * NF() * 3;
		}

		void GetBufferNormal(void* &nptr, unsigned int &nsz, bool use_element_draw = false) {
			if (use_element_draw) {
				nptr = HasNormals() ? indexdata.nmptr.data() : nullptr;
				nsz = HasNormals() ? sizeof(cy::Point3f) * NV() : 0;
			}
			else {
				nptr = HasNormals() ? arraydata.nmptr.data() : nullptr;
				nsz = HasNormals() ? sizeof(cy::Point3f) * NF() * 3 : 0;
			}
		}

		cy::Point3f* GetBufferNormalPtr(bool use_element_draw = false) {
			return use_element_draw ? (HasNormals() ? indexdata.nmptr.data() : nullptr) : (HasNormals() ? arraydata.nmptr.data() : nullptr);
		}

		unsigned int GetBufferNormalSize(bool use_element_draw = false) {
			return use_element_draw ? (HasNormals() ? sizeof(cy::Point3f) * NV() : 0) : (HasNormals() ? sizeof(cy::Point3f) * NF() * 3 : 0);
		}

		void GetBufferTexcoord(void* &tptr, unsigned int &tsz, bool use_element_draw = false) {
			if (use_element_draw) {
				tptr = HasTextureVertices() ? indexdata.txptr.data() : nullptr;
				tsz = HasTextureVertices() ? sizeof(cy::Point3f) * NV() : 0;
			}
			else {
				tptr = HasTextureVertices() ? arraydata.txptr.data() : nullptr;
				tsz = HasTextureVertices() ? sizeof(cy::Point3f) * NF() * 3 : 0;
			}
		}

		cy::Point3f* GetBufferTexcoordPtr(bool use_element_draw = false) {
			return use_element_draw ? (HasTextureVertices() ? indexdata.txptr.data() : nullptr) : (HasTextureVertices() ? arraydata.txptr.data() : nullptr);
		}

		unsigned int GetBufferTexcoordSize(bool use_element_draw = false) {
			return use_element_draw ? (HasTextureVertices() ? sizeof(cy::Point3f) * NV() : 0) : (HasTextureVertices() ? sizeof(cy::Point3f) * NF() * 3 : 0);
		}

		void GetBufferIndex(void* &iptr, unsigned int &isz, unsigned int &num, bool use_element_draw = false) {
			if (use_element_draw) {
				num = NV();
				iptr = &(F(0));
				isz = sizeof(cy::TriMesh::TriFace) * NF();
			}
			else {
				num = NF() * 3;
				iptr = NULL;
				isz = 0;
			}
		}
	};
};

#endif//_OTV_MESH_WRAPPER_H_