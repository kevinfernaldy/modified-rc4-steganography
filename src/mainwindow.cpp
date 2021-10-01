#include <include/mainwindow.h>
#include "./ui_mainwindow.h"

#include <include/rc4widget.h>
#include <include/steganography.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentWidget = new RC4Widget();
    ui->widgetContainerLayout->addWidget(currentWidget);

    treeViewModel = new QStandardItemModel;
    treeViewModel->appendRow(new QStandardItem("RC4 Modified Cipher"));
    treeViewModel->appendRow(new QStandardItem("RC4 Modified Steganography"));

    ui->treeView->setModel(treeViewModel);
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete currentWidget;
    delete treeViewModel;
}


void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    ui->widgetContainerLayout->removeWidget(currentWidget);
    delete currentWidget;

    switch (index.row()) {
        case 0: {
            currentWidget = new RC4Widget();
            break;
        }
        case 1: {
            currentWidget = new SteganographyWidget();
            break;
        }
    }

    ui->widgetContainerLayout->addWidget(currentWidget);
}

