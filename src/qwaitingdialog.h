#ifndef QWAITINGDIALOG_H
#define QWAITINGDIALOG_H
#include <QWidget>
#include <QMovie>
#include <QLabel>
class QWaitingDialog : public QWidget
{
    Q_OBJECT
public:
    explicit QWaitingDialog(QWidget *parent = nullptr);
    ~QWaitingDialog();
private:
    QMovie *m_movie=nullptr;
    QLabel *m_lable=nullptr;
};

#endif // QWAITINGDIALOG_H
