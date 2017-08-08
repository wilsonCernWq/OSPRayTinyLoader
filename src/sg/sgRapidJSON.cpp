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
    camera.AddMember("matrix.vx", CreateVec(glm::column(sg->cameraSg.matrix,0), alloc), alloc);
    camera.AddMember("matrix.vy", CreateVec(glm::column(sg->cameraSg.matrix,1), alloc), alloc);
    camera.AddMember("matrix.vz", CreateVec(glm::column(sg->cameraSg.matrix,2), alloc), alloc);
    camera.AddMember("matrix.vw", CreateVec(glm::column(sg->cameraSg.matrix,3), alloc), alloc);
  }
  root.AddMember("camera", camera, alloc);

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
  // HOLD;
}
