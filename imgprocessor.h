#ifndef IMGPROCESSOR_H
#define IMGPROCESSOR_H

#include "showwidget.h"
//#include "finddialog.h"
#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QImage>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QSaveFile>
#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QSpinBox>
#include <QToolBar>
#include <QToolButton>
#include <QFontComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextCursor>


class ImgProcessor : public QMainWindow
{
    Q_OBJECT

public:
    ImgProcessor(QWidget *parent = 0);
    ~ImgProcessor();
    void createActions();                      //创建动作
    void createMenus();                        //创建菜单
    void createToolBars();                     //创建工具栏
    void loadFile(QString  filename);          //加载文件
    void mergeFormat(QTextCharFormat);         //合并格式
private:
    //各项菜单栏
    QMenu *fileMenu;                           //文件菜单
    QMenu *zoomMenu;                           //缩放菜单
    QMenu *rotateMenu;                         //旋转
    QMenu *mirrorMenu;                         //镜像
    //放置图片
    QImage img;

    QString fileName;                          //打开文件名
    QString curFileName;                       //保存文件名
    //显示中心部件
    ShowWidget *showWidget;

    //查找对话框
//    findDialog *find_dialog;
    QString str;
    QLabel *lineLabel;
    QLineEdit *lineEdit;
    QPushButton *findNextBtn;
    QPushButton *cancelBtn;

    QLabel *directLabel;

    QRadioButton *upBtn;
    QRadioButton *downBtn;

    QCheckBox *lowOrUpperBox;

    //替换对话框
    QString str2;
    QString str3;
    QLabel *lineLabel21;           //查找
    QLineEdit *lineEdit21;          //查找内容
    QLabel *lineLabel22;           //替换
    QLineEdit *lineEdit22;          //替换内容
    QPushButton *findNextBtn2;     //查找下一个
    QPushButton *subBtn2;          //替换按钮
    QPushButton *cancelBtn2;

    QLabel *directLabel2;
    QCheckBox *lowOrUpperBox2;
    bool findjudge;


    //文件菜单项
    QAction *openFileAction;                    //打开文件动作
    QAction *NewFileAction;                     //新建文件
    QAction *SaveAction;                        //保存文件
    QAction *SaveAsAction;                      //另存为文件
    QAction *PrintTextAction;                   //打印文本动作
    QAction *PrintImageAction;                  //打印图片
    QAction *exitAction;                        //退出动作

    //编辑菜单项
    QAction *copyAction;                        //复制动作
    QAction *cutAction;                         //剪贴
    QAction *pasteAction;                       //粘贴
    QAction *findAction;                        //查找
    QAction *subAction;                         //替换
    QAction *aboutAction;                       //关于
    QAction *zoomInAction;                      //放大
    QAction *zoomOutAction;                     //缩小

    //旋转菜单栏--图像处理
    QAction *rotate90Action;                    //90°
    QAction *rotate180Action;                   //180°
    QAction *rotate270Action;                   //270°

    //镜像菜单栏
    QAction *mirrorVerticalAction;              //垂直镜像
    QAction *mirrorHorizonAction;               //水平镜像
    QAction *undoAction;                        //撤销
    QAction *redoAction;                        //取消撤销

    //工具栏
    QToolBar *fileTool;                         //文件-打开、新建、保存，另存为,打印文本(图像)
    QToolBar *zoomTool;                         //缩放
    QToolBar *rotateTool;                       //旋转
    QToolBar *mirrorTool;                       //镜像
    QToolBar *doToolBar;                        //撤销、回撤--操作

    //文本编辑
    QLabel *fontLabel;                          //字体设置项
    QFontComboBox *fontComboBox;                //字体选框
    QLabel *fontLabel2;                         //大小
    QComboBox *sizeComboBox;                    //大小选框
    QToolButton *boldBtn;                       //加粗按钮
    QToolButton *italicBtn;                     //斜体
    QToolButton *underlineBtn;                  //下划线
    QToolButton *colorBtn;                      //字体颜色
    QToolButton *colorBackBtn;                  //背景颜色按钮
    QToolBar *fontToolBar;                      //字体工具栏

    //排序设置项
    QLabel *listLabel;
    QComboBox *listComboBox;
    QActionGroup *actGrp;
    QAction *leftAction;                        //居左
    QAction *rightAction;                       //局右
    QAction *centerAction;                      //居中
    QAction *justifyAction;                     //两端对齐
    QToolBar *listToolBar;                      //排序工具栏

protected slots:
    void ShowNewFile();                         //新建文件
    void ShowOpenFile();                        //打开文件
    void ShowSaveFile();                        //保存文件
    void ShowSaveAsFile();                      //另存为
    void ShowFindDialog();                      //查找对话框
    void ShowFindText();                        //查找
    void ShowFindStr();

    void ShowSubDialog();                       //替换对话框
    void ShowSubstitute();                      //替换
    void ShowSubText();
    void ShowSubText2();
    void ShowFindStr2 ();

    void ShowPrintText();                       //打印文本
    void ShowPrintImage();                      //打印图像
    void ShowZoomIn();                          //放大功能
    void ShowZoomOut();                         //缩小功能
    void ShowRotate90();                        //旋转90°
    void ShowRotate180();
    void ShowRotate270();
    void ShowMirrorVertical();                  //镜像--纵向镜像
    void ShowMirrorHorizontal();                //横向镜像
    void ShowFontComboBox(QString comboStr);    //显示字体框
    void ShowSizeSpinBox(QString spinValue);    //大小自旋盒
    void ShowBoldBtn();                         //加粗功能
    void ShowItalicBtn();                       //显示斜体
    void ShowUnderlineBtn();                    //下划线
    void ShowColorBtn();
    void ShowBackColorBtn();                    //背景颜色
    //字符格式化
    void ShowCurrentFormatChanged(const QTextCharFormat &fmt);

    void ShowList(int);                          //排序
    void ShowAlignment(QAction *act);            //对齐方式
    void ShowCursorPositionChanged();            //显示光标位置

};

#endif // IMGPROCESSOR_H






