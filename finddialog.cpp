#include "finddialog.h"

findDialog::findDialog(QWidget *parent)
    :QDialog(parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);

    lineLabel = new QLabel(tr("查找内容(N): "));
    lineEdit = new QLineEdit;

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget (lineLabel);
    inputLayout->addWidget (lineEdit);

    findNextBtn = new QPushButton(tr("查找下一个"));
    cancelBtn = new QPushButton(tr("取消"));
    connect (cancelBtn, SIGNAL(clicked(bool)), this, SLOT(reject()));

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget (findNextBtn);
    buttonLayout->addWidget (cancelBtn);

    directLabel = new QLabel(tr("方向"));
    upBtn = new QRadioButton(tr("向上"));


    upBtn->setChecked (true);
    findNextBtn->setEnabled (false);

    downBtn = new QRadioButton(tr("向下"));

    lowOrUpperBox = new QCheckBox(tr("区分大小写(C)"));

    mainLayout->addLayout (inputLayout, 0, 0, 1, 3);
    mainLayout->addLayout (buttonLayout, 0, 4, 2, 1);
    mainLayout->addWidget (lowOrUpperBox, 2, 0);
    mainLayout->addWidget (directLabel, 1, 1);
    mainLayout->addWidget (upBtn, 2, 1);
    mainLayout->addWidget (downBtn, 2, 2);

    connect (lineEdit, SIGNAL(textChanged(QString)), this, SLOT(findButtonState()));
    connect (lowOrUpperBox, SIGNAL(clicked(bool)), this, SLOT(findBoxState()));
    connect (findNextBtn, SIGNAL(clicked(bool)), this, SLOT(findText()));

}


void findDialog::findButtonState ()
{
    if (lineEdit->text ().isEmpty ())
        findNextBtn->setEnabled (false);
    else
        findNextBtn->setEnabled (true);
}








