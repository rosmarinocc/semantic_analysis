#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "LR1.h"
#include "form.h"
#include "ui_mainwindow.h"
#include "qwaitingdialog.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
const QString Linpath  ="lexinput.txt"; //前声明，用于default，实际上强制要求用户指定
const QString Ginpath  ="grainput.txt";
const QString Loutpath  ="lexoutput.txt";
const QString Goutpath  ="ActionGotoTable.txt";
const QString Treepath  ="tree.dot";
const QString Processpath="process.txt";
const QString Semansticpath="semanresult.txt";

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT    
public:
    MainWindow(QWidget *parent = nullptr);    
    ~MainWindow();
    LR1 *LR1ANA; //LR1分析器
    QString Lexinputpath;
    QString Grainputpath;
    QString Lexoutpath  ;
    QString Graoutpath  ;
    QString treepath  ;
    QString processpath;
    QString outpath;
    QString semanpath;
    bool lexisok;
    bool graisok;
    bool outisok;
    bool lexanaok;
    bool graanaok;

private slots:

    //进行词法分析
    void on_pbt_lex_clicked();
    void on_pbt_gra_clicked();

    //目录文件的选择
    void on_actionIntroduction_triggered();
    void on_action_gra_triggered();
    void on_action_output_triggered();
    void on_action_lex_triggered();
private:
    Ui::MainWindow *ui;
    bool Process_circle();
    void filetoText(const QString filePath,QTextBrowser * towhere);
};
#endif // MAINWINDOW_H
