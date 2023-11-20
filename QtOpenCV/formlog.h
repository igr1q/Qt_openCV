#ifndef FORMLOG_H
#define FORMLOG_H

#include <QWidget>
#include<QStringListModel>
#include <QCoreApplication>
#include<QStringListModel>
#include<QListView>
#include<QVBoxLayout>
#include <QSizePolicy>

namespace Ui {
class FormLog;
}

class FormLog : public QWidget
{
    Q_OBJECT

public:
    explicit FormLog(QWidget *parent = nullptr);
    ~FormLog();
public slots:
    void updateLogList(const QStringList &logList);
 signals:
    void logListUpdated(const QStringList &logList);

private:
    Ui::FormLog *ui;
    QStringListModel *logModel;
    QStringList allLogs;
};

#endif // FORMLOG_H
