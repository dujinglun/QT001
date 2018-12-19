#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QFont>
#include <QDebug>
#include <QMouseEvent>


#include "scp_multi_sig.h"


mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);

    pgVarible = new gVarible_t;
/**
 *数字键盘
*/
    connect(ui->digitButton_1, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_2, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_3, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_4, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_5, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_6, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_7, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_8, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_9, SIGNAL(pressed()),this, SLOT(digitClicked()));
    connect(ui->digitButton_10, SIGNAL(pressed()),this, SLOT(digitClicked()));

    connect(ui->digitButton_point, SIGNAL(clicked()),this, SLOT(pointClicked()));
    connect(ui->digitButton_clear, SIGNAL(clicked()),this, SLOT(clear()));
/**
 *退出
*/
    connect(ui->pushButton_exit, SIGNAL(clicked()),this, SLOT(on_pushButton_exit_clicked()));

/**
 * uart-Iriduim数据系统
*/
    subPage2 = new Form2();
    subPage2->setWindowFlags(Qt::FramelessWindowHint);
    connect(subPage2, SIGNAL(subClosedSig()),this, SLOT(reshow()));
    connect(subPage2, SIGNAL(openSubSig(int)),this, SLOT(subshow(int)));
/**
 * 保留
 */
    subPage3 = new Form3();
    subPage3->setWindowFlags(Qt::FramelessWindowHint);
    connect(subPage3, SIGNAL(subClosedSig()),this, SLOT(reshow()));



    subPage4 = new Form4();
    subPage4->setWindowFlags(Qt::FramelessWindowHint);
    connect(subPage4, SIGNAL(subClosedSig()),this, SLOT(reshow()));
    connect(subPage4, SIGNAL(openSubSig(int)),this, SLOT(subshow(int)));

    subPage5 = new Form5();
    subPage5->setWindowFlags(Qt::FramelessWindowHint);
    connect(subPage5, SIGNAL(subClosedSig()),this, SLOT(reshow()));
    connect(subPage5, SIGNAL(openSubSig(int)),this, SLOT(subshow(int)));


    tickTimer = new QTimer(this);

    tickTimer->start(1000); /* tickInc */
    connect(tickTimer,SIGNAL(timeout()),this, SLOT(tickSlot()));




}

mainWidget::~mainWidget()
{
    delete ui;
}

/******************************************************************************/

void mainWidget::digitClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());

    int digitValue = clickedButton->text().toInt();

    if (ui->display->text() == "0" && clickedButton->text() != ".")
        return;

    ui->display->setText(ui->display->text() + QString::number(digitValue));
}

void mainWidget::pointClicked()
{
    if (!ui-> display->text().contains("."))
        ui-> display->setText(ui->display->text() + tr("."));
}

void mainWidget::clear()
{
    ui-> display->setText("");
}

void mainWidget::on_pushButton_exit_clicked()
{
    this->close();
}
/******************************************************************************/

void mainWidget::reshow()
{
    this->show();
}
void mainWidget::subshow(int formNum)
{
    switch(formNum){
    case 1:
        //subPage1->show();
        break;
    case 2:
        subPage2->show();
        break;
    case 3:
        subPage3->show();
        break;
    case 4:
        subPage4->show();
        break;
    case 5:
        subPage5->show();
        break;
    default:
        break;

    }

}
/******************************************************************************/


void mainWidget::on_pushButton_UART_clicked()
{
    this->hide();
    subPage2->show();
}

void mainWidget::on_pushButton1_3_clicked()
{
    this->hide();
    subPage5->show();
}


/******************************************************************************/

void mainWidget::on_pushButton1_4_clicked()
{
    pgVarible->SendBufPayto += pgVarible->strQrcode.mid(0,34);
    pgVarible->SendBufPayto += " ";
    pgVarible->SendBufPayto += ui->display->text();

    if(pgVarible->strQrcode.size() == 0 ){
        return;
    }
    if(ui->display->text().size() == 0 ){
        ui->display->setText("token!") ;
        return;
    }

    this->hide();
    subPage4->show();

}

/******************************************************************************/



/******************************************************************************/
/**
 * @brief mainWidget::on_pushButton1_5_clicked
 * dump debug data
 */
//void mainWidget::on_pushButton1_5_clicked()
//{
//    //  QString tempStr = "45505446ff00010000000231f1adacc594b84ed361b809b07dd4b80049452c10e95c43d77a7b79cae9f602010000005701ff4c53ff043587cf03ffa3766180000000bbab7fb9aed383a971a73f9d54c317bc2afed604bb02fa9539199e5b30d703e303f6c2ad5aba74d702f82859ce4e87b463e59d84929a719cabe44ce8992a1ebcbf01000100fdffffff3166fd8043596d9338ebc9c805d287ddd776b9f3d921dc9c8537a246d426f922000000005701ff4c53ff043587cf03ffa3766180000000bbab7fb9aed383a971a73f9d54c317bc2afed604bb02fa9539199e5b30d703e303f6c2ad5aba74d702f82859ce4e87b463e59d84929a719cabe44ce8992a1ebcbf00000100fdffffff0200ab9041000000001976a9149ee8d1a45a323b6082608a423908071c3e8e9e0788ac503d9d2f000000001976a914ae2faf18493162af3a266708b7e3ed48e7f3278988acd8b00300";
//    QString tempStr = "45505446ff000100000004e6c715bad274f94514e99a8049ceb04236afa5715bb082856d160826e1ac1cd1010000005701ff4c53ff043587cf03ffa3766180000000bbab7fb9aed383a971a73f9d54c317bc2afed604bb02fa9539199e5b30d703e303f6c2ad5aba74d702f82859ce4e87b463e59d84929a719cabe44ce8992a1ebcbf01000000fdffffffaa0d562379a0ecd6aee418c19194ebb3ea3ea666f76c8c11aba9acd4044ffd68000000005701ff4c53ff043587cf03ffa3766180000000bbab7fb9aed383a971a73f9d54c317bc2afed604bb02fa9539199e5b30d703e303f6c2ad5aba74d702f82859ce4e87b463e59d84929a719cabe44ce8992a1ebcbf00000000fdffffff31f1adacc594b84ed361b809b07dd4b80049452c10e95c43d77a7b79cae9f602010000005701ff4c53ff043587cf03ffa3766180000000bbab7fb9aed383a971a73f9d54c317bc2afed604bb02fa9539199e5b30d703e303f6c2ad5aba74d702f82859ce4e87b463e59d84929a719cabe44ce8992a1ebcbf01000100fdffffff3166fd8043596d9338ebc9c805d287ddd776b9f3d921dc9c8537a246d426f922000000005701ff4c53ff043587cf03ffa3766180000000bbab7fb9aed383a971a73f9d54c317bc2afed604bb02fa9539199e5b30d703e303f6c2ad5aba74d702f82859ce4e87b463e59d84929a719cabe44ce8992a1ebcbf00000100fdffffff0200bbeea0000000001976a9149ee8d1a45a323b6082608a423908071c3e8e9e0788ac20ddd905000000001976a914ae2faf18493162af3a266708b7e3ed48e7f3278988acb5b10300";
//    short tempSize;
//    tempSize = (short)tempStr.length();

//    StringToHex(tempStr, pgVarible->rcvUsignedTransEmai);

//    pgVarible->rcvUsignedTransEmai.prepend( (unsigned char)(tempSize&0x00ff));  /*index 2*/
//    pgVarible->rcvUsignedTransEmai.prepend( (unsigned char)(tempSize>>8));  /*index 1*/

//    pgVarible->rcvUsignedTransEmai.prepend('U');                            /* index 0*/

//    /**
//     *
//     */
//    pgVarible->strQrcode = "qY3cpt8AfinTitzVHVGpuz719pinEtSehM";

//    ui->display->setText("27");

//}


void mainWidget::on_digitButton_7_clicked()
{

}


#define  QTUM_START_DELAY   15
//#define  CMD_QTUMD_START     "/root/qtum/qtumd  -testnet -daemon  "
void mainWidget::tickSlot()
{
    static int startCnt;
    QByteArray ba;

    startCnt += 1;

    QString lineState = QString("Starting,Wait %1 s Please").arg(QTUM_START_DELAY - startCnt);

    if(startCnt == QTUM_START_DELAY){
        QProcess  process;
        process.execute( CMD_QTUMD_START);
        process.waitForFinished();
        ba = process.readAllStandardOutput();

        process.close();

        qDebug() << ba;

        tickTimer->stop();

        ui->lineEditState->setText(ba);
    }else{
        ui->lineEditState->setText(lineState);
    }
}


