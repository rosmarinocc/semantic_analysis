/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionIntroduction;
    QAction *action_lex;
    QAction *action_gra;
    QAction *action_output;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QPushButton *pbt_lex;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pbt_gra;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QTextBrowser *lex_out1;
    QSpacerItem *verticalSpacer;
    QTextBrowser *lex_out2;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QTextBrowser *gra_out;
    QTextBrowser *sem_out;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QTextBrowser *control;
    QMenuBar *menubar;
    QMenu *menu;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1162, 729);
        actionIntroduction = new QAction(MainWindow);
        actionIntroduction->setObjectName(QString::fromUtf8("actionIntroduction"));
        action_lex = new QAction(MainWindow);
        action_lex->setObjectName(QString::fromUtf8("action_lex"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("D:/各种作业/编译原理/QtObject/QtObject/GrammerAnalysis/input_lex.ico"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8("../input_lex.ico"), QSize(), QIcon::Normal, QIcon::On);
        action_lex->setIcon(icon);
        action_gra = new QAction(MainWindow);
        action_gra->setObjectName(QString::fromUtf8("action_gra"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("D:/各种作业/编译原理/QtObject/QtObject/GrammerAnalysis/input_gra.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_gra->setIcon(icon1);
        action_output = new QAction(MainWindow);
        action_output->setObjectName(QString::fromUtf8("action_output"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("D:/各种作业/编译原理/QtObject/QtObject/GrammerAnalysis/output_lex.ico"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8("../output_lex.ico"), QSize(), QIcon::Normal, QIcon::On);
        action_output->setIcon(icon2);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout_4 = new QHBoxLayout(centralwidget);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(22);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label);

        pbt_lex = new QPushButton(centralwidget);
        pbt_lex->setObjectName(QString::fromUtf8("pbt_lex"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(20);
        pbt_lex->setFont(font1);
        pbt_lex->setStyleSheet(QString::fromUtf8("                   QPushButton{\n"
"				   background-color:rgba(141,173,225, 120);\n"
"                   border-style:outset;                \n"
"                   border-width:4px;                  \n"
"                   border-radius:10px;                \n"
"                   border-color:rgba(255,255,255,30);               \n"
"                   color:rgba(0,0,0,100); \n"
"                   padding:6px;}                \n"
"\n"
"                   QPushButton:pressed{\n"
"                   background-color:rgba(141, 173, 225, 250);\n"
"                   border-color:rgba(255,255,255,30);\n"
"                   border-style:inset;\n"
"                   color:rgba(0,0,0,100);\n"
"                   }\n"
"\n"
"                   QPushButton:hover{\n"
"                   background-color:rgba(141,173, 225, 180);\n"
"                   border-color:rgba(255,255,255,200);\n"
"                   color:rgba(0,0,0,200);\n"
"                   }"));

        horizontalLayout->addWidget(pbt_lex);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pbt_gra = new QPushButton(centralwidget);
        pbt_gra->setObjectName(QString::fromUtf8("pbt_gra"));
        pbt_gra->setFont(font1);
        pbt_gra->setStyleSheet(QString::fromUtf8("                   QPushButton{\n"
"				   background-color:rgba(141,173,225, 120);\n"
"                   border-style:outset;                \n"
"                   border-width:4px;                  \n"
"                   border-radius:10px;                \n"
"                   border-color:rgba(255,255,255,30);               \n"
"                   color:rgba(0,0,0,100); \n"
"                   padding:6px;}                \n"
"\n"
"                   QPushButton:pressed{\n"
"                   background-color:rgba(141, 173, 225, 250);\n"
"                   border-color:rgba(255,255,255,30);\n"
"                   border-style:inset;\n"
"                   color:rgba(0,0,0,100);\n"
"                   }\n"
"\n"
"                   QPushButton:hover{\n"
"                   background-color:rgba(141,173, 225, 180);\n"
"                   border-color:rgba(255,255,255,200);\n"
"                   color:rgba(0,0,0,200);\n"
"                   }"));

        horizontalLayout->addWidget(pbt_gra);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        horizontalLayout->setStretch(0, 20);
        horizontalLayout->setStretch(1, 1);
        horizontalLayout->setStretch(2, 1);
        horizontalLayout->setStretch(3, 1);
        horizontalLayout->setStretch(4, 1);

        verticalLayout_4->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox{\n"
"border-width:2px;\n"
"\n"
"border-style:solid;\n"
"\n"
"border-radius: 10px;\n"
"\n"
"border-color:gray;\n"
"\n"
"margin-top:0.5ex;\n"
"\n"
"}"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lex_out1 = new QTextBrowser(groupBox);
        lex_out1->setObjectName(QString::fromUtf8("lex_out1"));

        verticalLayout->addWidget(lex_out1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        lex_out2 = new QTextBrowser(groupBox);
        lex_out2->setObjectName(QString::fromUtf8("lex_out2"));

        verticalLayout->addWidget(lex_out2);


        verticalLayout_2->addLayout(verticalLayout);


        horizontalLayout_2->addWidget(groupBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setStyleSheet(QString::fromUtf8("QGroupBox{\n"
"border-width:2px;\n"
"\n"
"border-style:solid;\n"
"\n"
"border-radius: 10px;\n"
"\n"
"border-color:gray;\n"
"\n"
"margin-top:0.5ex;\n"
"\n"
"}"));
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gra_out = new QTextBrowser(groupBox_2);
        gra_out->setObjectName(QString::fromUtf8("gra_out"));

        verticalLayout_3->addWidget(gra_out);

        sem_out = new QTextBrowser(groupBox_2);
        sem_out->setObjectName(QString::fromUtf8("sem_out"));

        verticalLayout_3->addWidget(sem_out);


        horizontalLayout_2->addWidget(groupBox_2);

        horizontalLayout_2->setStretch(0, 12);
        horizontalLayout_2->setStretch(1, 1);
        horizontalLayout_2->setStretch(2, 24);

        verticalLayout_4->addLayout(horizontalLayout_2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_2);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setStyleSheet(QString::fromUtf8("QGroupBox{\n"
"border-width:2px;\n"
"\n"
"border-style:solid;\n"
"\n"
"border-radius: 10px;\n"
"\n"
"border-color:gray;\n"
"\n"
"margin-top:0.5ex;\n"
"\n"
"}"));
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        control = new QTextBrowser(groupBox_3);
        control->setObjectName(QString::fromUtf8("control"));

        horizontalLayout_3->addWidget(control);


        verticalLayout_4->addWidget(groupBox_3);

        verticalLayout_4->setStretch(0, 1);
        verticalLayout_4->setStretch(1, 20);
        verticalLayout_4->setStretch(2, 1);
        verticalLayout_4->setStretch(3, 5);

        horizontalLayout_4->addLayout(verticalLayout_4);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1162, 23));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMinimumSize(QSize(50, 50));
        MainWindow->addToolBar(Qt::LeftToolBarArea, toolBar);

        menubar->addAction(menu->menuAction());
        menu->addAction(actionIntroduction);
        toolBar->addAction(action_lex);
        toolBar->addSeparator();
        toolBar->addAction(action_gra);
        toolBar->addSeparator();
        toolBar->addAction(action_output);
        toolBar->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionIntroduction->setText(QCoreApplication::translate("MainWindow", "Introduction", nullptr));
        action_lex->setText(QCoreApplication::translate("MainWindow", "\350\257\215\346\263\225\345\210\206\346\236\220\350\276\223\345\205\245", nullptr));
        action_gra->setText(QCoreApplication::translate("MainWindow", "\350\257\255\346\263\225\345\210\206\346\236\220\350\276\223\345\207\272", nullptr));
        action_output->setText(QCoreApplication::translate("MainWindow", "\350\276\223\345\207\272\350\267\257\345\276\204", nullptr));
#if QT_CONFIG(tooltip)
        action_output->setToolTip(QCoreApplication::translate("MainWindow", "\350\276\223\345\207\272\346\226\207\344\273\266\350\267\257\345\276\204", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("MainWindow", "\347\274\226\350\257\221\345\216\237\347\220\206\350\257\215\346\263\225\350\257\255\346\263\225\345\210\206\346\236\220", nullptr));
        pbt_lex->setText(QCoreApplication::translate("MainWindow", "\350\257\215\346\263\225\345\210\206\346\236\220", nullptr));
        pbt_gra->setText(QCoreApplication::translate("MainWindow", "\350\257\255\346\263\225/\350\257\255\344\271\211\345\210\206\346\236\220(\350\257\255\346\263\225\346\240\221)", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\257\215\346\263\225\345\210\206\346\236\220", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\350\257\255\346\263\225/\350\257\255\344\271\211\345\210\206\346\236\220\347\273\223\346\236\234", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\345\217\260", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\344\275\277\347\224\250\344\273\213\347\273\215", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
