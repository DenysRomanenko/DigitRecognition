#pragma once

constexpr auto protoFile = "files\\lenet.prototxt";
constexpr auto modelFile = "files\\lenet.caffemodel";

constexpr double DEFAULT_RECOGNITION_RATE = 0.82;
constexpr int CAPTURED_IMAGE_WIDTH	= 640;
constexpr int CAPTURED_IMAGE_HEIGHT = 480;
constexpr int INPUT_WIDTH  = 28;
constexpr int INPUT_HEIGHT = 28;
constexpr int X_POSITION = 40;
constexpr int Y_POSITION = 10;
constexpr int ANALYZING_IMAGE_WIDTH	 = 560;
constexpr int ANALYZING_IMAGE_HEIGHT = 448;
constexpr int actualWidth = ANALYZING_IMAGE_WIDTH;
constexpr int actualHeight = ANALYZING_IMAGE_HEIGHT;
constexpr int CNN_OUTPUT = 10;
