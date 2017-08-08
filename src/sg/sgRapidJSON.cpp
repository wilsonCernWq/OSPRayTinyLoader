#include "sgRapidJSON.h"
#include <limits>
#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_access.hpp>
/* this is the file for the json implementation */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

/*---------------------------------------------------------------------------*/
/* I am defining a template in 'cpp' because it is needed only here */
template<typename T>
Value CreateVec(T vec, Document::AllocatorType& alloc)
{ 
  Value array(kArrayType);
  for (int i = 0; i < vec.length(); ++i)
  {
    array.PushBack(vec[i], alloc); // fluent API
  }
  return array;
}

Value
CreateMeshObject
(const otv::SceneGraphAbstract::SgObject& self, Document::AllocatorType& alloc)
{
  // create mesh
  Value obj(kObjectType);
  {
    Value filename;
    filename.SetString(otv::GetFullPath(self.filename).c_str(), alloc);
    obj.AddMember("file", filename, alloc);
    obj.AddMember("translate", CreateVec(self.T, alloc), alloc);
    obj.AddMember("rotate.vx", CreateVec(self.Rv[0] /*glm::column(self.R,0)*/, alloc), alloc);
    obj.AddMember("rotate.vy", CreateVec(self.Rv[1] /*glm::column(self.R,1)*/, alloc), alloc);
    obj.AddMember("rotate.vz", CreateVec(self.Rv[2] /*glm::column(self.R,2)*/, alloc), alloc);
    obj.AddMember("bbox.upper",  CreateVec(self.bbox.upper, alloc), alloc);
    obj.AddMember("bbox.lower",  CreateVec(self.bbox.lower, alloc), alloc);
    obj.AddMember("center", CreateVec(self.center, alloc), alloc);
  }
  return obj;
}

Value
CreateSingleSceneGraph
(const otv::SceneGraphAbstract* sg, Document::AllocatorType& alloc)
{
  Value root(kObjectType);
  
  // create camera
  Value camera(kObjectType);
  {
    camera.AddMember("focus", CreateVec(sg->cameraSg.focus, alloc), alloc);
    camera.AddMember("dir",   CreateVec(sg->cameraSg.dir,   alloc), alloc);
    camera.AddMember("pos",   CreateVec(sg->cameraSg.pos,   alloc), alloc);
    camera.AddMember("up",    CreateVec(sg->cameraSg.up,    alloc), alloc);
    camera.AddMember("zoom",  sg->cameraSg.zoom, alloc);
    camera.AddMember("matrix.vx",CreateVec(glm::column(sg->cameraSg.matrix,0),alloc),alloc);
    camera.AddMember("matrix.vy",CreateVec(glm::column(sg->cameraSg.matrix,1),alloc),alloc);
    camera.AddMember("matrix.vz",CreateVec(glm::column(sg->cameraSg.matrix,2),alloc),alloc);
    camera.AddMember("matrix.vw",CreateVec(glm::column(sg->cameraSg.matrix,3),alloc),alloc);
  }
  root.AddMember("camera", camera, alloc);

  // create light
  Value light(kObjectType);
  {
    // ambient light
    Value Lamb(kObjectType);
    {
      Lamb.AddMember("intensity", sg->lightSg.Iamb, alloc);
      Lamb.AddMember("color", CreateVec(sg->lightSg.Camb, alloc), alloc);
    }
    light.AddMember("ambient", Lamb, alloc);    
    // distant light
    Value Ldir(kObjectType);
    {
      Ldir.AddMember("intensity", sg->lightSg.Idir, alloc);
      Ldir.AddMember("color", CreateVec(sg->lightSg.Cdir, alloc), alloc);
      Ldir.AddMember("matrix.vx",CreateVec(glm::column(sg->lightSg.Mdir,0),alloc),alloc);
      Ldir.AddMember("matrix.vy",CreateVec(glm::column(sg->lightSg.Mdir,1),alloc),alloc);
      Ldir.AddMember("matrix.vz",CreateVec(glm::column(sg->lightSg.Mdir,2),alloc),alloc);
      Ldir.AddMember("matrix.vw",CreateVec(glm::column(sg->lightSg.Mdir,3),alloc),alloc);
      Ldir.AddMember("focus", CreateVec(sg->lightSg.Fdir, alloc), alloc);
      Ldir.AddMember("dir",   CreateVec(sg->lightSg.Ddir, alloc), alloc);
      Ldir.AddMember("pos",   CreateVec(sg->lightSg.Pdir, alloc), alloc);
      Ldir.AddMember("up",    CreateVec(sg->lightSg.Udir, alloc), alloc);
      Ldir.AddMember("zoom", sg->lightSg.Zdir, alloc);      
    }
    light.AddMember("directional", Ldir, alloc);
    // sun light
    Value Lsun(kObjectType);
    {
      Lsun.AddMember("intensity", sg->lightSg.Isun, alloc);
      Lsun.AddMember("color", CreateVec(sg->lightSg.Csun, alloc), alloc);
      Lsun.AddMember("matrix.vx",CreateVec(glm::column(sg->lightSg.Msun,0),alloc),alloc);
      Lsun.AddMember("matrix.vy",CreateVec(glm::column(sg->lightSg.Msun,1),alloc),alloc);
      Lsun.AddMember("matrix.vz",CreateVec(glm::column(sg->lightSg.Msun,2),alloc),alloc);
      Lsun.AddMember("matrix.vw",CreateVec(glm::column(sg->lightSg.Msun,3),alloc),alloc);
      Lsun.AddMember("focus", CreateVec(sg->lightSg.Fsun, alloc), alloc);
      Lsun.AddMember("dir",   CreateVec(sg->lightSg.Dsun, alloc), alloc);
      Lsun.AddMember("pos",   CreateVec(sg->lightSg.Psun, alloc), alloc);
      Lsun.AddMember("up",    CreateVec(sg->lightSg.Usun, alloc), alloc);
      Lsun.AddMember("zoom", sg->lightSg.Zsun, alloc);      
    }
    light.AddMember("sun", Lsun, alloc);
  }
  root.AddMember("light", light, alloc);
  
  // create mesh
  Value array(kArrayType);
  for (auto& m : sg->objectsSg) {
    array.PushBack(CreateMeshObject(m, alloc), alloc); // fluent API
  }
  root.AddMember("meshes", array, alloc);
    
  return root;
}

void otv::SgRapidJSON::Dump(const std::string& fname)
{
  // initialization
  Document doc; doc.SetObject();
  Document::AllocatorType& alloc = doc.GetAllocator();

  // create JSON
  doc.AddMember("sg", CreateSingleSceneGraph(this, alloc), alloc);

  // get output string
  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  const char* output = buffer.GetString();

  // print to file
  std::ofstream myfile;
  myfile.open((fname + ".json").c_str());
  myfile << output << std::endl;;
  myfile.close();
}

/*---------------------------------------------------------------------------*/
/* I am defining a template in 'cpp' because it is needed only here */
otv::vec3f ReadVec3(const Value& a)
{ 
  assert(a.IsArray());
  assert(a.Size() == 3);
  return otv::vec3f(a[0].GetFloat(),a[1].GetFloat(),a[2].GetFloat());
}

otv::vec4f ReadVec4(const Value& a)
{ 
  assert(a.IsArray());
  assert(a.Size() == 4);
  return otv::vec4f(a[0].GetFloat(),a[1].GetFloat(),a[2].GetFloat(),a[3].GetFloat());
}

void otv::SgRapidJSON::Load(const std::string& fname)
{
  // read file
  std::ifstream in(fname.c_str());
  std::string contents((std::istreambuf_iterator<char>(in)), 
		       std::istreambuf_iterator<char>());
  
  // parse into JSON
  Document d;
  d.Parse(contents.c_str());
  std::cout << "successful loading" << std::endl;

  // parse camera
  cameraSg.focus = ReadVec3(d["sg"]["camera"]["focus"]);
  cameraSg.dir   = ReadVec3(d["sg"]["camera"]["dir"]);
  cameraSg.pos   = ReadVec3(d["sg"]["camera"]["pos"]);
  cameraSg.up    = ReadVec3(d["sg"]["camera"]["up"]);
  cameraSg.zoom  = d["sg"]["camera"]["zoom"].GetFloat();
  cameraSg.matrix = mat4f(ReadVec4(d["sg"]["camera"]["matrix.vx"]),
			  ReadVec4(d["sg"]["camera"]["matrix.vy"]),
			  ReadVec4(d["sg"]["camera"]["matrix.vz"]),
			  ReadVec4(d["sg"]["camera"]["matrix.vw"]));
  
  std::cout << "successful loading camera" << std::endl;

  // parse light
  // -- ambient
  lightSg.Iamb = d["sg"]["light"]["ambient"]["intensity"].GetFloat();
  lightSg.Camb = ReadVec3(d["sg"]["light"]["ambient"]["color"]);
  // -- directional
  lightSg.Idir = d["sg"]["light"]["directional"]["intensity"].GetFloat();
  lightSg.Cdir = ReadVec3(d["sg"]["light"]["directional"]["color"]);
  lightSg.Mdir = mat4f(ReadVec4(d["sg"]["light"]["directional"]["matrix.vx"]),
		       ReadVec4(d["sg"]["light"]["directional"]["matrix.vy"]),
		       ReadVec4(d["sg"]["light"]["directional"]["matrix.vz"]),
		       ReadVec4(d["sg"]["light"]["directional"]["matrix.vw"]));
  lightSg.Fdir = ReadVec3(d["sg"]["light"]["directional"]["focus"]);
  lightSg.Udir = ReadVec3(d["sg"]["light"]["directional"]["up"]);
  lightSg.Pdir = ReadVec3(d["sg"]["light"]["directional"]["pos"]);
  lightSg.Ddir = ReadVec3(d["sg"]["light"]["directional"]["dir"]);
  lightSg.Zdir = d["sg"]["light"]["directional"]["zoom"].GetFloat();	    
  // -- sum
  lightSg.Isun = d["sg"]["light"]["sun"]["intensity"].GetFloat();
  lightSg.Csun = ReadVec3(d["sg"]["light"]["sun"]["color"]);
  lightSg.Msun = mat4f(ReadVec4(d["sg"]["light"]["sun"]["matrix.vx"]),
		       ReadVec4(d["sg"]["light"]["sun"]["matrix.vy"]),
		       ReadVec4(d["sg"]["light"]["sun"]["matrix.vz"]),
		       ReadVec4(d["sg"]["light"]["sun"]["matrix.vw"]));
  lightSg.Fsun = ReadVec3(d["sg"]["light"]["sun"]["focus"]);
  lightSg.Usun = ReadVec3(d["sg"]["light"]["sun"]["up"]);
  lightSg.Psun = ReadVec3(d["sg"]["light"]["sun"]["pos"]);
  lightSg.Dsun = ReadVec3(d["sg"]["light"]["sun"]["dir"]);
  lightSg.Zsun = d["sg"]["light"]["sun"]["zoom"].GetFloat();	    

  std::cout << "successful loading light" << std::endl;
  
  // parse mesh
  for (auto& m : d["sg"]["meshes"].GetArray()) {
    objectsSg.emplace_back();
    objectsSg.back().filename = m["file"].GetString();
    objectsSg.back().T = ReadVec3(m["translate"]);
    objectsSg.back().Rv[0] = ReadVec3(m["rotate.vx"]);
    objectsSg.back().Rv[1] = ReadVec3(m["rotate.vy"]);
    objectsSg.back().Rv[2] = ReadVec3(m["rotate.vz"]);
  }
  std::cout << "successful loading mesh" << std::endl;
}
