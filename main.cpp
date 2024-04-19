#include "mainwindow.h"
#include "PCANBasic.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog> // Include this for QInputDialog

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    TPCANBaudrate Baudrate;

    // Prompt the user for the desired baud rate
    bool ok;
    QString input = QInputDialog::getText(nullptr, "Baud Rate Selection","Enter the desired baud rate (125 or 500):", QLineEdit::Normal,"", &ok);
    if (!ok || input.isEmpty()) {
        QMessageBox::critical(nullptr, "Error", "Baud rate not specified.");
        return -1;
    }

    // Set the Baudrate based on user input
    if (input.toInt() == 125) {
        Baudrate = PCAN_BAUD_125K;
    } else if (input.toInt() == 500) {
        Baudrate = PCAN_BAUD_500K;
    } else {
        QMessageBox::critical(nullptr, "Error", "Invalid baud rate specified.");
        return -1;
    }

    MainWindow w(Baudrate); // Passing the Channel and Baudrate to the MainWindow constructor
    w.show();


    // Start the Qt event loop
    return a.exec();
}
