#include "showwidget.h"
#include <QHBoxLayout>

ShowWidget::ShowWidget(QWidget *parent) : QWidget(parent)
{
    imageLabel = new QLabel;
    //使图片放置合适位置
    imageLabel->setScaledContents (true);

    text = new QTextEdit;

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    mainLayout->addWidget (imageLabel);
    mainLayout->addWidget (text);
}
