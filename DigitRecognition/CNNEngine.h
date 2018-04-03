#pragma once
#define USE_OPENCV
#include <caffe\caffe.hpp>
#include <opencv2\core.hpp>
#include <QObject>
#include <QVector>
#include <memory>
#include <vector>

#include "Constants.h"

class CNNEngine : public QObject
{
	Q_OBJECT

public:
	static CNNEngine& instance() {
		static CNNEngine engine;
		return engine;
	}

	void initialize();
	bool isInitialized() const noexcept { return m_isInitialized; }
	
	QVector<int> recognizeNumbers(const cv::Mat& grayImage, std::vector<cv::Mat>& subImages);
	QVector<int> recognizeNumbers(std::vector<cv::Mat>&& images);

	void setDigitSize(int width, int height) noexcept { m_digitWidth = width; m_digitHeight = height; }
	void setRate(double val)				 noexcept { m_rate	 = val; }
	void setStep(int val)					 noexcept { m_step	 = val; }
	void setMaxSum(int val)					 noexcept { m_maxSum = val; }
	void setMinSum(int val)					 noexcept { m_minSum = val; }
	void setAi(bool val)					 noexcept { m_useAi	 = val; }

private:
	CNNEngine(QObject *parent = nullptr);
	void preparseImage(const cv::Mat& image, std::vector<cv::Mat>& subImages);
	QVector<int> processCNN(const std::vector<cv::Mat>& subImages);
	QVector<int> analyzeOutput(const std::vector<float>& output);

private:
	std::unique_ptr<caffe::Net<float>> m_net = nullptr;
	double m_rate			= DEFAULT_RECOGNITION_RATE;
	
	int m_step				= 1;
	int m_minSum			= 0;
	int m_maxSum			= 0;
	int m_digitWidth		= INPUT_WIDTH;
	int m_digitHeight		= INPUT_HEIGHT;
	
	bool m_isInitialized	= false;
	bool m_useAi			= false;
};
