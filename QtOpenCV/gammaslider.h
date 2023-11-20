#ifndef GAMMASLIDER_H
#define GAMMASLIDER_H

#include <QWidget>

namespace Ui {
class GammaSlider;
}

class GammaSlider : public QWidget
{
    Q_OBJECT
signals:
    void gammaValueChanged(int value);
public:
    explicit GammaSlider(QWidget *parent = nullptr);
    ~GammaSlider();

private slots:
    void on_horizontalSlider_sliderMoved(int position);

private:
    Ui::GammaSlider *ui;
};

#endif // GAMMASLIDER_H
