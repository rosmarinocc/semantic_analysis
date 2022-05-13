#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "mainwindow.h"
#include <windows.h>
#include <shellapi.h>

//#include <winbase.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->LR1ANA =new LR1(this->ui->control,this->ui->lex_out1,this->ui->lex_out2,this->ui->gra_out);
    this->Lexinputpath=Linpath ;
    this->Grainputpath=Ginpath ;
    this->Lexoutpath  =Loutpath;
    this->Graoutpath  =Goutpath;
    lexisok=false;
    graisok=false;
    outisok=false;
    lexanaok=false;
    graanaok=false;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbt_lex_clicked()
{
    if(this->lexisok&&this->graisok&&this->outisok)
    {
        Mess_to_control("[Lex-Analysis] 文件规定如下：\n",this->ui->control, QTmessage_append);
        Mess_to_control("[Lex-Analysis] 词法分析输入文件："+this->Lexinputpath+"\n",this->ui->control , QTmessage_append);
        Mess_to_control("[Lex-Analysis] 词法分析输出文件："+this->Lexoutpath+"\n",this->ui->control , QTmessage_append);

        this->ui->action_lex->setEnabled(false);
        this->ui->action_gra->setEnabled(false);
        this->ui->action_output->setEnabled(false);
        this->ui->control->append("[Lex-Analysis] 词法分析开始……\n");
        this->ui->control->append("[Lex-Analysis] 词法分析结果将会被存储在指定输出路径……\n");
        this->LR1ANA->lex.lex_analyze(this->Lexinputpath.toStdString());
        this->LR1ANA->lex.output_result(this->Lexoutpath.toStdString());

        this->filetoText(this->Lexinputpath,this->ui->lex_out1);
        this->filetoText(this->Lexoutpath,this->ui->lex_out2);

        this->ui->control->append("[Lex-Analysis] 词法分析结束\n");
        this->lexanaok=true;
        this->ui->action_lex->setEnabled(true);
        this->ui->action_gra->setEnabled(true);
        this->ui->action_output->setEnabled(true);
    }
    else
    {
        this->ui->control->append("请先指定路径,指定输入和输出文件\n");
        QMessageBox::warning(this, tr("提示"),
                             tr("请先指定输入和输出路径！\n"),
                             QMessageBox::Cancel);
    }

}


bool MainWindow::Process_circle()
{
    qDebug()<<lexisok<<endl;
    qDebug()<<graisok<<endl;
    qDebug()<<outisok<<endl;
    qDebug()<<lexanaok<<endl;

    Mess_to_control("[Gra-Analysis] 文件规定如下：\n",this->ui->control,QTmessage_append);
    qDebug()<<"here1"<<endl;
    Mess_to_control("[Gra-Analysis] 语法分析输入文件："+this->Grainputpath+"\n",this->ui->control,QTmessage_append);
    qDebug()<<"here2"<<endl;
    Mess_to_control("[Gra-Analysis] 语法分析输出文件："+this->Graoutpath+"\n",this->ui->control , QTmessage_append);
    this->ui->action_lex->setEnabled(false);
    this->ui->action_gra->setEnabled(false);
    this->ui->action_output->setEnabled(false);
    this->ui->control->append("[Gra-Analysis] 语法分析开始……\n");
    this->ui->control->append("[Gra-Analysis] 语法分析结果将会被存储在指定输出路径……\n");
    qDebug()<<"here2"<<endl;

    this->LR1ANA->read_generators(this->Grainputpath.toStdString());
    qDebug()<<"93"<<this->Grainputpath<<endl;
    qDebug()<<"here3"<<endl;
    this->LR1ANA->get_firstset_of_vn();
    qDebug()<<"here4"<<endl;
    this->LR1ANA->init_items();
    qDebug()<<"here5"<<endl;
    this->LR1ANA->getTable();

    this->LR1ANA->printTable(this->Graoutpath.toStdString());

    int ret=this->LR1ANA->grammartree(this->treepath.toStdString(),this->processpath.toStdString(),this->LR1ANA->lex.Result);


    if(ret==0)
    {
        this->LR1ANA->SemanticAnalysis.printQuadruple(this->semanpath.toStdString().c_str());

        QString comm="dot -Tjpg " + this->treepath + " -o " + this->outpath + "/tree.jpg";
        WinExec(comm.toStdString().c_str(),SW_HIDE);
        qDebug()<<comm<<endl;
        QString treejpg = this->outpath + "/tree.jpg";
        QString treefile = treejpg.replace(QString("/"), QString("\\"));        
        qDebug()<<treefile<<endl;
        QString com1="rundll32.exe C:\\Windows\\System32\\shimgvw.dll,ImageView_Fullscreen "+treefile;
        WinExec(com1.toStdString().c_str(),SW_NORMAL);
    }
    else
    {

        QMessageBox::warning(this, tr("提示"),
                             tr("ActionGototable错误，归结失败！！\n"),
                             QMessageBox::Cancel);
    }


    this->ui->control->append("[Gra-Analysis] 语法分析结束\n");
    int semErrorNum = this->LR1ANA->SemanticAnalysis.errorInfo.size();
    if (semErrorNum)
    {
        this->ui->control->append("*****ERROR DETECTED*****");
        for (int i = 0; i < semErrorNum; i++)
        {
            this->ui->control->append(QString::fromStdString(this->LR1ANA->SemanticAnalysis.errorInfo[i])+"\n");
        }
    }
    this->graanaok=true;
    return true;
}

void MainWindow::on_pbt_gra_clicked()
{
    if(this->lexisok&&this->graisok&&this->outisok&&this->lexanaok)
    {
        QWaitingDialog *p_wait=new QWaitingDialog(this);
        p_wait->show();
        QFuture<int> fut_syllable=QtConcurrent::run([=]()->int{ return
                    this->Process_circle();}); //异步处理
        while(!fut_syllable.isFinished())
        {
            QApplication::processEvents(QEventLoop::AllEvents, 100);//唤醒主界面
        }
        p_wait->close();
        this->ui->action_lex->setEnabled(true);
        this->ui->action_gra->setEnabled(true);
        this->ui->action_output->setEnabled(true);
        filetoText(this->processpath.toStdString().c_str(),this->ui->gra_out);
        filetoText(this->semanpath.toStdString().c_str(),this->ui->sem_out);
        QMessageBox::warning(this, tr("提示"),
                             tr("语法分析结束！\n"),
                             QMessageBox::Cancel);
    }
    else
    {
        this->ui->control->append("请检查路径，为了展示语法树，先进行词法分析\n");
        QMessageBox::warning(this, tr("提示"),
                             tr("请检查路径，为了展示语法树，先进行词法分析！\n"),
                             QMessageBox::Cancel);
    }
}


void MainWindow::on_actionIntroduction_triggered()
{
    Form *intro = new Form();
    intro->setAttribute(Qt::WA_DeleteOnClose);
    intro->show();
}


void MainWindow::on_action_lex_triggered()
{
    this->Lexinputpath = QFileDialog :: getOpenFileName(this, tr("Open File"), "/home", tr("Text files (*.txt);;CPP files (*.cpp"));
    this->lexisok=true;
    if(this->lexisok&&this->graisok&&this->outisok)
    {
        this->ui->pbt_lex->setDisabled(false);//在没有指定路径前，无法进行词法和语法分析
        this->ui->pbt_gra->setDisabled(false);
    }
}


void MainWindow::on_action_gra_triggered()
{
    this->Grainputpath = QFileDialog :: getOpenFileName(this, tr("Open File"), "/home", tr("Text files (*.txt)"));
    this->graisok=true;
    if(this->lexisok&&this->graisok&&this->outisok)
    {
        this->ui->pbt_lex->setDisabled(false);//在没有指定路径前，无法进行词法和语法分析
        this->ui->pbt_gra->setDisabled(false);
    }
}


void MainWindow::on_action_output_triggered()
{
    this->outpath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      "",
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);
    this->Lexoutpath=this->outpath+"/"+Loutpath;
    this->Graoutpath=this->outpath+"/"+Goutpath;
    this->treepath=this->outpath+"/"+Treepath;
    this->processpath=this->outpath+"/"+Processpath;
    this->semanpath = this->outpath+"/"+Semansticpath;

    qDebug()<<Lexoutpath<<endl;
    qDebug()<<Graoutpath<<endl;
    qDebug()<<treepath<<endl;
    qDebug()<<this->processpath<<endl;
    qDebug()<<this->semanpath<<endl;

    this->outisok=true;
    if(this->lexisok&&this->graisok&&this->outisok)
    {
        this->ui->pbt_lex->setDisabled(false);//在没有指定路径前，无法进行词法和语法分析
        this->ui->pbt_gra->setDisabled(false);
    }
}

void MainWindow::filetoText(const QString filePath,QTextBrowser * towhere)
{
    stringstream sstr;
    fstream fin;
    fin.open(filePath.toStdString().c_str(),ios::in);
    if(!fin.is_open())
    {
        QString mes="文件"+filePath+"打开失败！\n";
        Mess_to_control("[Open-file] Open-file failed because can't access to it!\n",this->ui->control,QTmessage_append);
        QMessageBox::warning(this, tr("提示"),
                             tr(mes.toStdString().c_str()),
                             QMessageBox::Cancel);
    }
    char ch;
    while (1) {
        fin.get(ch);
        if (fin.eof())
            break;
        sstr << ch;
    }
    fin.close();
    towhere->setText("");
    while (1) {
        char buf[1000] = { 0 };
        sstr.getline(buf, 1000, '\n');
        if (buf[0] == '\0')
            break;
        QString str=buf;
        Mess_to_control(str,towhere,QTmessage_append);
        //towhere->append(str);
    }
}
