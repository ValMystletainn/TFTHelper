#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLineEdit>
#include<QStatusBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_runBtn_clicked();

    void on_outputTypeCbx_currentIndexChanged(int index);

    void on_languageChanged(QAction* languageAction);

private:
    Ui::MainWindow *ui;

    bool checkInput(QLineEdit* inputLine, QStatusBar* statubar,int& input, int lowBound = INT_MIN, int upperBound = INT_MAX);
};
#endif // MAINWINDOW_H
