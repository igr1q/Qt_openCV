#include "mainwindow.h"
#include <QCoreApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QPixmap>

cv::VideoCapture cap;
cv::Mat frame;
bool captureStarted = false;

void processFrame(QLabel &label, QCheckBox &grayscaleCheckbox, QCheckBox &greenCheckbox, QCheckBox &gaussianFilterCheckbox) {
    if (cap.isOpened()) {
        cap.read(frame);
        if (!frame.empty()) {
            if (grayscaleCheckbox.isChecked()) {
                cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
                cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
            }
            if (greenCheckbox.isChecked()) {
                cv::Mat greenFilter(frame.size(), CV_8UC3, cv::Scalar(0, 255, 0));
                cv::bitwise_and(frame, greenFilter, frame);
            }
            if (gaussianFilterCheckbox.isChecked()) {
                cv::GaussianBlur(frame, frame, cv::Size(0, 0), 3);
            }
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
            QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            QPixmap pixmap = QPixmap::fromImage(qimg);
            label.setPixmap(pixmap);
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

       QWidget window;
       window.setWindowTitle("Camera Filter Example");

       QLabel label;
       QVBoxLayout layout;
       layout.addWidget(&label);

       QPushButton button("Start/Stop");
       layout.addWidget(&button);

       QCheckBox grayscaleCheckbox("Черно-белый");
       layout.addWidget(&grayscaleCheckbox);

       QCheckBox greenCheckbox("Зеленый");
       layout.addWidget(&greenCheckbox);

       QCheckBox gaussianFilterCheckbox("Фильтр Гаусса");
       layout.addWidget(&gaussianFilterCheckbox);

       window.setLayout(&layout);
       window.show();

       QObject::connect(&button, &QPushButton::clicked, [&]() {
           if (captureStarted) {
               cap.release();
               button.setText("Start");
               label.clear();
           } else {
               cap.open(0); // Open the default camera
               button.setText("Stop");

           }
           captureStarted = !captureStarted;
       });

       QObject::connect(&grayscaleCheckbox, &QCheckBox::stateChanged, [&](int state) {
           if (captureStarted) {
               processFrame(label, grayscaleCheckbox, greenCheckbox, gaussianFilterCheckbox);
           }
       });

       QObject::connect(&greenCheckbox, &QCheckBox::stateChanged, [&](int state) {
           if (captureStarted) {
               processFrame(label, grayscaleCheckbox, greenCheckbox, gaussianFilterCheckbox);
           }
       });

       QObject::connect(&gaussianFilterCheckbox, &QCheckBox::stateChanged, [&](int state) {
           if (captureStarted) {
               processFrame(label, grayscaleCheckbox, greenCheckbox, gaussianFilterCheckbox);
           }
       });

       QTimer timer;
       timer.setInterval(33); // 30 FPS
       QObject::connect(&timer, &QTimer::timeout, [&]() {
           if (captureStarted) {

               processFrame(label, grayscaleCheckbox, greenCheckbox, gaussianFilterCheckbox);
           }
       });
       timer.start();

       return app.exec();
}
