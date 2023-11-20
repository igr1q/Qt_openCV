#include "formlog.h"
#include "ui_formlog.h"

FormLog::FormLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLog),
    logModel(new QStringListModel(this))
{
    ui->setupUi(this);
    QSizePolicy policy = sizePolicy();
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    setSizePolicy(policy);
    setMinimumSize(600, 400);
    setMaximumSize(800,600);
    ui->listView->setModel(logModel);
    connect(this, &FormLog::logListUpdated, this, &FormLog::updateLogList);
}

FormLog::~FormLog()
{
    delete ui;
}

void FormLog::updateLogList(const QStringList &logList)
{
    logModel->setStringList(logList);
}
