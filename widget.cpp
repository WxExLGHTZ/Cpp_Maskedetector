#include "widget.h"
#include "ui_widget.h"
#include <string>
#include <QSerialPortInfo>



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    arduino_is_available = false;
    arduino_port_name ="";
    arduino = new QSerialPort;

    /*
    //find vendor and prudct id (company / arduino type)  needed if you will use other MC
    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
     qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
     if(serialPortInfo.hasVendorIdentifier())
     {
         qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
     }
     qDebug() << "Has Product ID: " << serialPortInfo.hasProductIdentifier();
     if(serialPortInfo.hasProductIdentifier())
     {
         qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
     }
    }*/

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
        {
            if(serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id)
            {
                if(serialPortInfo.productIdentifier()== arduino_unu_product_id)
                {
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                }
            }
        }
    }

    if(arduino_is_available)
    {
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::WriteOnly);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneAndHalfStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
    }
    else
    {
        qDebug() << "Port error: Arduino not found";
    }

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(startstopBtnPressed()));
    connect(this,SIGNAL(updateNow(std::string)),this,SLOT(updateLabel(std::string)));
    connect(this,SIGNAL(sendToArduino(std::string)),this,SLOT(updateArduino(std::string)));
    ui->graphicsView->setScene(new QGraphicsScene(this));


}

Widget::~Widget()
{
    if(arduino->isOpen())
    {
        arduino->close();
    }
    delete ui;

}

void Widget::startstopBtnPressed()
{

    ui->graphicsView->scene()->addItem(&pixmap);

    if(cap.isOpened())
    {
        ui->pushButton->setText("Start");
        ui->graphicsView->scene()->removeItem(&pixmap);
        ui->label->setText("");
        cap.release();
        return;
    }

    ui->pushButton->setText("Stop");

    cap.open(0);


    cv::Mat frame;

    QFile xml(":/cascades/myhaar_maskdetection.xml");

    if (xml.open(QFile::ReadOnly | QFile::Text))
    {
        QTemporaryFile temp;
        if (temp.open())
        {
            temp.write(xml.readAll());
            temp.close();
            if (maskCascade.load(temp.fileName().toStdString()))
            {
                qDebug() << "Successfully loaded classifier!";
            }
            else
            {
                qDebug() << "Could not load classifier.";
            }
        }
        else
        {
            qDebug() << "Can't open temp file.";
        }
    }
    else
    {
        qDebug() << "Can't open XML.";
    }



    while(cap.isOpened())
    {
        cap >> frame;
        if(!frame.empty())
        {
            cv::Mat grey_image;
            cv::cvtColor(frame, grey_image,cv::COLOR_BGR2GRAY);
            cv::equalizeHist(grey_image, grey_image);

            std::vector<cv::Rect> faces;

            maskCascade.detectMultiScale(grey_image, faces, 1.1, 2,  0|cv::CASCADE_SCALE_IMAGE,
                                         cv::Size(frame.cols/4, frame.rows/4));

            for( size_t i = 0; i < faces.size(); i++)
            {
                cv::rectangle(frame, faces[i], cv::Scalar( 255, 0, 0 ));

            }



            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);




            QImage qimg(frame.data,frame.cols,frame.rows,frame.step,QImage::Format_RGB888);

            pixmap.setPixmap( QPixmap::fromImage(qimg) );
            ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);




            if(!faces.empty())
            {
               emit updateNow("g");

               emit sendToArduino("g");
            }
            else
            {
               emit updateNow("r");

               emit sendToArduino("r");
            }



        }
        qApp->processEvents();
    }

    ui->pushButton->setText("Start");
}

void Widget::updateLabel(std::string out)
{
    if(out =="r")
    {
        ui->label->setText("        Keine Maske vorhanden");
        ui->label->setStyleSheet("color: #f72247");

    }
    else if(out =="g")
    {
        ui->label->setText("        Maske vorhanden");
        ui->label->setStyleSheet("color: #47f722");
    }
}

void Widget::updateArduino(std::string value)
{
    if(value == "g")
    {
        Widget::updateRGB(QString("g%1"));
    }
    else
    {
        Widget::updateRGB(QString("r%1"));
    }

}

void Widget::updateRGB(QString command)
{
    if(arduino->isWritable())
    {
        arduino->write(command.toStdString().c_str());
    }
    else
    {
        qDebug() << "Could not write to serial";
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    if(cap.isOpened())
    {
       cap.release();

    }

}
