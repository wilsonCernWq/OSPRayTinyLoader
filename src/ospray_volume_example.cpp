#include <stdint.h>
#include <stdio.h>
#include <vector>
#ifdef _WIN32
#  define NOMINMAX
#  include <malloc.h>
#else
#  include <alloca.h>
#endif
#include "ospray/ospray.h"
#include "ospray/ospcommon/vec.h"

using namespace ospcommon;

// helper function to write the rendered image as PPM file
void writePPM(const char *fileName, const vec2i &size, const uint32_t *pixel) {
	FILE *file = fopen(fileName, "wb");
	fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
	unsigned char *out = (unsigned char *)alloca(3*size.x);
	for (int y = 0; y < size.y; y++) {
		const unsigned char *in = (const unsigned char *)&pixel[(size.y-1-y)*size.x];
		for (int x = 0; x < size.x; x++) {
			out[3*x + 0] = in[4*x + 0];
			out[3*x + 1] = in[4*x + 1];
			out[3*x + 2] = in[4*x + 2];
		}
		fwrite(out, 3*size.x, sizeof(char), file);
	}
	fprintf(file, "\n");
	fclose(file);
}

int main(int argc, const char **argv){



	vec3i volumeDims(256, 256, 256);
	for (int i = 1; i < argc; ++i){
		if (std::strcmp(argv[i], "-s") == 0) {
			volumeDims.x = std::atoi(argv[++i]);
			volumeDims.y = std::atoi(argv[++i]);
			volumeDims.z = std::atoi(argv[++i]);
			std::cout << "Got volume dims from cmd line: " << volumeDims << "\n";
		}
	}

	const vec2i imageSize(1024, 1024);
	const vec3f camPos(-248, -62, 60);
	const vec3f camDir = vec3f(volumeDims) / 2.f - camPos;
	const vec3f camUp(0, 0, 1);

	ospInit(&argc, argv);

	OSPCamera camera = ospNewCamera("perspective");
	ospSetf(camera, "aspect", imageSize.x / static_cast<float>(imageSize.y));
	ospSetVec3f(camera, "pos", (osp::vec3f&)camPos);
	ospSetVec3f(camera, "dir", (osp::vec3f&)camDir);
	ospSetVec3f(camera, "up",  (osp::vec3f&)camUp);
	ospCommit(camera);

	OSPTransferFunction transferFcn = ospNewTransferFunction("piecewise_linear");
	const std::vector<vec3f> colors = {
		vec3f(0, 0, 0.563),vec3f(0, 0, 1),vec3f(0, 1, 1),vec3f(0.5, 1, 0.5),vec3f(1, 1, 0),vec3f(1, 0, 0),vec3f(0.5, 0, 0)
	};
	const std::vector<float> opacities = {0.01f, 0.05f, 0.01f};
	OSPData colorsData = ospNewData(colors.size(), OSP_FLOAT3, colors.data());
	ospCommit(colorsData);
	OSPData opacityData = ospNewData(opacities.size(), OSP_FLOAT, opacities.data());
	ospCommit(opacityData);
	const vec2f valueRange(static_cast<float>(0), static_cast<float>(255));
	ospSetData(transferFcn, "colors", colorsData);
	ospSetData(transferFcn, "opacities", opacityData);
	ospSetVec2f(transferFcn, "valueRange", (osp::vec2f&)valueRange);
	ospCommit(transferFcn);

	std::vector<unsigned char> volumeData(volumeDims.x * volumeDims.y * volumeDims.z, 0);
	for (size_t i = 0; i < volumeData.size(); ++i) { volumeData[i] = i % 255; }
	OSPVolume volume = ospNewVolume("block_bricked_volume");
	ospSetString(volume, "voxelType", "uchar");
	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
	ospSetObject(volume, "transferFunction", transferFcn);
	ospSetRegion(volume, volumeData.data(), osp::vec3i{0, 0, 0}, (osp::vec3i&)volumeDims);
	ospCommit(volume);

	OSPModel world = ospNewModel();
	ospAddVolume(world, volume);
	ospCommit(world);

	OSPRenderer renderer = ospNewRenderer("scivis");
	ospSetObject(renderer, "model", world);
	ospSetObject(renderer, "camera", camera);
	ospCommit(renderer);

	OSPFrameBuffer framebuffer = ospNewFrameBuffer((osp::vec2i&)imageSize, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
	ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);

	for (int i = 0; i < 50; ++i) {
	    ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
	}

	const uint32_t * fb = (uint32_t*)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);
	writePPM("volume.ppm", imageSize, fb);
	ospUnmapFrameBuffer(fb, framebuffer);
	return 0;
}

