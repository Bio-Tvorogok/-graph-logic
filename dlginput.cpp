#include "dlginput.h"
#include <QDebug>
DlgInput::DlgInput(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QGridLayout *layout = new QGridLayout(this);
    lTipInput = new QLabel("Подсказки тут",this);
    lTipInput->setFont(QFont("Times", 13));
    layout->addWidget(lTipInput, 0, 0, 1, 2);
    eInput = new QLineEdit(this);
    //eInput->setEnabled(false);
    eInput->setFont(QFont("Times", 13));
    layout->addWidget(eInput, 1, 0);
    btnApply = new QPushButton("Применить", this);
    btnApply->setFont(QFont("Times", 13));
    btnCancel = new QPushButton("Отмена", this);
    btnCancel->setFont(QFont("Times", 13));
    //btnApply->setEnabled(false);
    connect(btnApply, SIGNAL(clicked()), SLOT(accept_func()));
    connect(this, SIGNAL(accept_signal()), SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), SLOT(reject()));

    layout->addWidget(btnApply, 1, 1);
    layout->addWidget(btnCancel, 1, 2);
    //setFixedSize(minimumSize());
}

void DlgInput::accept_func()
{
    if (!eInput->text().isEmpty()) {
        //if (salary_edit->text().toInt())
        QRegExp re;
        if (!is_recovery)
            re.setPattern("\\d*");
        else
            re.setPattern("[0-9 ]+");
        if (re.exactMatch(eInput->text()))
            emit accept_signal();
        else {
            qDebug() << "bad";
        }
    }
}

