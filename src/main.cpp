#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc

#include "common.h"
#include "helper.h"
#include "trackball.h"
#include "meshwrapper.h"
#include "callbacks.h"

using namespace otv;

void render()
{
    ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
    gfb.BindTexture();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINSIZE.x, WINSIZE.y, GL_RGBA, GL_UNSIGNED_BYTE, ofb);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gfb.GetID());
    glBlitFramebuffer(0, 0, WINSIZE.x, WINSIZE.y, 0, 0, WINSIZE.x, WINSIZE.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glutSwapBuffers();
}

int main(int argc, const char **argv)
{
    //! check argument number
    if (argc < 2) {
	std::cerr << "The program needs at lease one input argument!" << std::endl;
	exit(EXIT_FAILURE);
    }
    ospInit(&argc, argv);

    //! create world and renderer
    world = ospNewModel();
    renderer = ospNewRenderer("scivis"); // possible options: "pathtracer" "raytracer"

    //! geometry/volume
    mesh.LoadFromFileObj(argv[1]);
    camFocus = otv::make_vec(0.5 * (mesh.GetBBoxMax() + mesh.GetBBoxMin()));
    camPos *= (mesh.GetBBoxMax() - mesh.GetBBoxMin()).Length() / 10.0f;
    for (int i = 0; i < mesh.geometries.size(); ++i) {
	if (mesh.geometries[i].num_faces != 0) {
	    OSPGeometry geometry_data = ospNewGeometry("triangles");
	    //! vertex
	    OSPData vertex_data = ospNewData(mesh.geometries[i].vertex.size() / 3, OSP_FLOAT3, mesh.geometries[i].vertex.data(), OSP_DATA_SHARED_BUFFER);
	    ospCommit(vertex_data);
	    ospSetObject(geometry_data, "vertex", vertex_data);
	    //! index
	    OSPData index_data = ospNewData(mesh.geometries[i].index.size() / 3, OSP_INT3, mesh.geometries[i].index.data(), OSP_DATA_SHARED_BUFFER);
	    ospCommit(index_data);
	    ospSetObject(geometry_data, "index", index_data);
	    //! normal
	    if (mesh.geometries[i].has_normal) {
		OSPData normal_data = ospNewData(mesh.geometries[i].normal.size() / 3, OSP_FLOAT3, mesh.geometries[i].normal.data(), OSP_DATA_SHARED_BUFFER);
		ospCommit(normal_data);
		ospSetObject(geometry_data, "vertex.normal", normal_data);
	    }
	    //! texture coordinate
	    if (mesh.geometries[i].has_texcoord) {
		OSPData texcoord_data = ospNewData(mesh.geometries[i].texcoord.size() / 2, OSP_FLOAT2, mesh.geometries[i].texcoord.data(), OSP_DATA_SHARED_BUFFER);
		ospCommit(texcoord_data);
		ospSetObject(geometry_data, "vertex.texcoord", texcoord_data);
	    }
	    //! material
	    OSPMaterial mtl_data = ospNewMaterial(renderer, "OBJMaterial");
	    ospcommon::vec3f mtl_Tf = otv::make_vec(mesh.GetMaterial(i, "Tf"));
	    ospcommon::vec3f mtl_Kd = otv::make_vec(mesh.GetMaterial(i, "Kd"));
	    ospcommon::vec3f mtl_Ks = otv::make_vec(mesh.GetMaterial(i, "Ks"));			
	    ospSetVec3f(mtl_data, "Tf", (osp::vec3f&)mtl_Tf);
	    ospSetVec3f(mtl_data, "Kd", (osp::vec3f&)mtl_Kd);
	    ospSetVec3f(mtl_data, "Ks", (osp::vec3f&)mtl_Ks);
	    ospSet1f(mtl_data, "Ns", mesh.tiny.materials[i].shininess);
	    ospSet1f(mtl_data, "d",  mesh.tiny.materials[i].dissolve);
	    if (!mesh.textures[i].map_Kd.IsEmpty()) {
		auto tex_dim = otv::make_vec(mesh.textures[i].map_Kd.Size());
		OSPTexture2D map_Kd = ospNewTexture2D((osp::vec2i&)tex_dim, OSP_TEXTURE_RGBA8, mesh.textures[i].map_Kd.data.data(), 1);
		ospSetObject(mtl_data, "map_Kd", map_Kd);
	    }
	    if (!mesh.textures[i].map_Ks.IsEmpty()) {
		auto tex_dim = otv::make_vec(mesh.textures[i].map_Ks.Size());
		OSPTexture2D map_Ks = ospNewTexture2D((osp::vec2i&)tex_dim, OSP_TEXTURE_RGBA8, mesh.textures[i].map_Ks.data.data(), 1);
		ospSetObject(mtl_data, "map_Ks", map_Ks);
	    }
	    if (!mesh.textures[i].map_Ns.IsEmpty()) {
		auto tex_dim = otv::make_vec(mesh.textures[i].map_Ns.Size());
		OSPTexture2D map_Ns = ospNewTexture2D((osp::vec2i&)tex_dim, OSP_TEXTURE_RGBA8, mesh.textures[i].map_Ns.data.data(), 1);
		ospSetObject(mtl_data, "map_Ns", map_Ns);
	    }
	    if (!mesh.textures[i].map_d.IsEmpty()) {
		auto tex_dim = otv::make_vec(mesh.textures[i].map_d.Size());
		OSPTexture2D map_d = ospNewTexture2D((osp::vec2i&)tex_dim, OSP_TEXTURE_RGBA8, mesh.textures[i].map_d.data.data(), 1);
		ospSetObject(mtl_data, "map_d", map_d);
	    }
	    if (!mesh.textures[i].map_Bump.IsEmpty()) {
		auto tex_dim = otv::make_vec(mesh.textures[i].map_Bump.Size());
		OSPTexture2D map_Bump = ospNewTexture2D((osp::vec2i&)tex_dim, OSP_TEXTURE_RGBA8, mesh.textures[i].map_Bump.data.data(), 1);
		ospSetObject(mtl_data, "map_Bump", map_Bump);
	    }
	    ospCommit(mtl_data);
	    ospSetMaterial(geometry_data, mtl_data);
	    //! commit geometry
	    ospCommit(geometry_data);
	    ospAddGeometry(world, geometry_data);
	}
    }
    ospCommit(world);

    //! camera
    camera = ospNewCamera("perspective");
    ospSetf(camera, "aspect", static_cast<float>(WINSIZE.x) / static_cast<float>(WINSIZE.y));
    UpdateCamera(false);

    //! lighting
    OSPLight ambient_light = ospNewLight(renderer, "AmbientLight");
    ospCommit(ambient_light);
    OSPLight directional_light = ospNewLight(renderer, "DirectionalLight");
    ospSetVec3f(directional_light, "direction", osp::vec3f{ 0.0f, 11.0f, 0.0f});
    ospCommit(directional_light);
    std::vector<OSPLight> light_list { ambient_light, directional_light };
    OSPData lights = ospNewData(light_list.size(), OSP_OBJECT, light_list.data());
    ospCommit(lights);

    //! renderer
    ospSetData(renderer, "lights", lights);
    ospSetObject(renderer, "model", world);
    ospSetObject(renderer, "camera", camera);
    ospCommit(renderer);

    //! render to buffer
    framebuffer = ospNewFrameBuffer((osp::vec2i&)WINSIZE, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
    ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
    ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
    ofb = (uint32_t*)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);

    //! initialize openGL
    {
	glutInit(&argc, const_cast<char**>(argv));
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(WINX, WINY);
	glutInitWindowSize(WINSIZE.x, WINSIZE.y);
	glutCreateWindow(argv[0]);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
	    std::cerr << "Error: Cannot Initialize GLEW " << glewGetErrorString(err) << std::endl;
	    return EXIT_FAILURE;
	}
	gfb.Initialize(true, 4, WINSIZE.x, WINSIZE.y);
    }

    // execute the program
    {
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glDisable(GL_DEPTH_TEST);
	glutDisplayFunc(render);
	glutIdleFunc(Idle);
	glutMouseFunc(GetMouseButton);
	glutMotionFunc(GetMousePosition);
	glutKeyboardFunc(GetNormalKeys);
	glutInitContextFlags(GLUT_DEBUG);
	glutMainLoop();
    }

    // exit
    Clean();
    return EXIT_SUCCESS;
}
