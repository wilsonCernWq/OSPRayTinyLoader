#include "scenegraph.h"

#include <limits>
#include <iostream>

/* this is the file for the json implementation */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

/* I am defining a template in 'cpp' because it is needed only here */
template<typename T>
Value CreateVec(T vec, Document::AllocatorType& allocator)
{ 
  Value array(kArrayType);
  for (int i = 0; i < vec.length(); ++i)
  {
    array.PushBack(vec[i], allocator); // fluent API
  }
  return array;
}
  
void otv::SceneGraph::Dump()
{
  // initialization
  Document doc; doc.SetObject();
  Document::AllocatorType& allocator = doc.GetAllocator();

  // testing
  Value array2 = CreateVec(vec2f(1.0f), allocator);
  Value array3 = CreateVec(vec3f(1.0f), allocator);
  Value array4 = CreateVec(vec4f(1.0f), allocator);
  doc.AddMember("array2", array2, allocator);
  doc.AddMember("array3", array3, allocator);
  doc.AddMember("array4", array4, allocator);

  // output
  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  const char* output = buffer.GetString();
  std::cout << output << std::endl;
}
