#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLabel>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/video/video.hpp>
#include<opencv2/objdetect/objdetect.hpp>
#include<QTimer>
#include<QDebug>
#include <QCoreApplication>
#include<QDir>
#include<QFile>
#include<QStringListModel>
#include<QListView>
#include <QDateTime>
#include <QTextStream>
#include<formlog.h>
#include<QDialog>
#include<gammaslider.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_CamControll_clicked();

    void on_actionGause_triggered();

    void on_actionWhiteNBlack_triggered();

    void on_actionGreen_triggered();

    void on_actionFaceRecognition_triggered();

    void on_actionShowLog_triggered();

    void on_actionhsv_triggered();

    void on_actionycrcb_triggered();

    void on_action_triggered();
    void updateGammaValue(int gammaValue);

private:
    Ui::MainWindow *ui;
    bool
        applyGaussianFilter,
        applyWhiteNBlackFilter,
        applyGreenFilter,
        applyDetectedFace,
        applyhsv,
        applyYCrCb,
        applyGammaCorrection;
    QString faceCascadePath;
    QStringListModel *logModel;
    QString logFolderPath;
    FormLog formlog;
    GammaSlider gammaSliderWindow;
    double gamma;
public:
    cv::VideoCapture cap;
    cv::Mat frame;
    cv::Mat hsvFrame;
    cv::Mat ycrcbFrame;
    bool captureStarted = false;
    void applyFilters();
    void updateFrame();
    void detectFaces();
    void readLogsFromFile(QStringList &logList);
    void writeLogToFile(const QString &logEntry);
    void logAction(const QString &action, bool enabled);
};
#endif // MAINWINDOW_H
