#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "PCANBasic.h"
#include <QRegularExpression>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QInputDialog>
#include <QThread>
#include <QSystemTrayIcon>
#include <QCoreApplication>
#include <QTimer>
#include "bruteworker.h"
#include <QProcess>


MainWindow::MainWindow(TPCANBaudrate baudrate, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    if (!initializePCAN()) {
        QCoreApplication::quit();
    }

    ui->setupUi(this);

    bruteWorker = new BruteWorker;
    bruteThread = new QThread;
    bruteWorker->moveToThread(bruteThread);
    connect(bruteThread, &QThread::started, bruteWorker, &BruteWorker::startBrute);
    connect(bruteWorker, &BruteWorker::finished, bruteThread, &QThread::quit);
    connect(bruteWorker, &BruteWorker::finished, bruteWorker, &BruteWorker::deleteLater);
    connect(bruteThread, &QThread::finished, bruteThread, &QThread::deleteLater);


    Baudrate = baudrate; // Initialize the member variable

    stopButtonPressed = false;

    // Set the default value for each slot to "00"
    ui->s1->setText("00");
    ui->s2->setText("00");
    ui->s3->setText("00");
    ui->s4->setText("00");
    ui->s5->setText("00");
    ui->s6->setText("00");
    ui->s7->setText("00");
    ui->s8->setText("00");

    ui->id->setText("000");

    // Connect the textChanged signal from each QLineEdit to the onTextChanged slot
    connect(ui->s1, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->s2, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->s3, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->s4, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->s5, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->s6, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->s7, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->s8, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);

    connect(ui->id, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);


    connect(ui->s1ON, &QPushButton::clicked, this, &MainWindow::on_s1ON_clicked);
    connect(ui->s1OFF, &QPushButton::clicked, this, &MainWindow::on_s1OFF_clicked);
    connect(ui->s1inc, &QPushButton::clicked, this, &MainWindow::on_s1inc_clicked);
    connect(ui->s1dec, &QPushButton::clicked, this, &MainWindow::on_s1dec_clicked);

    connect(ui->s2ON, &QPushButton::clicked, this, &MainWindow::on_s2ON_clicked);
    connect(ui->s2OFF, &QPushButton::clicked, this, &MainWindow::on_s2OFF_clicked);
    connect(ui->s2inc, &QPushButton::clicked, this, &MainWindow::on_s2inc_clicked);
    connect(ui->s2dec, &QPushButton::clicked, this, &MainWindow::on_s2dec_clicked);

    connect(ui->s3ON, &QPushButton::clicked, this, &MainWindow::on_s3ON_clicked);
    connect(ui->s3OFF, &QPushButton::clicked, this, &MainWindow::on_s3OFF_clicked);
    connect(ui->s3inc, &QPushButton::clicked, this, &MainWindow::on_s3inc_clicked);
    connect(ui->s3dec, &QPushButton::clicked, this, &MainWindow::on_s3dec_clicked);

    connect(ui->s4ON, &QPushButton::clicked, this, &MainWindow::on_s4ON_clicked);
    connect(ui->s4OFF, &QPushButton::clicked, this, &MainWindow::on_s4OFF_clicked);
    connect(ui->s4inc, &QPushButton::clicked, this, &MainWindow::on_s4inc_clicked);
    connect(ui->s4dec, &QPushButton::clicked, this, &MainWindow::on_s4dec_clicked);

    connect(ui->s5ON, &QPushButton::clicked, this, &MainWindow::on_s5ON_clicked);
    connect(ui->s5OFF, &QPushButton::clicked, this, &MainWindow::on_s5OFF_clicked);
    connect(ui->s5inc, &QPushButton::clicked, this, &MainWindow::on_s5inc_clicked);
    connect(ui->s5dec, &QPushButton::clicked, this, &MainWindow::on_s5dec_clicked);

    connect(ui->s6ON, &QPushButton::clicked, this, &MainWindow::on_s6ON_clicked);
    connect(ui->s6OFF, &QPushButton::clicked, this, &MainWindow::on_s6OFF_clicked);
    connect(ui->s6inc, &QPushButton::clicked, this, &MainWindow::on_s6inc_clicked);
    connect(ui->s6dec, &QPushButton::clicked, this, &MainWindow::on_s6dec_clicked);

    connect(ui->s7ON, &QPushButton::clicked, this, &MainWindow::on_s7ON_clicked);
    connect(ui->s7OFF, &QPushButton::clicked, this, &MainWindow::on_s7OFF_clicked);
    connect(ui->s7inc, &QPushButton::clicked, this, &MainWindow::on_s7inc_clicked);
    connect(ui->s7dec, &QPushButton::clicked, this, &MainWindow::on_s7dec_clicked);

    connect(ui->s8ON, &QPushButton::clicked, this, &MainWindow::on_s8ON_clicked);
    connect(ui->s8OFF, &QPushButton::clicked, this, &MainWindow::on_s8OFF_clicked);
    connect(ui->s8inc, &QPushButton::clicked, this, &MainWindow::on_s8inc_clicked);
    connect(ui->s8dec, &QPushButton::clicked, this, &MainWindow::on_s8dec_clicked);


    connect(ui->setmessage, &QPushButton::clicked, this, &MainWindow::on_setmessage_clicked);
    connect(ui->savemessage, &QPushButton::clicked, this, &MainWindow::on_savemessage_clicked);

    connect(ui->brute, &QPushButton::clicked, this, &MainWindow::on_brute_clicked);

    connect(ui->sendmessage, &QPushButton::clicked, this, &MainWindow::on_sendmessage_clicked);

    connect(ui->idinc, &QPushButton::clicked, this, &MainWindow::on_idinc_clicked);
    connect(ui->iddec, &QPushButton::clicked, this, &MainWindow::on_iddec_clicked);

    connect(ui->stop, &QPushButton::clicked, this, &MainWindow::on_stop_clicked);


}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::initializePCAN()
{
    TPCANStatus stsResult;
    TPCANHandle Channel = PCAN_USBBUS1;

    stsResult = CAN_Initialize(Channel, Baudrate, PCAN_TYPE_ISA);
    if (stsResult != PCAN_ERROR_OK)
    {
        QMessageBox::critical(nullptr, "Error", "Failed to initialize the PCAN device. Error code: " + QString::number(stsResult));
        return false; // Return false if initialization failed
    }
    else
    {
        // This block will only execute if the initialization was successful
        QMessageBox::information(nullptr, "Success", "PCAN device initialized successfully.");
        return true; // Return true if initialization was successful
    }
}



////////////////////////////////////////
////// Slot 1 Implimentations //////////
////////////////////////////////////////


void MainWindow::on_s1ON_clicked()
{
    ui->s1->setText("FF");
}

void MainWindow::on_s1OFF_clicked()
{
    ui->s1->setText("00");
}

void MainWindow::on_s1inc_clicked()
{
    QString currentText = ui->s1->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s1->setText(newText);
    }
    else
    {
        ui->s1->setText("00");
    }
}

void MainWindow::on_s1dec_clicked()
{
    QString currentText = ui->s1->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s1->setText(newText);
    }
    else
    {
        ui->s1->setText("00");
    }
}

////////////////////////////////////////
////// Slot 2 Implementations //////////
////////////////////////////////////////


void MainWindow::on_s2ON_clicked()
{
    ui->s2->setText("FF");
}

void MainWindow::on_s2OFF_clicked()
{
    ui->s2->setText("00");
}

void MainWindow::on_s2inc_clicked()
{
    QString currentText = ui->s2->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s2->setText(newText);
    }
    else
    {
        ui->s2->setText("00");
    }
}

void MainWindow::on_s2dec_clicked()
{
    QString currentText = ui->s2->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s2->setText(newText);
    }
    else
    {
        ui->s2->setText("00");
    }
}


////////////////////////////////////////
////// Slot 2 Implementations //////////
////////////////////////////////////////


void MainWindow::on_s3ON_clicked()
{
    ui->s3->setText("FF");
}

void MainWindow::on_s3OFF_clicked()
{
    ui->s3->setText("00");
}

void MainWindow::on_s3inc_clicked()
{
    QString currentText = ui->s3->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s3->setText(newText);
    }
    else
    {
        ui->s3->setText("00");
    }
}

void MainWindow::on_s3dec_clicked()
{
    QString currentText = ui->s3->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok) {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s3->setText(newText);
    }
    else
    {
        ui->s3->setText("00");
    }
}



////////////////////////////////////////
////// Slot 4 Implementations //////////
////////////////////////////////////////


void MainWindow::on_s4ON_clicked()
{
    ui->s4->setText("FF");
}

void MainWindow::on_s4OFF_clicked()
{
    ui->s4->setText("00");
}

void MainWindow::on_s4inc_clicked()
{
    QString currentText = ui->s4->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s4->setText(newText);
    }
    else
    {
        ui->s4->setText("00");
    }
}

void MainWindow::on_s4dec_clicked()
{
    QString currentText = ui->s4->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s4->setText(newText);
    }
    else
    {
        ui->s4->setText("00");
    }
}


////////////////////////////////////////
////// Slot 5 Implementations //////////
////////////////////////////////////////


void MainWindow::on_s5ON_clicked()
{
    ui->s5->setText("FF");
}

void MainWindow::on_s5OFF_clicked()
{
    ui->s5->setText("00");
}

void MainWindow::on_s5inc_clicked()
{
    QString currentText = ui->s5->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s5->setText(newText);
    }
    else
    {
        ui->s5->setText("00");
    }
}

void MainWindow::on_s5dec_clicked()
{
    QString currentText = ui->s5->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s5->setText(newText);
    }
    else
    {
        ui->s5->setText("00");
    }
}


////////////////////////////////////////
////// Slot 6 Implementations //////////
////////////////////////////////////////


void MainWindow::on_s6ON_clicked()
{
    ui->s6->setText("FF");
}

void MainWindow::on_s6OFF_clicked()
{
    ui->s6->setText("00");
}

void MainWindow::on_s6inc_clicked()
{
    QString currentText = ui->s6->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s6->setText(newText);
    }
    else
    {
        ui->s6->setText("00");
    }
}

void MainWindow::on_s6dec_clicked()
{
    QString currentText = ui->s6->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok) {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s6->setText(newText);
    }
    else
    {
        ui->s6->setText("00");
    }
}


////////////////////////////////////////
////// Slot 7 Implementations //////////
////////////////////////////////////////


void MainWindow::on_s7ON_clicked()
{
    ui->s7->setText("FF");
}

void MainWindow::on_s7OFF_clicked()
{
    ui->s7->setText("00");
}

void MainWindow::on_s7inc_clicked()
{
    QString currentText = ui->s7->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s7->setText(newText);
    }
    else
    {
        ui->s7->setText("00");
    }
}

void MainWindow::on_s7dec_clicked()
{
    QString currentText = ui->s7->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s7->setText(newText);
    }
    else
    {
        ui->s7->setText("00");
    }
}


////////////////////////////////////////
////// Slot 8 Implementations //////////
////////////////////////////////////////


void MainWindow::on_s8ON_clicked()
{
    ui->s8->setText("FF");
}

void MainWindow::on_s8OFF_clicked()
{
    ui->s8->setText("00");
}

void MainWindow::on_s8inc_clicked()
{
    QString currentText = ui->s8->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s8->setText(newText);
    }
    else
    {
        ui->s8->setText("00");
    }
}

void MainWindow::on_s8dec_clicked()
{
    QString currentText = ui->s8->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        QString newText = QString::number(currentValue, 16).toUpper();
        ui->s8->setText(newText);
    }
    else
    {
        ui->s8->setText("00");
    }
}


////////////////////////////////////////////////////////////
//////////////////// END OF SLOT BUTTONS////////////////////
////////////////////////////////////////////////////////////

void MainWindow::on_idinc_clicked()
{
    QString currentText = ui->id->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue++;
        // Convert the incremented value to a hexadecimal string and pad with leading zeros to ensure 3 characters
        QString newText = QString("%1").arg(currentValue, 3, 16, QChar('0')).toUpper();
        ui->id->setText(newText);
    }
    else
    {
        ui->id->setText("000");
    }
}


void MainWindow::on_iddec_clicked()
{
    QString currentText = ui->id->text();
    bool ok;
    int currentValue = currentText.toInt(&ok, 16);

    if (ok)
    {
        currentValue--;
        if (currentValue < 0)
        {
            currentValue = 0;
        }
        // Convert the decremented value to a hexadecimal string and pad with leading zeros to ensure 3 characters
        QString newText = QString("%1").arg(currentValue, 3, 16, QChar('0')).toUpper();
        ui->id->setText(newText);
    }
    else
    {
        ui->id->setText("000");
    }
}


void MainWindow::onTextChanged()
{
    QLineEdit *senderLineEdit = qobject_cast<QLineEdit*>(sender());
    if (senderLineEdit) {
        QString text = senderLineEdit->text();
        if (senderLineEdit == ui->id)
        {
            // For the 'id' QLineEdit
            QRegularExpression validator("^[0-9A-Fa-f]*$");
            if (!validator.match(text).hasMatch())
            {
                text = text.left(3);
            }
        }
        else
        {

            if (text.length() > 2)
            {
                text = text.left(2);
            }
            QRegularExpression validator("^[0-9A-Fa-f]*$");
            if (!validator.match(text).hasMatch())
            {
                text = text.left(text.length() - 1); // Remove the last character if it's invalid
            }
        }
        senderLineEdit->setText(text); // Update the QLineEdit with the validated text
    }
}




void MainWindow::on_setmessage_clicked()
{
    // Read the ID from the id QLineEdit and convert it to an integer
    bool ok;
    int idValue = ui->id->text().toInt(&ok, 16); // Convert the text to an integer, base 16 for hexadecimal
    if (!ok)
    {
        // Handle the case where the conversion failed (e.g., the text is not a valid hexadecimal number)
        QMessageBox::warning(this, "Error", "Invalid ID format. Please enter a valid hexadecimal number.");
        return;
    }

    TPCANMsg CANMsg;
    CANMsg.ID = idValue;
    CANMsg.LEN = 8;

    // Function to convert QLineEdit text to BYTE and set it in CANMsg.DATA
    auto setDataByte = [&](QLineEdit* lineEdit, BYTE& dataByte)
    {
        QString text = lineEdit->text();
        bool ok;
        int value = text.toInt(&ok, 16); // Convert the text to an integer, base 16 for hexadecimal
        if (ok)
        {
            dataByte = static_cast<BYTE>(value); // Set the byte to the converted value
        }
        else
        {
            // Handle the case where the conversion failed (e.g., the text is not a valid hexadecimal number)
            dataByte = 0x00; // Example: set to 0x00 if the conversion fails
        }
    };

    // Set CANMsg.DATA[0] to CANMsg.DATA[7] based on the text in s1 to s8
    setDataByte(ui->s1, CANMsg.DATA[0]);
    setDataByte(ui->s2, CANMsg.DATA[1]);
    setDataByte(ui->s3, CANMsg.DATA[2]);
    setDataByte(ui->s4, CANMsg.DATA[3]);
    setDataByte(ui->s5, CANMsg.DATA[4]);
    setDataByte(ui->s6, CANMsg.DATA[5]);
    setDataByte(ui->s7, CANMsg.DATA[6]);
    setDataByte(ui->s8, CANMsg.DATA[7]);

    CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;

    QString messageString;
    for (int i = 0; i < CANMsg.LEN; ++i)
    {
        messageString += QString("%1 ").arg(CANMsg.DATA[i], 2, 16, QChar('0'));
    }

    QMessageBox::information(nullptr, "Message", "Message set: " + messageString);

    // Specify the file path to the desktop
    QString filePath = QDir::homePath() + "/Desktop/MessageSent.txt";
    QFile file(filePath);


    if (!file.exists()) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Could not create file for writing.";
        }
        file.close();
    }

    //Append File
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "Message ID: " << QString::number(idValue, 16).toUpper() << "\n";
        out << "Message: " << messageString << "\n";
        file.close();
    }
    else
    {
        qDebug() << "Could not open file for appending.";
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(file.fileName()));
}



void MainWindow::on_savemessage_clicked()
{

    // Prompt the user for a comment
    bool ok;
    QString comment = QInputDialog::getText(this, "Comment", "Enter your comment:", QLineEdit::Normal, "", &ok);
    if (!ok)
    {
        return;
    }

    bool check;
    int idValue = ui->id->text().toInt(&check, 16);
    if (!check)
    {
        QMessageBox::warning(this, "Error", "Invalid ID format. Please enter a valid hexadecimal number.");
        return;
    }

    TPCANMsg CANMsg;
    CANMsg.ID = idValue;
    CANMsg.LEN = 8;


    auto setDataByte = [&](QLineEdit* lineEdit, BYTE& dataByte)
    {
        QString text = lineEdit->text();
        bool ok;
        int value = text.toInt(&ok, 16);
        if (ok)
        {
            dataByte = static_cast<BYTE>(value);
        }
        else
        {
            dataByte = 0x00;
        }
    };

    setDataByte(ui->s1, CANMsg.DATA[0]);
    setDataByte(ui->s2, CANMsg.DATA[1]);
    setDataByte(ui->s3, CANMsg.DATA[2]);
    setDataByte(ui->s4, CANMsg.DATA[3]);
    setDataByte(ui->s5, CANMsg.DATA[4]);
    setDataByte(ui->s6, CANMsg.DATA[5]);
    setDataByte(ui->s7, CANMsg.DATA[6]);
    setDataByte(ui->s8, CANMsg.DATA[7]);

    CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;

    QString messageString;
    for (int i = 0; i < CANMsg.LEN; ++i)
    {
        messageString += QString("%1 ").arg(CANMsg.DATA[i], 2, 16, QChar('0'));
    }
    QString filePath = QDir::homePath() + "/Desktop/SuccessfulMessage.txt";
    QFile file(filePath);


    if (!file.exists()) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Could not create file for writing.";
            return;
        }
        file.close();
    }

    // Append File
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "Message ID: " << QString::number(idValue, 16).toUpper() << "\n";
        out << "Message: " << messageString << "\n";
        out << "Comment: " << comment << "\n\n";
        file.close();
    }
    else
    {
        // Handle the case where the file could not be opened for appending
        qDebug() << "Could not open file for appending.";
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(file.fileName()));

    // Optionally, inform the user that the message and comment have been saved
    QMessageBox::information(this, "Success", "Message and comment have been saved.");
}

void MainWindow::on_sendmessage_clicked()
{

    qDebug() << "Button clicked";

    bool ok;
    int idValue = ui->id->text().toInt(&ok, 16);
    if (!ok)
    {
        QMessageBox::warning(this, "Error", "Invalid ID format. Please enter a valid hexadecimal number.");
        return;
    }


    TPCANMsg CANMsg;
    CANMsg.ID = idValue;
    CANMsg.LEN = 8;

    auto setDataByte = [&](QLineEdit* lineEdit, BYTE& dataByte)
    {
        QString text = lineEdit->text();
        bool ok;
        int value = text.toInt(&ok, 16);
        if (ok)
        {
            dataByte = static_cast<BYTE>(value);
        }
        else
        {
            dataByte = 0x00;
        }
    };

    setDataByte(ui->s1, CANMsg.DATA[0]);
    setDataByte(ui->s2, CANMsg.DATA[1]);
    setDataByte(ui->s3, CANMsg.DATA[2]);
    setDataByte(ui->s4, CANMsg.DATA[3]);
    setDataByte(ui->s5, CANMsg.DATA[4]);
    setDataByte(ui->s6, CANMsg.DATA[5]);
    setDataByte(ui->s7, CANMsg.DATA[6]);
    setDataByte(ui->s8, CANMsg.DATA[7]);

    CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;


    sendCANMessage(CANMsg);
}

void MainWindow::sendCANMessage(TPCANMsg& CANMsg)
{
    TPCANStatus stsResult;
    TPCANHandle channel = PCAN_USBBUS1;

    stsResult = CAN_Write(channel, &CANMsg);
    if (stsResult != PCAN_ERROR_OK) {

        qDebug() << "Failed to send the CAN message. Error code: " << stsResult;


        if (stsResult == PCAN_ERROR_BUSOFF)
        {
            TPCANStatus resetResult = CAN_Reset(channel);
            if (resetResult != PCAN_ERROR_OK)
            {
                qDebug() << "Failed to reset the CAN bus. Error code: " << resetResult;
                QSystemTrayIcon trayIcon(this);
                trayIcon.showMessage("Error", "Failed to reset the CAN bus. Error code: " + QString::number(resetResult), QSystemTrayIcon::Warning);
            }
            else
            {
                qDebug() << "CAN bus has been reset.";
                QSystemTrayIcon trayIcon(this);
                trayIcon.showMessage("Info", "CAN bus has been reset. Please retry sending the message.", QSystemTrayIcon::Information);
            }
        }

        QSystemTrayIcon trayIcon(this);
        trayIcon.showMessage("Error", "Failed to send the CAN message. Error code: " + QString::number(stsResult), QSystemTrayIcon::Warning);

        return;
    }

    QMessageBox::information(this, "Success", "Message has been sent.");
    qDebug() << "SENT CAN";
}



void MainWindow::on_brute_clicked()
{

    bruteThread->start();
}


void MainWindow::on_stop_clicked()
{
    bruteWorker->stopBrute();
}





