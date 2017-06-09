#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "meshwrapper.h"

cyPoint3f otv::Mesh::GetMaterial(unsigned int i, std::string str) 
{
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

bool otv::Mesh::LoadFromFileObj(const char* fname, bool loadMtl)
{
    //! load mesh from file
    dirpath = SplitPath(fname);
    tiny.clear();
    bool succeed = tinyobj::LoadObj(&(tiny.attributes), 
				    &(tiny.shapes), 
				    &(tiny.materials), 
				    &(tiny.err), 
				    fname, DirPath(), true);
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
	for (size_t f = 0; f < tiny.shapes[s].mesh.num_face_vertices.size(); f++) { 
            // Loop over faces (polygon)
	    //! per-face material
	    int mtl_index = tiny.shapes[s].mesh.material_ids[f];
	    int geo_index = mtl_index;
	    if (geo_index < 0) { 
		std::cerr << "wrong index" << std::endl; exit(EXIT_FAILURE); 
	    }
	    // number of vertices of this face
	    int fv = tiny.shapes[s].mesh.num_face_vertices[f]; 
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

void otv::Mesh::AddToModel(OSPModel& model, OSPRenderer& renderer) 
{
    for (int i = 0; i < this->geometries.size(); ++i) {
	if (this->geometries[i].num_faces != 0) {
	    OSPGeometry gdata = ospNewGeometry("triangles");
	    //! vertex
	    OSPData vdata =
		ospNewData(this->geometries[i].vertex.size() / 3, 
			   OSP_FLOAT3, this->geometries[i].vertex.data(), 
			   OSP_DATA_SHARED_BUFFER);
	    ospCommit(vdata);
	    ospSetObject(gdata, "vertex", vdata);
	    //! index
	    OSPData idata = 
		ospNewData(this->geometries[i].index.size() / 3, 
			   OSP_INT3, this->geometries[i].index.data(), 
			   OSP_DATA_SHARED_BUFFER);
	    ospCommit(idata);
	    ospSetObject(gdata, "index", idata);
	    //! normal
	    if (this->geometries[i].has_normal) {
		OSPData ndata =
		    ospNewData(this->geometries[i].normal.size() / 3, 
			       OSP_FLOAT3, this->geometries[i].normal.data(), 
			       OSP_DATA_SHARED_BUFFER);
		ospCommit(ndata);
		ospSetObject(gdata, "vertex.normal", ndata);
	    }
	    //! texture coordinate
	    if (this->geometries[i].has_texcoord) {
		OSPData tdata = 
		    ospNewData(this->geometries[i].texcoord.size() / 2, 
			       OSP_FLOAT2, this->geometries[i].texcoord.data(), 
			       OSP_DATA_SHARED_BUFFER);
		ospCommit(tdata);
		ospSetObject(gdata, "vertex.texcoord", tdata);
	    }
	    //! material
	    OSPMaterial mtl_data = ospNewMaterial(renderer, "OBJMaterial");
	    ospcommon::vec3f mtl_Tf = otv::make_vec(this->GetMaterial(i, "Tf"));
	    ospcommon::vec3f mtl_Kd = otv::make_vec(this->GetMaterial(i, "Kd"));
	    ospcommon::vec3f mtl_Ks = otv::make_vec(this->GetMaterial(i, "Ks"));
	    ospSetVec3f(mtl_data, "Tf", (osp::vec3f&)mtl_Tf);
	    ospSetVec3f(mtl_data, "Kd", (osp::vec3f&)mtl_Kd);
	    ospSetVec3f(mtl_data, "Ks", (osp::vec3f&)mtl_Ks);
	    ospSet1f(mtl_data, "Ns", this->tiny.materials[i].shininess);
	    ospSet1f(mtl_data, "d",  this->tiny.materials[i].dissolve);	    
	    if (!this->textures[i].map_Kd.IsEmpty()) {
	    	auto tex_dim = otv::make_vec(this->textures[i].map_Kd.Size());
	    	OSPTexture2D map_Kd = 
	    	    ospNewTexture2D((osp::vec2i&)tex_dim, 
	    			    OSP_TEXTURE_RGBA8, 
	    			    this->textures[i].map_Kd.data.data(), 1);
	    	ospSetObject(mtl_data, "map_Kd", map_Kd);
	    }
	    if (!this->textures[i].map_Ks.IsEmpty()) {
	    	auto tex_dim = otv::make_vec(this->textures[i].map_Ks.Size());
	    	OSPTexture2D map_Ks = 
	    	    ospNewTexture2D((osp::vec2i&)tex_dim, 
	    			    OSP_TEXTURE_RGBA8, 
	    			    this->textures[i].map_Ks.data.data(), 1);
	    	ospSetObject(mtl_data, "map_Ks", map_Ks);
	    }
	    if (!this->textures[i].map_Ns.IsEmpty()) {
	    	auto tex_dim = otv::make_vec(this->textures[i].map_Ns.Size());
	    	OSPTexture2D map_Ns = 
	    	    ospNewTexture2D((osp::vec2i&)tex_dim, 
	    			    OSP_TEXTURE_RGBA8, 
	    			    this->textures[i].map_Ns.data.data(), 1);
	    	ospSetObject(mtl_data, "map_Ns", map_Ns);
	    }
	    if (!this->textures[i].map_d.IsEmpty()) {
	    	auto tex_dim = otv::make_vec(this->textures[i].map_d.Size());
	    	OSPTexture2D map_d = 
	    	    ospNewTexture2D((osp::vec2i&)tex_dim, 
	    			    OSP_TEXTURE_RGBA8, 
	    			    this->textures[i].map_d.data.data(), 1);
	    	ospSetObject(mtl_data, "map_d", map_d);
	    }
	    if (!this->textures[i].map_Bump.IsEmpty()) {
	    	auto tex_dim = otv::make_vec(this->textures[i].map_Bump.Size());
	    	OSPTexture2D map_Bump =
	    	    ospNewTexture2D((osp::vec2i&)tex_dim, 
	    			    OSP_TEXTURE_RGBA8, 
	    			    this->textures[i].map_Bump.data.data(), 1);
	    	ospSetObject(mtl_data, "map_Bump", map_Bump);
	    }
	    ospCommit(mtl_data);
	    ospSetMaterial(gdata, mtl_data);
	    //! commit geometry
	    ospCommit(gdata);
	    ospAddGeometry(model, gdata);
	}
    }
}
