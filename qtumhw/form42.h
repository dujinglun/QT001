#ifndef FORM42_H
#define FORM42_H

#include <QWidget>

namespace Ui {
class Form42;
}

class Form42 : public QWidget
{
    Q_OBJECT
    
private:
      Form42(QWidget *parent = 0);
    ~Form42();
public:
    static Form42 * GetInstance()
        {
            static Form42 *Instance;
            if(Instance == NULL) { //判断是否第一次调用
                Instance = new Form42();
            }
            return Instance;
        }


    
private:
    Ui::Form42 *ui;
};





#endif // FORM42_H
