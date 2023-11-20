#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      applyGaussianFilter(false),
      applyWhiteNBlackFilter(false),
      applyGreenFilter(false),
      applyDetectedFace(false),
      applyhsv(false),
      applyYCrCb(false),
      applyGammaCorrection(false),
      logModel(new QStringListModel(this)),
      logFolderPath(QDir(QCoreApplication::applicationDirPath()).filePath("logs"))
{
    ui->setupUi(this);
    captureStarted = false;
    // Создание папки для хранения логов
    QDir().mkpath(logFolderPath);
    // Создание файл лога, если его нет
    QString logFilePath = QDir(logFolderPath).filePath("log.txt");
    QFile file(logFilePath);
    if (!file.exists()) {
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    // Устанавливаем масштабирование содержимого для QLabel
    ui->cam->setScaledContents(true);
    // Получение пути к приложению
    QString appPath = QCoreApplication::applicationDirPath();

    // Сформируем абсолютный путь к файлу каскада лица
    faceCascadePath = QDir(appPath).filePath("data/haarcascade_frontalface_default.xml");
    // Подключаем слот для обработки таймера
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(33); // 30 FPS

    // Установка политики размеров для основного виджета
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(ui->verticalLayout); // Установка QVBoxLayout как основного макета
    setCentralWidget(centralWidget);

    QSizePolicy policy = sizePolicy();
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    setSizePolicy(policy);
    setMinimumSize(600, 600);

    connect(&gammaSliderWindow, &GammaSlider::gammaValueChanged, this, &MainWindow::updateGammaValue);
}

MainWindow::~MainWindow()
{
    if (cap.isOpened()) {
        cap.release();
    }
    delete ui;
}

void MainWindow::on_CamControll_clicked()
{
    QString action;
    if (captureStarted) {
        cap.release();
        ui->CamControll->setText("Start");
        ui->cam->clear();
        action = "Camera stopped";
    } else {
        cap.open(0); // Открыть камеру по умолчанию
        if (!cap.isOpened()) {
            qDebug() << "Error: Camera not opened";
            return;
        }
        ui->CamControll->setText("Stop");
        action = "Camera started";
    }
    captureStarted = !captureStarted;
    // Залогировать действие
    logAction(action, captureStarted);
}

void MainWindow::updateFrame()
{
    if (captureStarted) {
        if (cap.isOpened()) {
            cap.read(frame);
            if (!frame.empty()) {
                applyFilters();
                cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
                QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
                qimg = qimg.copy();
                QPixmap pixmap = QPixmap::fromImage(qimg);
                ui->cam->setPixmap(pixmap);

                QCoreApplication::processEvents();
            }
        }
    }
}



void MainWindow::detectFaces()
{
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load(faceCascadePath.toStdString())) {
        qDebug() << "Error: Unable to load face cascade classifier.Path: " << faceCascadePath;
        return;
    }

    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(frame, faces, 1.05, 9, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(50, 50));

    // Рисуем прямоугольники вокруг обнаруженных лиц
    for (const cv::Rect& face : faces) {
        cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2);
    }
}


void MainWindow::applyFilters()
{
    if (applyWhiteNBlackFilter) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
    }

    if (applyGreenFilter) {
        cv::Mat greenFilter(frame.size(), CV_8UC3, cv::Scalar(0, 255, 0));
        cv::bitwise_and(frame, greenFilter, frame);
    }

    if (applyGaussianFilter) {
        cv::GaussianBlur(frame, frame, cv::Size(0, 0), 3);
    }

    if(applyDetectedFace)
    {
        detectFaces();
    }

    if(applyhsv)
    {
        // Применяем преобразование HSV
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        // Разделяем изображение на каналы
        std::vector<cv::Mat> channels;
        cv::split(hsvFrame, channels);

        // Создаем матрицу для объединенного изображения
        cv::Mat displayImage(frame.rows * 2, frame.cols * 2, CV_8UC3);

        // Копируем изображение в верхнюю левую ячейку
        frame.copyTo(displayImage(cv::Rect(0, 0, frame.cols, frame.rows)));

        // Копируем и конвертируем каналы HSV в оставшиеся ячейки
        cv::Mat hueChannel, satChannel, valChannel;
        cv::cvtColor(channels[0], hueChannel, cv::COLOR_GRAY2BGR);
        cv::cvtColor(channels[1], satChannel, cv::COLOR_GRAY2BGR);
        cv::cvtColor(channels[2], valChannel, cv::COLOR_GRAY2BGR);

        hueChannel.copyTo(displayImage(cv::Rect(frame.cols, 0, frame.cols, frame.rows)));
        satChannel.copyTo(displayImage(cv::Rect(0, frame.rows, frame.cols, frame.rows)));
        valChannel.copyTo(displayImage(cv::Rect(frame.cols, frame.rows, frame.cols, frame.rows)));
        displayImage.copyTo(frame);
    }

    if (applyYCrCb)
    {
        // Применяем преобразование YCrCb
        cv::cvtColor(frame, ycrcbFrame, cv::COLOR_BGR2YCrCb);

        // Разделяем изображение на каналы
        std::vector<cv::Mat> channels;
        cv::split(ycrcbFrame, channels);

        // Создаем матрицу для объединенного изображения
        cv::Mat displayImage(frame.rows * 2, frame.cols * 2, CV_8UC3);

        // Копируем изображение в верхнюю левую ячейку
        frame.copyTo(displayImage(cv::Rect(0, 0, frame.cols, frame.rows)));

        // Копируем и конвертируем каналы YCrCb в оставшиеся ячейки
        cv::Mat yChannel, crChannel, cbChannel;
        cv::cvtColor(channels[0], yChannel, cv::COLOR_GRAY2BGR);
        cv::cvtColor(channels[1], crChannel, cv::COLOR_GRAY2BGR);
        cv::cvtColor(channels[2], cbChannel, cv::COLOR_GRAY2BGR);

        yChannel.copyTo(displayImage(cv::Rect(frame.cols, 0, frame.cols, frame.rows)));
        crChannel.copyTo(displayImage(cv::Rect(0, frame.rows, frame.cols, frame.rows)));
        cbChannel.copyTo(displayImage(cv::Rect(frame.cols, frame.rows, frame.cols, frame.rows)));
        displayImage.copyTo(frame);
    }
    if(applyGammaCorrection)
    {
        cv::Mat gammaCorrectedFrame;
        cv::Mat gammaCorrectionMat = cv::Mat::zeros(1, 256, CV_8U);

        for (int i = 0; i < 256; ++i)
        {
            gammaCorrectionMat.at<uchar>(i) = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
        }

        cv::LUT(frame, gammaCorrectionMat, gammaCorrectedFrame);
        gammaCorrectedFrame.copyTo(frame);
    }
}


void MainWindow::updateGammaValue(int gammaValue)
{
    gamma = gammaValue / 100.0;
    qDebug() << "Gamma Value changed to: " << gamma;
}

void MainWindow::on_actionGause_triggered()
{
    applyGaussianFilter = !applyGaussianFilter;
    logAction("Gaussian Filter", applyGaussianFilter);

}

void MainWindow::on_actionWhiteNBlack_triggered()
{
    applyWhiteNBlackFilter = !applyWhiteNBlackFilter;
    logAction("White and Black Filter", applyWhiteNBlackFilter);
}



void MainWindow::on_actionGreen_triggered()
{
    applyGreenFilter = !applyGreenFilter;
    logAction("Green Filter", applyGreenFilter);
}

void MainWindow::on_actionFaceRecognition_triggered()
{
    applyDetectedFace =! applyDetectedFace;
    logAction("Face Recognition", applyDetectedFace);
}

void MainWindow::on_actionShowLog_triggered()
{
    if(!formlog.isVisible())
    {
        QStringList logList;
        readLogsFromFile(logList);
        formlog.updateLogList(logList);
    }
    formlog.show();
}


void MainWindow::readLogsFromFile(QStringList &logList)
{
    // Создание файла лога, если его нет
    QString logFilePath = QDir(logFolderPath).filePath("log.txt");
    QFile file(logFilePath);
    if (!file.exists()) {
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    // Чтение логов из файла и обновление модели
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            logList.append(in.readLine());
        }
        file.close();
    }
}

void MainWindow::logAction(const QString &action, bool enabled)
{
    // Запись действие в лог с меткой времени
    QString logEntry = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ": ";
    logEntry += action + " " + (enabled ? "enabled" : "disabled");

    // Добавление запись в модель
    QStringList logList = logModel->stringList();
    logList.append(logEntry);
    logModel->setStringList(logList);
    // Записать в файл
    writeLogToFile(logEntry);

    // Отправить сигнал обновления лога в FormLog
    formlog.emit logListUpdated(logList);
}

void MainWindow::writeLogToFile(const QString &logEntry)
{
    // Запись лог в файл в папке для логов
    QString logFilePath = QDir(logFolderPath).filePath("log.txt");
    QFile file(logFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << logEntry << "\n";
        file.close();
    }
}


void MainWindow::on_actionhsv_triggered()
{
    applyhsv =!applyhsv;
    logAction("HSV Filter", applyhsv);
}

void MainWindow::on_actionycrcb_triggered()
{
    applyYCrCb = !applyYCrCb;
    logAction("YCrCb Filter",applyYCrCb);
}

void MainWindow::on_action_triggered()
{
    applyGammaCorrection =!applyGammaCorrection;
    logAction("GammaCorrection ",applyGammaCorrection);
    if (applyGammaCorrection)
    {
        gammaSliderWindow.show();
    }
    else
    {
        gammaSliderWindow.close();
    }

}
