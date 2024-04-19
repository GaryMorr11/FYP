#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include "PCANBasic.h"
#include "bruteworker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(TPCANBaudrate baudrate, QWidget *parent = nullptr);
    ~MainWindow();

    void sendCANMessage(TPCANMsg& CANMsg);
    bool stopButtonPressed;


private:
    QString m_comment;

public slots:
    void on_brute_clicked();
    void on_stop_clicked();


private slots:
    void on_s1ON_clicked();
    void on_s1OFF_clicked();
    void onTextChanged(); // Slot for handling text changes

    void on_s1inc_clicked();

    void on_s1dec_clicked();

    void on_sendmessage_clicked();

    void on_setmessage_clicked();

    void on_s2ON_clicked();

    void on_s2OFF_clicked();

    void on_s2dec_clicked();

    void on_s2inc_clicked();

    void on_s3ON_clicked();

    void on_s3OFF_clicked();

    void on_s3inc_clicked();

    void on_s3dec_clicked();

    void on_s4ON_clicked();

    void on_s4OFF_clicked();

    void on_s4inc_clicked();

    void on_s4dec_clicked();

    void on_s5ON_clicked();

    void on_s5OFF_clicked();

    void on_s5inc_clicked();

    void on_s5dec_clicked();

    void on_s6ON_clicked();

    void on_s6OFF_clicked();

    void on_s6inc_clicked();

    void on_s6dec_clicked();

    void on_s7ON_clicked();

    void on_s7OFF_clicked();

    void on_s7inc_clicked();

    void on_s7dec_clicked();

    void on_s8ON_clicked();

    void on_s8OFF_clicked();

    void on_s8inc_clicked();

    void on_s8dec_clicked();

    void on_savemessage_clicked();

    //void on_brute_clicked();




    void on_idinc_clicked();

    void on_iddec_clicked();


private:
    Ui::MainWindow *ui;


    TPCANHandle Channel;
    TPCANBaudrate Baudrate;
    bool initializePCAN();
    QThread* bruteThread; // Declaration of the QThread pointer
    BruteWorker* bruteWorker; // Declaration of the BruteWorker pointer
};




#endif // MAINWINDOW_H
