#ifdef ENGINE
export module FeatureTest;
import lmath;
using lmath::mat4;

export
class FeatureTestApplication {
public:
	void run(char const* gltfPath);
private:
	void Create(char const* gltfPath);
	void Setup();
	void Draw();
	void Compute();
	void MainLoop();
	void Finish();
};
#endif // ENGINE

struct FeatureTestConstants {
	mat4 view;
	mat4 proj;
	mat4 viewProj;
};
