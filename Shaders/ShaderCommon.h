#define BINDING_TEXTURE 0 // Sampled image
#define BINDING_BUFFER 1
// #define LUZ_BINDING_TLAS 2
#define BINDING_STORAGE_IMAGE 3

struct imageOptConstants {
	int width;
	int height;
	int factor; //test
	int pad[1];
};