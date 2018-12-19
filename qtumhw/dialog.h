#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

private:
    Dialog(QWidget *parent = 0);
    ~Dialog();
public:
    static Dialog * GetInstance()
        {
            static Dialog *Instance;
            if(Instance == NULL) { //判断是否第一次调用
                Instance = new Dialog();
            }
            return Instance;
        }
private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
