export module ImageOptimization;

export
struct ImageOptimizationInfo
{
	int width;
	int height;
	float learningRate;
	int numIterations;
};

export
class ImageOptimizationApplication {
public:
	void run(ImageOptimizationInfo* pImageOptimizationInfo);
private:
	void Setup();
	void Create();
	void Compute();
	void CmdOptimizationStep();
	void Finish();

	ImageOptimizationInfo* info;
};

export
struct ImageOptConstants {
	int width;
	int height;
	int imageOptRID;
	int imageGTRID;

	int gradRID;
	float learningRate;
	int numIterations;
	int pad[1];
};