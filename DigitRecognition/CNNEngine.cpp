#include "CNNEngine.h"

#include <caffe\util\io.hpp>
#include <caffe\blob.hpp>
#include <caffe\layers\memory_data_layer.hpp>
#include <opencv2\imgproc.hpp>
#include <QFile>
#include <QSet>

CNNEngine::CNNEngine(QObject *parent)
	: QObject(parent)
{}

void CNNEngine::initialize() {
	caffe::Caffe::set_mode(caffe::Caffe::Brew::CPU);

	if (!(QFile::exists(protoFile) && QFile::exists(modelFile)))
		return;
	
	m_net = std::make_unique<caffe:: Net<float>>(protoFile, caffe::TEST);
	m_net->CopyTrainedLayersFrom(modelFile);

	m_isInitialized = true;
}

QVector<int> CNNEngine::recognizeNumbers(const cv::Mat& grayImage, std::vector<cv::Mat>& subImages) {
	QSet<int> result;
	preparseImage(grayImage, subImages);
	
	if (m_useAi && subImages.size()) {
		result = QSet<int>::fromList(processCNN(subImages).toList());
	}
	
	return result.toList().toVector();
}

QVector<int> CNNEngine::recognizeNumbers(std::vector<cv::Mat>&& images) {
	for (auto& it : images) {
		cv::resize(it, it, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));
	}

	return processCNN(images);
}

void CNNEngine::preparseImage(const cv::Mat& image, std::vector<cv::Mat>& subImages) {
	subImages.clear();
	subImages.reserve(200);

	int digitHeight = m_digitHeight;
	int digitWidth	= m_digitWidth;
	int step		= m_step;
	int minSum		= m_minSum;
	int maxSum		= m_maxSum;

	for (int i = 0; (i + digitHeight) < image.rows; i += step) {
		for (int j = 0; (j + digitWidth) < image.cols; j += step) {
			const cv::Mat img(image, cv::Rect(j, i, digitWidth, digitHeight));
			auto whiteColorSum = cv::sum(img)[0]; // as image with digit count rectangle with specific number of white color 
			if (whiteColorSum > minSum && whiteColorSum < maxSum) {
				cv::Mat tmp;
				cv::resize(img, tmp, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));
				subImages.push_back(tmp);
			}
		}
	}
}

QVector<int> CNNEngine::processCNN(const std::vector<cv::Mat>& subImages) {
	using namespace caffe;
	auto memory_data_layer = boost::static_pointer_cast<MemoryDataLayer<float>>(m_net->layer_by_name("data"));
	memory_data_layer->set_batch_size(subImages.size());
	std::vector<int> label(subImages.size());
	memory_data_layer->AddMatVector(subImages, label);
	auto res = m_net->Forward()[1];

	auto begin = res->mutable_cpu_data();
	const std::vector<float> output(begin, begin + res->count());
	
	return analyzeOutput(output);
}

QVector<int> CNNEngine::analyzeOutput(const std::vector<float>& output) {
	QVector<int> result;
	result.reserve(output.size() % 10);
	int number = -1;
	float rate = m_rate;
	for (int i = 0, size = output.size(); i < size; ++i) {
		if (output[i] > rate) {
			rate = output[i];
			number = i % CNN_OUTPUT;
		}
		if ((i + 1) % CNN_OUTPUT == 0 && i != 0) {
			if (number != -1)
				result << number;
			number = -1;
			rate = m_rate;
		}
	}

	return result;
}
