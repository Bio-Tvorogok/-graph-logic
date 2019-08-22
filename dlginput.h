#ifndef DLGINPUT_H
#define DLGINPUT_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>

class DlgInput : public QDialog
{
    Q_OBJECT
public:
    explicit DlgInput(QWidget *parent = nullptr);
    ~DlgInput() { }
    QLabel *lTipInput;   // Подсказки ввода
    QLineEdit *eInput;
    QPushButton *btnApply;   // Применить ввод (с проверкой корректности).
    QPushButton *btnCancel;
    bool is_recovery = false;
private slots:
    void accept_func();

signals:
    void accept_signal();
};

#endif // DLGINPUT_H
