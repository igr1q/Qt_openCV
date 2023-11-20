#include "gammaslider.h"
#include "ui_gammaslider.h"

GammaSlider::GammaSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GammaSlider)
{
    ui->setupUi(this);
}

GammaSlider::~GammaSlider()
{
    delete ui;
}

void GammaSlider::on_horizontalSlider_sliderMoved(int position)
{
        emit gammaValueChanged(position);
}
