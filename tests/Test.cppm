export module Test;

export
namespace Test {
enum TestName {
	NeuralSdf,
	ImageOptimization,
	Feature,
	RadienceField,
	HelloTriangle
};

void Test(TestName testName);
} // namespace Test