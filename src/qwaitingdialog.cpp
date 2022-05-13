#include "qwaitingdialog.h"



QWaitingDialog::QWaitingDialog(QWidget *parent) : QWidget(parent)
{
    m_lable=new QLabel(this);
    m_lable->setFixedSize(250,250);
    this->setWindowOpacity(0.8);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);//设置为对话框风格，并且去掉边框
    this->setWindowModality(Qt::WindowModal);
    m_movie=new QMovie("E:/useful_machines/QtObject/please-wait.gif");
    m_lable->setStyleSheet("background-color:transparent;");
    m_lable->setMovie(m_movie);
    m_lable->setScaledContents(true);
    m_movie->start();
}

QWaitingDialog::~QWaitingDialog()
{
    delete m_lable;
    delete m_movie;
}
