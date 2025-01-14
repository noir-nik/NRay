import lmath;
import vulkan_backend;

using Pixel = lmath::float4;

void fillUV(lmath::float4* image, int width, int height){
	for (auto i = 0; i < height; i++) {
		for (auto j = 0; j < width; j++) {
			int idx = i * width + j;
			image[idx].r = i / (float)height;
			image[idx].g = j / (float)width;
			image[idx].b = 0.0f;
			image[idx].a = 1.0f;
		}
	}
}
