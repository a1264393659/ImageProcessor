#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QString>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

class findDialog : public QDialog
{
    Q_OBJECT
public:
    enum state { NEXT, PRE } ;
    findDialog(QWidget *parent = 0);
    QLabel *lineLabel;
    QLineEdit *lineEdit;
    QPushButton *findNextBtn;
    QPushButton *cancelBtn;

    QLabel *directLabel;

    QRadioButton *upBtn;
    QRadioButton *downBtn;

    QCheckBox *lowOrUpperBox;

    state uod;

signals:
    void findNext(QString, bool b = 0);
    void findPrevious(QString, bool b = 0);

private slots:
    void findButtonState();
    void findBoxState();
    void findText ();

};

#endif // FINDDIALOG_H



