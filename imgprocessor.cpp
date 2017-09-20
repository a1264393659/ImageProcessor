#include "imgprocessor.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QColor>
#include <QColorDialog>
#include <QTextList>
#include <QMessageBox>
#include <QDebug>

ImgProcessor::ImgProcessor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle (tr("Easy Word"));         //设置窗体标题
    //创建放置图像QLabel和文本编辑器QTextEdit的QWidget对象showWidget,并将该QWidget对象设置
    //为中心部件
    showWidget = new ShowWidget(this);
    setCentralWidget (showWidget);

//    find_dialog = new findDialog(this);

    curFileName = tr("");                     //初始化文件名

    //排序
    listLabel = new QLabel(tr("排序"));
    listComboBox = new QComboBox;
    listComboBox->addItem ("Standard");
    listComboBox->addItem ("QTextListFormat::ListDisc");    //圆盘
    listComboBox->addItem ("QTextListFormat::ListCircle");  //圆
    listComboBox->addItem ("QTextListFormat::ListSquare");  //方形
    listComboBox->addItem ("QTextListFormat::ListDecimal"); //十进制
    listComboBox->addItem ("QTextListformat::ListLowerAlpha"); //小写字母
    listComboBox->addItem ("QTextListFormat::ListUpperAlpha"); //大写字母
    listComboBox->addItem ("QTextListformat::ListLowerRoman"); //小写罗马字符
    listComboBox->addItem ("QTextListFormat::ListUpperRoman"); //大写罗马字符

    //在工具栏上嵌入控件:字体，大小，粗体，斜体，字体颜色
    //字体--这里的字体，字号仅仅是下拉列表框改变
    fontLabel = new QLabel(tr("字体:"));
    fontComboBox = new QFontComboBox;
    //setFontFilter--接口过滤(只在下拉列表框中显示某一类字体,默认情况下为QFontComboBox::AllFonts
    //列出所有字体
    fontComboBox->setFontFilters (QFontComboBox::ScalableFonts);

    //字号
    fontLabel2 = new QLabel(tr("字号:"));
    sizeComboBox = new QComboBox;

    /**QFontDatabase-实现在字号下拉列表框中填充各种不同的字号条目
     * 其用于表示当前系统中所有可用的格式信息,主要是字体和字号大小
     * provide information about the fonts avaliable in the underlying(潜在) window system*/

    QFontDatabase db;

    //standardSize(): return a list of standard font size(返回可用标准字号的列表).
    foreach (int size, db.standardSizes ())
        sizeComboBox->addItem (QString::number (size)); //将它们插入到字号下拉框中

    //加粗
    boldBtn = new QToolButton;
    boldBtn->setIcon (QIcon("bold.png"));
    //设置成是否是开关(toggle)按钮(true)---即可复选的
    boldBtn->setCheckable (true);

    //斜体
    italicBtn = new QToolButton;
    italicBtn->setIcon (QIcon("italic.png"));
    italicBtn->setCheckable (true);

    //下划线
    underlineBtn = new QToolButton;
    underlineBtn->setIcon (QIcon("underline.png"));
    underlineBtn->setCheckable (true);

    //颜色
    colorBtn = new QToolButton;
    colorBtn->setIcon (QIcon("color.png"));
    colorBtn->setCheckable (true);

    //背景颜色
    colorBackBtn = new QToolButton;
    colorBackBtn->setIcon (QIcon("color.png"));
    colorBtn->setCheckable (true);

    /* 创建动作、菜单、工具栏函数 */
    createActions ();
    createMenus ();
    createToolBars ();
    if (img.load ("image.png"))
    {
        //在imageLabel对象放置图像
        showWidget->imageLabel->setPixmap (QPixmap::fromImage (img));
    }

    //连接槽函数
    connect (fontComboBox, SIGNAL(activated(QString)), this, SLOT(ShowFontComboBox(QString)));
    connect (sizeComboBox, SIGNAL(activated(QString)), this, SLOT(ShowSizeSpinBox(QString)));
    connect (boldBtn, SIGNAL(clicked(bool)), this, SLOT(ShowBoldBtn()));
    connect (italicBtn, SIGNAL(clicked(bool)), this, SLOT(ShowItalicBtn()));
    connect (underlineBtn, SIGNAL(clicked(bool)), this, SLOT(ShowUnderlineBtn()));
    connect (colorBtn, SIGNAL(clicked(bool)), this , SLOT(ShowColorBtn()));
    connect (colorBackBtn, SIGNAL(clicked(bool)),this, SLOT(ShowBackColorBtn()));

    //设置字符格式--之前的字符的字体，字号,仅仅是下拉列表框的改变, 还需要调用此函数改变文本字体
    //当光标所在字符格式发生变化时调用此槽函数，函数根据 新的字符格式 将 工具栏 各个格式的显示更新？？
    connect (showWidget->text, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
             this, SLOT(ShowCurrentFormatChanged(QTextCharFormat)));

    connect (listComboBox, SIGNAL(activated(int)), this, SLOT(ShowList(int)));
    //连接撤销信号和 rede按钮显示状况相连接
    connect (showWidget->text->document (), SIGNAL(undoAvailable(bool)),
             redoAction, SLOT(setEnabled(bool)));
    connect (showWidget->text->document (), SIGNAL(redoAvailable(bool)),
             redoAction, SLOT(setEnabled(bool)));
    //用于排版
    connect (showWidget->text, SIGNAL(cursorPositionChanged()),
             this, SLOT(ShowCursorPositionChanged()));
}

//动作(Action)的实现
void ImgProcessor::createActions ()
{

    //基于文件操作的动作(Action)的代码
    //“打开”动作
    //在创建“打开文件”动作的同时,指定了此动作使用的图标、名称及父窗口
    openFileAction = new QAction(QIcon("open.png"), tr("打开"), this);
    //设定此动作的组合键为【Ctrl+O】。
    openFileAction->setShortcut (tr("Ctrl+O"));
    //设定了状态条显示,当鼠标移至此动作对应的菜单条目或工具栏按钮上时,在状态条上显示"打开xxx"提示
    openFileAction->setStatusTip (tr("open a file"));
    connect (openFileAction, SIGNAL(triggered(bool)), this, SLOT(ShowOpenFile()));

    //“新建”动作
    NewFileAction = new QAction(QIcon("new.png"), tr("新建"), this);
    NewFileAction->setShortcut (tr("Ctrl+N"));
    NewFileAction->setStatusTip (tr("new a file"));
    connect (NewFileAction, SIGNAL(triggered(bool)), this, SLOT(ShowNewFile()));

    //“保存”动作
    SaveAction = new QAction(QIcon("save.png"), tr("保存"), this);
    SaveAction->setShortcut (tr("Ctrl+S"));
    SaveAction->setStatusTip (tr("save a file"));
    connect (SaveAction, SIGNAL(triggered(bool)), this, SLOT(ShowSaveFile()));

    //“另存为”动作
    SaveAsAction = new QAction(tr("Save as"), this);
    SaveAsAction->setShortcut (tr("Ctrl+alt+S"));
    SaveAsAction->setStatusTip (tr("Save as"));
    connect (SaveAsAction, SIGNAL(triggered(bool)), this, SLOT(ShowSaveAsFile()));

    //“退出”动作
    exitAction = new QAction(tr("exit"), this);
    exitAction->setShortcut (tr("Ctrl+Q"));
    exitAction->setStatusTip (tr("exit程序"));
    connect (exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

    //"复制"动作
    copyAction = new QAction(QIcon("copy.png"), tr("复制"), this);
    copyAction->setShortcut (tr("Ctrl+C"));
    copyAction->setStatusTip (tr("复制"));
    //连接引发信号 和 中心部件的文本编辑框 复制信号
    connect (copyAction, SIGNAL(triggered(bool)), showWidget->text, SLOT(copy()));

    //“剪切”信号
    cutAction = new QAction(QIcon("cut.png"), tr("剪贴"), this);
    cutAction->setShortcut (tr("Ctrl+X"));
    cutAction->setStatusTip (tr("剪切文件"));
    connect (cutAction, SIGNAL(triggered(bool)), showWidget->text, SLOT(cut()));

    //"粘贴"动作
    pasteAction = new QAction(QIcon("paste.png"), tr("粘贴"), this);
    pasteAction->setShortcut (tr("Ctrl+V"));
    pasteAction->setStatusTip (tr("粘贴文件"));
    connect (pasteAction, SIGNAL(triggered(bool)), showWidget->text, SLOT(paste()));

    //“查找”动作
//    findDlg = new QDialog(this);
//    findDlg->setWindowTitle(tr("查找"));
//    findLineEdit = new QLineEdit(findDlg);
//    QPushButton *btn= new QPushButton(tr("查找下一个"), findDlg);
//    QVBoxLayout *layout= new QVBoxLayout(findDlg);
//    layout->addWidget(findLineEdit);
//    layout->addWidget(btn);
//    connect(btn, SIGNAL(clicked()), this, SLOT(ShowFindDialog()));

    findAction = new QAction(QIcon("find.png"), tr("查找"), this);
    findAction->setShortcut (tr("Ctrl+f"));
    findAction->setStatusTip (tr("查找文件"));
    connect (findAction, SIGNAL(triggered(bool)), this, SLOT(ShowFindDialog()));

    subAction = new QAction(QIcon("sub.png"), tr("替换"), this);
    subAction->setShortcut (tr("Ctrl+H"));
    subAction->setStatusTip (tr("替换"));
    connect (subAction, SIGNAL(triggered(bool)), this, SLOT(ShowSubDialog()));

    //“关于"动作
    aboutAction = new QAction(tr("关于"), this);
    connect (aboutAction, SIGNAL(triggered(bool)), this, SLOT(QApplication::aboutQt ()));

    //实现打印文本和图像、图像缩放和镜像的动作(Action)的代码
    //“打印文本”动作
    PrintTextAction = new QAction(QIcon("printText.png"), tr("打印文本"), this);
    PrintTextAction->setStatusTip (tr("print a Text"));
    connect (PrintTextAction, SIGNAL(triggered(bool)), this, SLOT(ShowPrintText()));

    //“打印图像”动作
    PrintImageAction = new QAction(QIcon("printImage.png"), tr("打印图像"), this);
    PrintImageAction->setStatusTip (tr("print a Image"));
    connect (PrintImageAction, SIGNAL(triggered(bool)), this, SLOT(ShowPrintImage()));

    //“放大”动作
    zoomInAction = new QAction(QIcon("zoomin.png"), tr("放大"), this);
    zoomInAction->setStatusTip (tr("enlarge a image"));
    connect (zoomInAction, SIGNAL(triggered(bool)), this, SLOT(ShowZoomIn()));

    //"缩小"动作
    zoomOutAction = new QAction(QIcon("zoomout.png"), tr("缩小"), this);
    zoomOutAction->setStatusTip (tr("Reduce a image"));
    connect (zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(ShowZoomOut()));

    //实现图像旋转动作(Action)
    //旋转90°
    rotate90Action = new QAction(QIcon("rotate90.png"), tr("旋转 90°"), this);
    rotate90Action->setStatusTip (tr("将一幅图旋转 90°"));
    connect (rotate90Action, SIGNAL(triggered(bool)), this, SLOT(ShowRotate90()));

    //旋转180°
    rotate180Action = new QAction(QIcon("rotate180.png"), tr("旋转 180°"), this);
    rotate180Action->setStatusTip (tr("将一幅图旋转 180°"));
    connect (rotate180Action, SIGNAL(triggered(bool)), this, SLOT(ShowRotate180()));

    //旋转270°
    rotate270Action = new QAction(QIcon("rotate270.png"), tr("旋转 270°"), this);
    rotate270Action->setStatusTip (tr("将一幅图旋转 270°"));
    connect (rotate270Action, SIGNAL(triggered(bool)), this, SLOT(ShowRotate270()));

    //实现图像镜像的动作(Action)
    //纵向镜像
    mirrorVerticalAction = new QAction(QIcon("mirrorVertical.png"), tr("纵向镜像"), this);
    mirrorVerticalAction->setStatusTip (tr("对一幅图做纵镜像"));
    connect (mirrorVerticalAction, SIGNAL(triggered(bool)), this, SLOT(ShowMirrorVertical()));

    //横向镜像
    mirrorHorizonAction = new QAction(QIcon("mirrorHorizontal.png"), tr("横向镜像"), this);
    mirrorHorizonAction->setStatusTip (tr("对一幅图做横镜像"));
    connect (mirrorHorizonAction, SIGNAL(triggered(bool)), this, SLOT(ShowMirrorHorizontal()));

    //实现撤销和恢复的动作(Action)
    undoAction = new QAction(QIcon("undo.png"), "撤销", this);
    connect (undoAction, SIGNAL(triggered(bool)), showWidget->text, SLOT(undo()));
    redoAction = new QAction(QIcon("redo.png"), "重做", this);
    connect (redoAction, SIGNAL(triggered(bool)), showWidget->text, SLOT(redo()));

    //排序: 左对齐、右对齐、居中、两端对齐
    //setCheckable()只能是可选择的，或不能选择的.选中了，就不能取消选中状态(不可复选)
    //setChecked()可以是选中的,或者是非选中的(可复选)
    actGrp = new QActionGroup(this);
    leftAction = new QAction(QIcon("left.png"), "left", actGrp);
    leftAction->setCheckable (true);
    rightAction = new QAction(QIcon("right.png"), "right", actGrp);
    rightAction->setCheckable (true);
    centerAction = new QAction(QIcon("center.png"), "center", actGrp);
    centerAction->setCheckable (true);
    justifyAction = new QAction(QIcon("justify.png"), "justify", actGrp);
    justifyAction->setCheckable (true);
    //按下某个对齐按钮的响应,根据比较判断触发的是哪个对齐按钮
    connect (actGrp, SIGNAL(triggered(QAction*)), this, SLOT(ShowAlignment(QAction*)));

}

//实现了各个动作之后,需要将它们通过菜单，工具栏或快捷键的方式体现出来.
//菜单的实现
void ImgProcessor::createMenus ()
{
    //文件菜单
    //直接调用QMainWindow的menuBar()函数即可得到主窗口的菜单条指针,再调用菜单条
    //QMenuBar的addMenu()函数,即可完成在菜单条中插入一个新菜单fileMenu,fileMenu()函数为一个
    //QMenu类对象
    fileMenu = menuBar ()->addMenu (tr("文件"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(NewFileAction);
    fileMenu->addAction(SaveAction);
    fileMenu->addAction(SaveAsAction);
    fileMenu->addAction(PrintTextAction);
    fileMenu->addAction(PrintImageAction);
    fileMenu->addSeparator ();             //添加分割
    fileMenu->addAction(exitAction);
    //编辑菜单(缩放...)
    zoomMenu = menuBar ()->addMenu (tr("编辑"));
    zoomMenu->addAction(copyAction);
    zoomMenu->addAction(cutAction);
    zoomMenu->addAction(pasteAction);
    zoomMenu->addAction(findAction);
    zoomMenu->addAction(subAction);
    zoomMenu->addAction(aboutAction);
    zoomMenu->addSeparator ();
    zoomMenu->addAction(zoomInAction);
    zoomMenu->addAction(zoomOutAction);
    //旋转菜单
    rotateMenu = menuBar ()->addMenu (tr("旋转"));
    rotateMenu->addAction(rotate90Action);
    rotateMenu->addAction(rotate180Action);
    rotateMenu->addAction(rotate270Action);
    //镜像菜单
    mirrorMenu = menuBar ()->addMenu (tr("镜像"));
    mirrorMenu->addAction(mirrorVerticalAction);
    mirrorMenu->addAction(mirrorHorizonAction);
}

//工具栏的实现(ToolBars)的实现
void ImgProcessor::createToolBars ()
{
    //文件工具条
    //直接调用QMainWindow的addToolBar()函数即可获得主窗口的工具条对象,每新增一个工具条
    //调用一次addToolBar()函数,赋予不同的名称,即可在主窗口中新增一个工具条
    fileTool = addToolBar ("File");
    //调用QToolBar的addAction()函数在工具条中插入属于本工具条的动作.
    //这些工具条可以在工具栏单击右键,将弹出工具条显示的选择菜单
    fileTool->addAction(openFileAction);
    fileTool->addAction(NewFileAction);
    fileTool->addAction(SaveAction);
    fileTool->addAction(PrintTextAction);
    fileTool->addAction(PrintImageAction);
    //编辑工具条
    zoomTool = addToolBar ("Edit");
    zoomTool->addAction(copyAction);
    zoomTool->addAction(cutAction);
    zoomTool->addAction(pasteAction);
    zoomTool->addSeparator ();
    zoomTool->addAction(zoomInAction);
    zoomTool->addAction(zoomOutAction);
    //旋转工具条
    rotateTool = addToolBar ("rotate");
    rotateTool->addAction(rotate90Action);
    rotateTool->addAction(rotate180Action);
    rotateTool->addAction(rotate270Action);
    //撤销和重做工具条
    doToolBar = addToolBar ("doEdit");
    doToolBar->addAction(undoAction);
    doToolBar->addAction(redoAction);
    /***
     * 工具条是可移动窗口,停靠的区域由QToolBar的allowAreas决定
     * Qt::LeftToolBarArea、Qt::RightToolBarArea、Qt::TopToolBarArea、
     * Qt::BottomToolBarArea, Qt::AllToolBarAreas(默认)。
     * 可以通过调用setAllowAreas()函数来指定工具条可停靠的区域
     * 如:fileTool->setAllowAreas(Qt::TopToolBarArea|Qt::LeftToolBarArea);
     * 来限定fileTool只能出现在主窗口的顶部或者左侧.工具条也可通过setMovable()设定可移动性:
     * 如fileTool->setMovable (false)来指定文件工具条不可移动 */

    //字体工具条
    fontToolBar = addToolBar ("Font");
    fontToolBar->addWidget (fontLabel);
    fontToolBar->addWidget (fontComboBox);
    fontToolBar->addWidget (fontLabel2);
    fontToolBar->addWidget (sizeComboBox);
    fontToolBar->addSeparator ();
    fontToolBar->addWidget (boldBtn);
    fontToolBar->addWidget (italicBtn);
    fontToolBar->addWidget (underlineBtn);
    fontToolBar->addSeparator ();
    fontToolBar->addWidget (colorBtn);
    fontToolBar->addSeparator ();
    fontToolBar->addWidget (colorBackBtn);

    //排序工具条
    listToolBar = addToolBar ("list");
    listToolBar->addWidget (listLabel);
    listToolBar->addWidget (listComboBox);
    listToolBar->addSeparator ();
    listToolBar->addActions(actGrp->actions ());
}

//文件操作功能
//新建文件
void ImgProcessor::ShowNewFile ()
{
    ImgProcessor *newImgProcessor = new ImgProcessor;
    newImgProcessor->show ();
}

//打开文件
void ImgProcessor::ShowOpenFile ()
{
    fileName = QFileDialog::getOpenFileName(this, "打开");
    curFileName = fileName;
    if (!fileName.isEmpty ())       //文件名不为空
    {
        //原窗口内容为空，则直接加载该文件
        if (showWidget->text->document ()->isEmpty ())
        {
            loadFile (fileName);    //loadFile()函数实现在下方.
        }
        //否则，打开新窗口加载文件
        else
        {
            ImgProcessor *newImgProcessor = new ImgProcessor;
            newImgProcessor->show ();
            newImgProcessor->loadFile (fileName);
        }
    }
}

//loadFile()函数实现，利用QFile和QTextStream完成具体读取文件内容工作
void ImgProcessor::loadFile (QString filename)
{
    printf ("file name:%s\n", (char*)filename.data ());
    QFile file(filename);     //获取文件
    if (file.open (QIODevice::ReadOnly | QIODevice::Text))
    {
        //提供基于文本流的功能(对于ASCII文本）
        QTextStream textStream(&file);
        while (!textStream.atEnd ())      //判断是否到文件流或文件终点
        {
            //读取流中的一行,并返回包含文本的返回的字符串
            //(不包含任何换行符合回车符-不同于QIODevice::readline()--不会省略最后换行符).
            showWidget->text->append (textStream.readLine ());
            printf("read line\n");
        }
        printf("end\n");
    }
}

//保存文件
void ImgProcessor::ShowSaveFile ()
{
    if (curFileName.isEmpty ())    //原路径文件名
    {
        QString filename = QFileDialog::getSaveFileName (this, tr("保存文件"),
                                                         QString(), tr("*.txt;;*.cpp;;*.doc"));
        if (!filename.isEmpty ()) {
            QFile file(filename);
            if (!file.open (QIODevice::WriteOnly)) {
                QMessageBox::critical (this, tr("错误"), tr("不能打开文件"));
                return;
            }
            curFileName = filename;      //下次保存，默认同一条路径
            QTextStream outs(&file);
            outs << showWidget->text->toPlainText ();
            outs.flush ();
            file.close ();
        }
    }
    else {
        QFile file(curFileName);
        if (!file.open (QIODevice::WriteOnly)) {
            QMessageBox::critical (this, tr("错误"), tr("不能打开文件"));
            return;
        }
        QTextStream outs(&file);
        outs << showWidget->text->toPlainText ();
        outs.flush ();
        file.close ();
    }

}

//另存为
void ImgProcessor::ShowSaveAsFile ()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("保存文件"),
		QString(), tr("*.txt;;*.cpp;;*.doc"));
    if (!filename.isEmpty ()) {
        QFile file(filename);
        if (!file.open (QIODevice::WriteOnly)) {
            QMessageBox::critical (this, tr("错误"), tr("不能打开文件"));
            return;
        }
        QTextStream outs(&file);
        //将text->toPlainText()返回文本编辑的纯文本,outs<<将纯文本写入流
        outs << showWidget->text->toPlainText ();
        outs.flush ();
        file.close ();
    }
}

//显示查找对话框
void ImgProcessor::ShowFindDialog ()
{
    QDialog *FindDialog = new QDialog(this);
    QGridLayout *mainLayout = new QGridLayout(FindDialog);

    lineLabel = new QLabel(tr("查找内容(N): "));
    lineEdit = new QLineEdit;

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget (lineLabel);
    inputLayout->addWidget (lineEdit);

    findNextBtn = new QPushButton(tr("findnext"));
    cancelBtn = new QPushButton(tr("取消"));
    connect (cancelBtn, SIGNAL(clicked(bool)), FindDialog, SLOT(reject()));

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget (findNextBtn);
    buttonLayout->addWidget (cancelBtn);

    directLabel = new QLabel(tr("方向"));
    upBtn = new QRadioButton(tr("向上"));


    upBtn->setChecked (true);
//    findNextBtn->setEnabled (false);

    downBtn = new QRadioButton(tr("向下"));

    lowOrUpperBox = new QCheckBox(tr("区分大小写(C)"));

    mainLayout->addLayout (inputLayout, 0, 0, 1, 3);
    mainLayout->addLayout (buttonLayout, 0, 4, 2, 1);
    mainLayout->addWidget (lowOrUpperBox, 2, 0);
    mainLayout->addWidget (directLabel, 1, 1);
    mainLayout->addWidget (upBtn, 2, 1);
    mainLayout->addWidget (downBtn, 2, 2);

    FindDialog->show ();
    str = "";
    connect (lineEdit, SIGNAL(textChanged(QString)), this, SLOT(ShowFindText()));
    connect (findNextBtn, SIGNAL(clicked(bool)), this, SLOT(ShowFindStr()));
}

void ImgProcessor::ShowFindText ()
{
    str = lineEdit->text ();
}

void ImgProcessor::ShowFindStr ()
{
//    qDebug() << str;
    bool judge;
    if (!str.isEmpty ()) {
        if (upBtn->isChecked () == Qt::Checked) {
            if (lowOrUpperBox->isChecked () == Qt::Checked)
                judge = showWidget->text->find (str,QTextDocument::FindCaseSensitively);
            else {
                judge = showWidget->text->find (str);
            }
        }
        else {
            if (lowOrUpperBox->isChecked () == Qt::Checked) {
                judge = showWidget->text->find (str, QTextDocument::FindCaseSensitively);
            }
            else {
                judge = showWidget->text->find (str, QTextDocument::FindBackward);
            }
        }
    }
    else {
        return;
    }
    if (!judge) {
        QMessageBox::information (this, tr("结果"), tr("no finding information"),
                                  QMessageBox::Yes);
    }
}

//替换对话框
void ImgProcessor::ShowSubDialog ()
{
    QDialog *SubDialog = new QDialog(this);
    QGridLayout *mainLayout = new QGridLayout(SubDialog);

    lineLabel21 = new QLabel(tr("查找内容(N): "));
    lineEdit21 = new QLineEdit;

    lineLabel22 = new QLabel(tr("替换(P): "));
    lineEdit22 = new QLineEdit;

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget (lineLabel21);
    inputLayout->addWidget (lineEdit21);

    QHBoxLayout *inputLayout2 = new QHBoxLayout;
    inputLayout2->addWidget (lineLabel22);
    inputLayout2->addWidget (lineEdit22);

    findNextBtn2 = new QPushButton(tr("findnext"));
    subBtn2 = new QPushButton(tr("替换"));
    cancelBtn2 = new QPushButton(tr("取消"));
    connect (cancelBtn2, SIGNAL(clicked(bool)), SubDialog, SLOT(reject()));

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget (findNextBtn2);
    buttonLayout->addWidget (subBtn2);
    buttonLayout->addWidget (cancelBtn2);

    lowOrUpperBox = new QCheckBox(tr("区分大小写(C)"));

    mainLayout->addLayout (inputLayout, 0, 0, 1, 3);
    mainLayout->addLayout (inputLayout2, 1, 0, 1, 3);
    mainLayout->addLayout (buttonLayout, 0, 4, 3, 1);
    mainLayout->addWidget (lowOrUpperBox, 2, 0);

    SubDialog->show ();
    str2 = "";
    str3 = "";
    findjudge = false;
    connect (lineEdit21, SIGNAL(textChanged(QString)), this, SLOT(ShowSubText()));
    connect (lineEdit22, SIGNAL(textChanged(QString)), this, SLOT(ShowSubText2()));
    connect (findNextBtn2, SIGNAL(clicked(bool)), this, SLOT(ShowFindStr2()));
    connect (subBtn2, SIGNAL(clicked(bool)), this, SLOT(ShowSubstitute()));
}

void ImgProcessor::ShowSubText ()
{
    str2 = lineEdit21->text ();
}

void ImgProcessor::ShowSubText2 ()
{
    str3 = lineEdit22->text ();
}

void ImgProcessor::ShowFindStr2 ()
{
    bool judge2;
    if (!str2.isEmpty ()) {
        if (lowOrUpperBox->isChecked () == Qt::Checked)
            judge2 = showWidget->text->find (str2,QTextDocument::FindCaseSensitively);
        else {
            judge2 = showWidget->text->find (str2, QTextDocument::FindBackward);
        }
    }
    else {
        return;
    }
    if (!judge2) {
        QMessageBox::information (this, tr("结果"), tr("no finding information"),
                                  QMessageBox::Yes);
    }
}


void ImgProcessor::ShowSubstitute ()
{
    QTextCursor textCur;
    int judge2;
    if (!str2.isEmpty ()) {
        if (lowOrUpperBox->isChecked () == Qt::Checked) {
            judge2 = showWidget->text->find (str2,QTextDocument::FindCaseSensitively);
            textCur = showWidget->text->textCursor ();
        }
        else {
            judge2 = showWidget->text->find (str2, QTextDocument::FindBackward);
            textCur = showWidget->text->textCursor ();
        }
    }
    else {
        return;
    }
    if (!judge2) {
        QMessageBox::information (this, tr("结果"), tr("no finding information"),
                                  QMessageBox::Yes);
    }
    else {
         textCur.insertText (str3);
    }

}

//打印文本
void ImgProcessor::ShowPrintText ()
{
    QPrinter printer;
    //创建一个QPrintDialog对象，参数为QPrinter对象
    QPrintDialog printDialog(&printer, this);
    //判断打印对话框显示后用户是否单击“打印”,打印--则相关打印属性将可以通过创建QPrintDialog
    //对象时,使用的QPrinter对象获得;单击取消，则不执行后续的打印操作
    if (printDialog.exec ())
    {
        //获得QTextEdit对象的文档
        QTextDocument *doc = showWidget->text->document ();
        //打印
        doc->print (&printer);
    }

}

//打印图像
void ImgProcessor::ShowPrintImage ()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec ())
    {
        //创建一个QPainter对象,并指定绘图设备为一个QPrinter对象
        QPainter painter(&printer);
        //获得QPainter对象的视图矩形区域
        QRect rect = painter.viewport ();

        QSize size = img.size ();                        //获得图像大小
        /* 按照图形的比例大小重新设置视图矩形区域 */
        //Qt::KeepAspectRatio:  按照图形比例大小，保持宽高比例，尽可能大的在矩形内
        size.scale (rect.size (), Qt::KeepAspectRatio);  //图像大小改变
        //重新得到painter对象的视图矩形区域
        //设置QPainter的视图矩形区域at(x,y) with the given width and height
        painter.setViewport (rect.x (), rect.y (), size.width (), size.height ());
        painter.setWindow (img.rect ());      //设置QPainter窗口大小为图像的大小
        painter.drawImage (0, 0, img);        //打印图像at(0,0)
    }
}

//放大
void ImgProcessor::ShowZoomIn ()
{
    if (img.isNull ())     //有效性判断
        return;
    QMatrix matrix;        //声明一个QMatrix类的实例
    //按照2倍比例对水平和垂直方向进行放大，并将当前显示的图形按照该坐标矩阵进行转换
    matrix.scale (2, 2);
    img = img.transformed (matrix);
    //重新设置显示图形
    showWidget->imageLabel->setPixmap (QPixmap::fromImage (img));
}

//缩小
void ImgProcessor::ShowZoomOut ()
{
    if (img.isNull ())
        return;
    QMatrix matrix;
    matrix.scale (0.5, 0.5);
    img = img.transformed (matrix);
    showWidget->imageLabel->setPixmap (QPixmap::fromImage (img));

}

//旋转90°
void ImgProcessor::ShowRotate90 ()
{
    if (img.isNull ())
        return;
    QMatrix matrix;
    matrix.rotate (90);
    img = img.transformed (matrix);
    showWidget->imageLabel->setPixmap (QPixmap::fromImage (img));
}

void ImgProcessor::ShowRotate180 ()
{
    if (img.isNull ())
        return;
    QMatrix matrix;
    matrix.rotate (180);
    img = img.transformed (matrix);
    showWidget->imageLabel->setPixmap (QPixmap::fromImage (img));
}

void ImgProcessor::ShowRotate270 ()
{
    if (img.isNull ())
        return;
    QMatrix matrix;
    matrix.rotate (270);
    img = img.transformed (matrix);
    showWidget->imageLabel->setPixmap (QPixmap::fromImage (img));
}

//镜像--垂直
void ImgProcessor::ShowMirrorVertical ()
{
    if (img.isNull ())
        return;
    //Qimage Qimage::mirrored(bool horizontal = false, bool vertical = true);
    //垂直镜像
    img = img.mirrored (false, true);
    showWidget->imageLabel->setPixmap (QPixmap::fromImage (img));
}

//水平镜像
void ImgProcessor::ShowMirrorHorizontal ()
{
    if (img.isNull ())
        return;
    //Qimage Qimage::mirrored(bool horizontal = true, bool vertical = false);
    //水平镜像
    img = img.mirrored (true, false);
    showWidget->imageLabel->setPixmap (QPixmap::fromImage (img));
}

//设置字体格式
//字体
void ImgProcessor::ShowFontComboBox (QString comboStr)
{
    QTextCharFormat fmt;            //创建一个QTextCharFormat对象
    fmt.setFontFamily (comboStr);   //选择的字体名称设置给QTextCharFormat对象
    mergeFormat (fmt);              //将新格式应用到光标选区内的字符
}

//对于所有的QTextDocumet进行的修改都通过QTextCursor类来完成
void ImgProcessor::mergeFormat (QTextCharFormat format)
{
    QTextCursor cursor = showWidget->text->textCursor ();   //获得编辑框中的光标
    //若光标没有高亮区，则光标所在处的词为选区(由前后有空格，“，”，“、”，“."等标点分隔
    if (!cursor.hasSelection ())
        cursor.select (QTextCursor::WordUnderCursor);
    //将format所表示的格式应用到光标所在处的字符上
    cursor.mergeCharFormat (format);
    //调用QTextEdit的mergeCurrentCharFormat()将格式应用到选区的所有字符上
    showWidget->text->mergeCurrentCharFormat (format);
}

//设置字号
void ImgProcessor::ShowSizeSpinBox (QString spinValue)
{
    QTextCharFormat fmt;
    //设置字号
    fmt.setFontPointSize (spinValue.toFloat ());
    //直接调用QTextEdit的
    showWidget->text->mergeCurrentCharFormat (fmt);
}

//设置文字显示加粗
void ImgProcessor::ShowBoldBtn ()
{
    QTextCharFormat fmt;
    //设置是否加粗
    fmt.setFontWeight (boldBtn->isChecked () ? QFont::Bold : QFont::Normal);
    showWidget->text->mergeCurrentCharFormat (fmt);
}

//设置文字倾斜
void ImgProcessor::ShowItalicBtn ()
{
    QTextCharFormat fmt;
    //设置是否倾斜 (true or false)
    fmt.setFontItalic (italicBtn->isChecked() );
    showWidget->text->mergeCurrentCharFormat (fmt);
}

//是否下划线
void ImgProcessor::ShowUnderlineBtn ()
{
    QTextCharFormat fmt;
    //设置是否有下划线
    fmt.setFontUnderline (underlineBtn->isChecked ());
    showWidget->text->mergeCurrentCharFormat (fmt);
}

//是否字体颜色
void ImgProcessor::ShowColorBtn ()
{
    QColor color = QColorDialog::getColor (Qt::red, this);
    if (color.isValid ())              //查看颜色是否有效
    {
        QTextCharFormat fmt;
        fmt.setForeground (color);     //设置前景颜色
        showWidget->text->mergeCurrentCharFormat (fmt);
    }
}

//是否背景颜色
void ImgProcessor::ShowBackColorBtn()
{
    QColor color = QColorDialog::getColor (Qt::white, this);
    if (color.isValid ())
    {
        QPalette p = showWidget->palette ();
        p.setColor (QPalette::Base, color);
        showWidget->setPalette (p);
    }
}

//设置字符格式
void ImgProcessor::ShowCurrentFormatChanged (const QTextCharFormat &fmt)
{
    //findText()返回(fmt.fontFamily()位置的索引)根据该索引得到ComboBox里面的元素
    fontComboBox->setCurrentIndex (fontComboBox->findText (fmt.fontFamily ()));
    sizeComboBox->setCurrentIndex (sizeComboBox->findText (QString::number (fmt.fontPointSize ())));
    boldBtn->setChecked(fmt.font ().bold ());
    italicBtn->setChecked (fmt.fontItalic ());
    underlineBtn->setChecked (fmt.fontUnderline ());
}

//实现段落对齐
void ImgProcessor::ShowAlignment (QAction *act)
{
    if (act == leftAction)
        showWidget->text->setAlignment (Qt::AlignLeft);
    if (act == rightAction)
        showWidget->text->setAlignment (Qt::AlignRight);
    if (act == centerAction)
        showWidget->text->setAlignment (Qt::AlignCenter);
    if (act == justifyAction)
        showWidget->text->setAlignment (Qt::AlignJustify);
}

//响应文本中发生改变的信号的函数
//完成四个按钮的状态更新,通过调用QTextEdit类的alignment()函数获得当前光标所在段落
//的对齐方式,设置相应的对齐按钮为按下状态
void ImgProcessor::ShowCursorPositionChanged ()
{
    if (showWidget->text->alignment () == Qt::AlignLeft)
        leftAction->setChecked (true);
    if (showWidget->text->alignment () == Qt::AlignRight)
        rightAction->setChecked (true);
    if (showWidget->text->alignment () == Qt::AlignCenter)
        centerAction->setChecked (true);
    if (showWidget->text->alignment () == Qt::AlignJustify)
        justifyAction->setChecked (true);

}

//通过获取当前文本段QTextBlockFormat的缩进值来进行相应的计算的方法,以获得排序文本的缩进值
void ImgProcessor::ShowList (int index)
{
    //获得编辑框的QTextCursor对象指针
    QTextCursor cursor = showWidget->text->textCursor ();
    if (index != 0)
    {
        QTextListFormat::Style style = QTextListFormat::ListDisc;  //初始化style属性值
        switch(index)
        {
        default:
        //根据索引值来确定style属性值
        case 1:
            style = QTextListFormat::ListDisc;       break;
        case 2:
            style = QTextListFormat::ListCircle;     break;
        case 3:
            style = QTextListFormat::ListSquare;     break;
        case 4:
            style = QTextListFormat::ListDecimal;    break;
        case 5:
            style = QTextListFormat::ListLowerAlpha; break;
        case 6:
            style = QTextListFormat::ListUpperAlpha; break;
        case 7:
            style = QTextListFormat::ListLowerRoman; break;
        case 8:
            style = QTextListFormat::ListUpperRoman; break;
        }
        /* 设置缩进值 */
        //设定beginEditBlock()和endEditBlock()间的所有操作相当于一个动作
        cursor.beginEditBlock ();
        //通过QTextcursor获得QTextBlockFormat对象,由其获得段落的缩进值.
        QTextBlockFormat blockFmt = cursor.blockFormat ();
        QTextListFormat  listFmt;

        //下面的if{...}else {...}代码用来改变段落缩进的长度
        if (cursor.currentList ())
        {
            listFmt = cursor.currentList()->format ();
        }
        else
        {
            //再通过QTextListFormat定义缩进值
            listFmt.setIndent (blockFmt.indent() + 1);
            blockFmt.setIndent (0);
            cursor.setBlockFormat (blockFmt);
        }
        //将设定的格式用到光标所在文本
//        qDebug() << "progressing...";
        listFmt.setStyle (style);
        cursor.createList (listFmt);
        cursor.endEditBlock ();
    }
    else
    {
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex (-1);
        cursor.mergeBlockFormat (bfmt);
    }
}

ImgProcessor::~ImgProcessor()
{

}


