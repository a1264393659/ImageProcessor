#include "imgprocessor.h"
#include <QApplication>
#include <QTextCodec>         //使能够输出中文

int main(int argc, char *argv[])
{
    //此语句一定要加上去，qt5中取消了QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk")
    //来设置中文字体编码
    //在读取文件时，才能不会读取中文为乱码
    QTextCodec::setCodecForLocale (QTextCodec::codecForName ("UTF8"));

    QApplication a(argc, argv);
    QFont f("ZYSong18030", 12);
    a.setFont (f);

    ImgProcessor w;
    w.show();

    return a.exec();
}
