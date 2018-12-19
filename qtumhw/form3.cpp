#include "form3.h"
#include "ui_form3.h"

 #include <QPainter>


Form3::Form3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form3)
{
    ui->setupUi(this);

    connect(ui->digitButton_1, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_2, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_3, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_4, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_5, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_6, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_7, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_8, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_9, SIGNAL(clicked()),this, SLOT(digitClicked()));
    connect(ui->digitButton_10, SIGNAL(clicked()),this, SLOT(digitClicked()));

    connect(ui->digitButton_point, SIGNAL(clicked()),this, SLOT(pointClicked()));
    connect(ui->digitButton_clear, SIGNAL(clicked()),this, SLOT(clear()));

}

Form3::~Form3()
{
    delete ui;
}

/******************************************************************************/

void Form3::digitClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());

    int digitValue = clickedButton->text().toInt();
    if (ui->display->text() == "0" && digitValue == 0.0)
        return;

    ui->display->setText(ui->display->text() + QString::number(digitValue));

}

void Form3::pointClicked()
{
    if (!ui-> display->text().contains("."))
        ui-> display->setText(ui->display->text() + tr("."));
}

void Form3::clear()
{
   ui-> display->setText("");
}

void Form3::on_pushButton_exit_clicked()
{
    emit subClosedSig();
    this->close();
}
/******************************************************************************/

void Form3::on_pushButton_clicked()
{
    emit getQrSig();

}

void Form3::on_pushButton_2_clicked()
{

}

void Form3::on_pushButton_3_clicked()
{

}



