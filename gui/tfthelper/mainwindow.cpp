#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utilities.h"

#include<QFile>
#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->inputGbx->setTitle(tr("Input parameters"));
//    ui->lvlabel->setText(tr("Lv"));
//    ui->tierlabel->setText(tr("Tier"));
//    ui->fixRVlabel->setText(tr("#rolling"));
//    ui->outputLabel->setText(tr("Output r.v."));
//    ui->targetDrawnNumlabel->setText(tr("#target drawn"));
//    ui->sameTierNumlabel->setText(tr("#same tier"));
//    ui->meanLabel->setText(tr("mean"));
//    ui->stdLabel->setText(tr("std"));

    ui->targetDrawnNumEdit->setText("0");
    ui->sameTierNumEdit->setText("0");
    ui->lvlineEdit->setFocus();

//    ui->outputTypeCbx->addItem(tr("#target")); // index 0
//    ui->outputTypeCbx->addItem(tr("#rolling")); // index 1

    ui->runBtn->setShortcut(Qt::Key_Enter);
    ui->runBtn->setShortcut(Qt::Key_Return);

    connect(ui->menuLanguage, SIGNAL(triggered(QAction*)), this, SLOT(on_languageChanged(QAction*)));
    ui->outputTypeCbx->currentIndexChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_runBtn_clicked()
{
    // 0. input parameter declaration
    int lv;
    int tier;
    int fixRV;
    int targetDrawn;
    int sameTierDrawn;
    //1. check Input validity and set parameter
    if (!checkInput(ui->lvlineEdit, ui->statusbar, lv, 2, 9))
        return;
    if (!checkInput(ui->tierlineEdit, ui->statusbar, tier, 1, 5))
        return;
    if (!checkInput(ui->fixRVEdit, ui->statusbar, fixRV, 1))
        return;
    if (!checkInput(ui->targetDrawnNumEdit, ui->statusbar, targetDrawn, 0))
        return;
    if (!checkInput(ui->sameTierNumEdit, ui->statusbar, sameTierDrawn, 0))
        return;

    // 2. get pdf
    std::vector<double> pdfArray;
    switch (ui->outputTypeCbx->currentIndex()) {
        case 0:
            pdfArray = TFTHelper::GetCardDrawnPDFArray(lv, tier, fixRV, targetDrawn, sameTierDrawn);
            break;
        case 1:
            pdfArray = TFTHelper::GetStopTimePDFArray(lv, tier, fixRV, targetDrawn, sameTierDrawn);
            break;
        default:
            break;
    }
    ui->statusbar->showMessage("");
    // 3. output
    int n = pdfArray.size();
    double mean = 0;
    double std = 0;
    double probSum = 0;

    ui->resultTable->setRowCount(0);
    ui->resultTable->clearContents();
    for (int i = 0; i < n; i++) {
        if (pdfArray[i] > 1e-4) {
            //printf("%d  \t%.2f%%\n", i, pdfArray[i] * 100);
            ui->resultTable->insertRow(i);
            ui->resultTable->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
            ui->resultTable->setItem(i, 1, new QTableWidgetItem(QString::asprintf("%.2f%%",pdfArray[i] * 100)));
        } else {
            //printf("%d  \t%.2e\n", i, pdfArray[i]);
            ui->resultTable->insertRow(i);
            ui->resultTable->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
            ui->resultTable->setItem(i, 1, new QTableWidgetItem(QString::asprintf("%.2e",pdfArray[i])));
        }

        mean += i * pdfArray[i];
        std += i * i * pdfArray[i];
        probSum += pdfArray[i];
        if (probSum > 1 - 1e-4) {
            break;
        }
    }

    std -= mean * mean;
    std = sqrt(std);

    ui->meanLabel->setText(tr("mean:") + QString::asprintf("%.2f", mean));
    ui->stdLabel->setText(tr("std:") + QString::asprintf("%.2f", std));

}

bool MainWindow::checkInput(QLineEdit* inputLine, QStatusBar* statubar, int& input, int lowBound, int upperBound) {
    bool isValid = false;
    input = inputLine->text().toInt(&isValid);
    isValid &= (input >= lowBound) && (input <= upperBound);
    if (!isValid) {
        inputLine->setFocus();
        statubar->showMessage(tr("Invalid input!"));
    }
    return isValid;
}

void MainWindow::on_outputTypeCbx_currentIndexChanged(int index)
{
    switch (index) {
        case 0:
            ui->fixRVlabel->setText(tr("#rolling"));
            break;
        case 1:
            ui->fixRVlabel->setText(tr("#target"));
            break;
        default:
            break;
    }
}

void MainWindow::on_languageChanged(QAction* languageAction) {
    QFile file(qApp->applicationDirPath() + "/setting/setting.ini");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    if (languageAction == ui->actionEnglish) {
        file.write("");
    } else if (languageAction == ui->actionChinese) {
        file.write("tfthelper_zh_CN.qm");
    }
    file.close();
    QMessageBox::information(this,"", "please restart the application for your language setting");
}


