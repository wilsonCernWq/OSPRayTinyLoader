#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "meshwrapper.h"
#include <limits>

std::string ParsePath(const std::string& str)
{
  std::string cstr = str;
#if						\
  defined(WIN32)  ||				\
  defined(_WIN32) ||				\
  defined(__WIN32) &&				\
  !defined(__CYGWIN__)
  std::replace(cstr.begin(), cstr.end(), '/', '\\');
#else
  std::replace(cstr.begin(), cstr.end(), '\\', '/');
#endif
  return cstr;
}

void
otv::Mesh::Material::LoadMtl
(const tinyobj::material_t& tinymtl, std::string& dpath)
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
  loadimg(map_Kd,   ParsePath(tinymtl.diffuse_texname), dpath);
  loadimg(map_Ks,   ParsePath(tinymtl.specular_texname), dpath);
  loadimg(map_Ns,   ParsePath(tinymtl.specular_highlight_texname), dpath);
  loadimg(map_d,    ParsePath(tinymtl.alpha_texname), dpath);
  loadimg(map_Bump, ParsePath(tinymtl.bump_texname), dpath);
}

void
otv::Mesh::ComputePath
(const std::string& str)
{
  fpath = ParsePath(str);
  size_t p = fpath.find_last_of("/\\");
  if (p != std::string::npos) {
    dpath = fpath.substr(0, p + 1);
    fname = fpath.substr(p + 1, fpath.size()-dpath.size());
  }
  else {
    dpath = "";
    fname = fpath;
  }
}

void
otv::Mesh::TinyObjLoader::Clear()
{
  err.clear();
  attributes.vertices.clear();
  attributes.normals.clear();
  attributes.texcoords.clear();
  shapes.clear();
  materials.clear();
}

bool
otv::Mesh::LoadFromFileObj
(const char* filename, bool loadMtl)
{
  // initialize
  tiny.Clear();
  ComputePath(filename);
  
  // load mesh from file using tiny obj loader
  bool succeed = tinyobj::LoadObj(&(tiny.attributes), 
				  &(tiny.shapes), 
				  &(tiny.materials), 
				  &(tiny.err), 
				  fpath.c_str(),
				  dpath == "" ? nullptr : dpath.c_str(),
				  true);
  if (!tiny.err.empty()) { std::cerr << tiny.err << std::endl; }
  if (!succeed) { return false; }

  // initialize bounding box
  bbox.upper = vec3f(std::numeric_limits<float>::min());
  bbox.lower = vec3f(std::numeric_limits<float>::max());

  // initialize _geometries_ and _materials_ array
  // note: the last material is the default
  materials.resize(tiny.materials.size()+1); 
  geometries.resize(tiny.shapes.size());

  // process geometry
  for (size_t s = 0; s < tiny.shapes.size(); s++) // Loop over shapes
  {    
    Geometry& geo = geometries[s];
    // note: it seems tiny obj loader uses per-face material
    //       but we need only one material per geometry
    //       so we use the first face for material index
    geo.num_faces = tiny.shapes[s].mesh.num_face_vertices.size();
    if (geo.num_faces > 0)
    {
      geo.mtl_index = tiny.shapes[s].mesh.material_ids[0];
    }
    else
    {
      std::cerr << "[Warning] found one shape with no faces" << std::endl;
    }
    
    size_t vidx_offset = 0;
    for (size_t f = 0; f < geo.num_faces; f++) // Loop over faces (polygon)
    {      
      // number of vertices of this face
      int fv = tiny.shapes[s].mesh.num_face_vertices[f]; 
      if (fv != 3)
      {
	static bool warned = false;
	if (!warned) {
	  std::cerr << "[Error] this mesh is not a pure trianglar mesh"
		    << std::endl;
	  warned = true;
	}
	exit(EXIT_FAILURE); 
      }	

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv /*fv=3*/; v++)
      {
	tinyobj::index_t idx = tiny.shapes[s].mesh.indices[vidx_offset + v];
	float vx = tiny.attributes.vertices[3 * idx.vertex_index + 0];
	float vy = tiny.attributes.vertices[3 * idx.vertex_index + 1];
	float vz = tiny.attributes.vertices[3 * idx.vertex_index + 2];
	geo.index.push_back(geo.index.size());
	geo.vertex.push_back(vx);
	geo.vertex.push_back(vy);
	geo.vertex.push_back(vz);
	bbox.upper = glm::max(bbox.upper, vec3f(vx, vy, vz));
	bbox.lower = glm::min(bbox.lower, vec3f(vx, vy, vz));
	  
	if (idx.normal_index >= 0)
	{
	  geo.has_normal = true;
	  float nx = tiny.attributes.normals[3 * idx.normal_index + 0];
	  float ny = tiny.attributes.normals[3 * idx.normal_index + 1];
	  float nz = tiny.attributes.normals[3 * idx.normal_index + 2];
	  geo.normal.push_back(nx);
	  geo.normal.push_back(ny);
	  geo.normal.push_back(nz);
	} 
	else
	{
	  static bool warned = false;
	  geo.has_normal = false;
	  if (!warned) {
	    std::cerr << "[Warning] normal not found" << std::endl;
	    warned = true;
	  }
	}
	  
	if (idx.texcoord_index >= 0)
	{
	  geo.has_texcoord = true;
	  float tx = tiny.attributes.texcoords[2 * idx.texcoord_index + 0];
	  float ty = tiny.attributes.texcoords[2 * idx.texcoord_index + 1];
	  geo.texcoord.push_back(tx);
	  geo.texcoord.push_back(ty);
	}
	else
	{
	  static bool warned = false;
	  geo.has_texcoord = false;
	  if (!warned) {
	    std::cerr << "[Warning] texture coordinate not found" << std::endl;
	    warned = true;
	  }
	}
      }
      vidx_offset += fv;	
    }
  }
  
  // process materials
  for (int i = 0; i < tiny.materials.size(); ++i)
  {
    materials[i].LoadMtl(tiny.materials[i], dpath);
  }
  return true;
}

void otv::Mesh::AddToModel(OSPModel& model, OSPRenderer& renderer) 
{
  for (auto& geo : geometries)
  {    
    if (geo.num_faces != 0)
    {
      OSPGeometry gdata = ospNewGeometry("triangles");
      
      // vertex
      OSPData vdata =
	ospNewData(geo.vertex.size() / 3, 
		   OSP_FLOAT3, geo.vertex.data(), 
		   OSP_DATA_SHARED_BUFFER);
      ospCommit(vdata);
      ospSetObject(gdata, "vertex", vdata);
      
      // index
      OSPData idata = 
	ospNewData(geo.index.size() / 3, 
		   OSP_INT3, geo.index.data(), 
		   OSP_DATA_SHARED_BUFFER);
      ospCommit(idata);
      ospSetObject(gdata, "index", idata);

      // normal
      if (geo.has_normal) {
	OSPData ndata =
	  ospNewData(geo.normal.size() / 3, 
		     OSP_FLOAT3, geo.normal.data(), 
		     OSP_DATA_SHARED_BUFFER);
	ospCommit(ndata);
	ospSetObject(gdata, "vertex.normal", ndata);
      }

      // texture coordinate
      if (geo.has_texcoord) {
	OSPData tdata = 
	  ospNewData(geo.texcoord.size() / 2, 
		     OSP_FLOAT2, geo.texcoord.data(), 
		     OSP_DATA_SHARED_BUFFER);
	ospCommit(tdata);
	ospSetObject(gdata, "vertex.texcoord", tdata);
      }

      // material
      // note: one geometry should have one corresponding material
      //       if the `mtl_index` is -1, which means the OBJ file
      //       doesn't define a seperate material for this geometry.
      //       So we use the default material instead
      auto  mtl_idx = geo.mtl_index < 0 ? (materials.size()-1) : geo.mtl_index;
      auto& mtl = materials[mtl_idx];
      OSPMaterial mtl_data = ospNewMaterial(renderer, "OBJMaterial");
      ospSetVec3f(mtl_data, "Kd", osp::vec3f{mtl.Kd.x, mtl.Kd.y, mtl.Kd.z});
      ospSetVec3f(mtl_data, "Ks", osp::vec3f{mtl.Ks.x, mtl.Ks.y, mtl.Ks.z});
      ospSet1f(mtl_data, "Ns", mtl.Ns);
      ospSet1f(mtl_data, "d",  mtl.d);	    
      if (!mtl.map_Kd.IsEmpty()) {
	OSPTexture2D map_Kd = mtl.map_Kd.CreateOSPTex();
	ospCommit(map_Kd);
	ospSetObject(mtl_data, "map_Kd", map_Kd);
      }
      if (!mtl.map_Ks.IsEmpty()) {
	OSPTexture2D map_Ks = mtl.map_Ks.CreateOSPTex();
	ospCommit(map_Ks);
	ospSetObject(mtl_data, "map_Ks", map_Ks);
      }
      if (!mtl.map_Ns.IsEmpty()) {
	OSPTexture2D map_Ns = mtl.map_Ns.CreateOSPTex();
	ospCommit(map_Ns);
	ospSetObject(mtl_data, "map_Ns", map_Ns);
      }
      if (!mtl.map_d.IsEmpty()) {
	OSPTexture2D map_d = mtl.map_d.CreateOSPTex();
	ospCommit(map_d);
	ospSetObject(mtl_data, "map_d", map_d);
      }
      if (!mtl.map_Bump.IsEmpty()) {
	OSPTexture2D map_Bump = mtl.map_Bump.CreateOSPTex();
	ospCommit(map_Bump);
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
