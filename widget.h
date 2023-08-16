#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPixmap>
#include <QCloseEvent>
#include <QDir>
#include <QTemporaryFile>

#include <QtSerialPort>
#include <QDebug>


#include <opencv2/opencv.hpp>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void updateNow(std::string in);

    //void sendToArduino(char value);

    void sendToArduino(std::string value);

private slots:
    void startstopBtnPressed();
    void updateLabel(std::string out);

    void updateArduino(std::string value);
    void updateRGB(QString);

private:
    Ui::Widget *ui;

    QGraphicsPixmapItem pixmap;
    cv::VideoCapture cap;
    cv::CascadeClassifier maskCascade;

    QSerialPort *arduino;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_unu_product_id = 67;
    QString arduino_port_name;
    bool arduino_is_available;

};
#endif // WIDGET_H
