#include "BruteWorker.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QSystemTrayIcon>
#include <QThread>
#include <QStandardPaths>

BruteWorker::BruteWorker(QObject *parent)
    : QObject(parent), stopRequested(false)
{
}

BruteWorker::~BruteWorker()
{
}


void BruteWorker::startBrute()
{
    int id = 0x000; // Initialize id to 0x000
    TPCANHandle channel = PCAN_USBBUS1; // Example channel, adjust as necessary
    TPCANMsg CANMsg;
    CANMsg.LEN = 8; // Standard CAN message length

    // Specify the file path to BruteMessages.txt on the desktop
    QString filePath = QDir::homePath() + "/Desktop/BruteMessages.txt";
    QFile file(filePath);

    // Check if the file exists
    if (!file.exists())
    {
        // If the file does not exist, create it
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // Handle the case where the file could not be created
            qDebug() << "Could not create file for writing.";
            return; // Exit the function if the file cannot be created
        }
        file.close(); // Close the file after creating it
    }

    // Debugging: Print the file path to ensure it's correct
    qDebug() << "File path: " << filePath;




    // Loop until the "stop" button is pressed
    while (!stopRequested) {
        // Loop through each byte in message.DATA[]
        for (int byteIndex = 0; byteIndex < CANMsg.LEN; ++byteIndex)
        {
            // Set each message.DATA[] variable to 00
            memset(CANMsg.DATA, 0x00, CANMsg.LEN);
            // Convert the id to a hexadecimal string and ensure it's 3 characters long
            QString idHex = QString("%1").arg(id, 3, 16, QChar('0'));
            CANMsg.ID = id; // Use the id directly as an integer for the CAN message ID
            bruteCANMessage(channel, CANMsg);

            if (file.open(QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&file);
                out << "ID: " << idHex << " Message: ";
                for (int i = 0; i < CANMsg.LEN; ++i)
                {
                    out << QString("%1 ").arg(CANMsg.DATA[i], 2, 16, QChar('0'));
                }
                out << "\n";
                file.close();
            }
            else
            {
                qDebug() << "Could not open file for appending.";
            }

            // Change the message.DATA[] variable to FF and send the message with the same ID
            memset(CANMsg.DATA, 0xFF, CANMsg.LEN);
            bruteCANMessage(channel, CANMsg);

            // Increment the ID by 1
            id++;

            if (file.open(QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&file);
                out << "ID: " << idHex << " Message: ";
                for (int i = 0; i < CANMsg.LEN; ++i)
                {
                    out << QString("%1 ").arg(CANMsg.DATA[i], 2, 16, QChar('0'));
                }
                out << "\n";
                file.close();
            }
            else
            {
                // Handle the case where the file could not be opened for appending
                qDebug() << "Could not open file for appending.";
            }
            if (id > 0xFFF)
            {
                id = 0x000;
                break;
            }

        }
        if (id > 0xFFF)
        {
            id = 0x000;
            break;
        }
    }

    emit finished();

}


void BruteWorker::stopBrute()
{
    mutex.lock();
    stopRequested = true;
    mutex.unlock();
}

void BruteWorker::bruteCANMessage(TPCANHandle channel, TPCANMsg& CANMsg)
{
    TPCANStatus stsResult;

    // Send the message
    stsResult = CAN_Write(channel, &CANMsg);
    if (stsResult != PCAN_ERROR_OK)
    {
        // Log the error message to a console or a log file
        qDebug() << "Failed to send the CAN message. Error code: " << stsResult;

        QSystemTrayIcon trayIcon(this);
        trayIcon.showMessage("Error", "Failed to send the CAN message. Error code: " + QString::number(stsResult), QSystemTrayIcon::Warning);

        return;
    }

    qDebug() << "Sent CAN Message";
    QThread::msleep(500);
}
