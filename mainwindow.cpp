#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QMessageBox>
#include <QPixmap>
#include <QLineEdit>
#include <QFile>
#include <QByteArray>
#include <QCryptographicHash>
#include <openssl/aes.h>
#include <openssl/rand.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit_3->setEchoMode(QLineEdit::Password);
    ui->lineEdit->setText("ip");
    ui->lineEdit_3->setText("password");
    ui->lineEdit_4->setText("port");
    ui->lineEdit_2->setText("ssh-user");
    ui->lineEdit_5->setText("anon-user");
    ui->plainTextEdit->setPlainText("type message here");
    ui->lineEdit_6->setText("secure-password");

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()),
            this, SLOT(MyTimerSlot()));

    int rate = 21000;

    timer->start(rate);

    ui->textBrowser->setTextColor(Qt::green);
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool encryptFile(const QString &filePath, const QByteArray &key)
{
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open file for reading.");
        return false;
    }

    QByteArray fileData = inputFile.readAll();
    inputFile.close();

    unsigned char iv[AES_BLOCK_SIZE];
    if (!RAND_bytes(iv, AES_BLOCK_SIZE)) {
        qWarning("Failed to generate IV.");
        return false;
    }

    QByteArray hashKey = QCryptographicHash::hash(key, QCryptographicHash::Sha256).left(32); // 256-bit key
    AES_KEY encryptKey;
    AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(hashKey.constData()), 256, &encryptKey);

    int padding = AES_BLOCK_SIZE - (fileData.size() % AES_BLOCK_SIZE);
    fileData.append(padding, padding);

    QByteArray encryptedData;
    encryptedData.append(reinterpret_cast<const char*>(iv), AES_BLOCK_SIZE); // Prepend IV

    int numBlocks = fileData.size() / AES_BLOCK_SIZE;
    unsigned char buffer[AES_BLOCK_SIZE];
    for (int i = 0; i < numBlocks; ++i) {
        AES_cbc_encrypt(reinterpret_cast<const unsigned char*>(fileData.constData()) + i * AES_BLOCK_SIZE,
                        buffer, AES_BLOCK_SIZE, &encryptKey, iv, AES_ENCRYPT);
        encryptedData.append(reinterpret_cast<const char*>(buffer), AES_BLOCK_SIZE);
    }

    QFile outputFile(filePath + ".enc");
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qWarning("Failed to open file for writing.");
        return false;
    }
    outputFile.write(encryptedData);
    outputFile.close();

    return true;
}


bool decryptFile(const QString &filePath, const QByteArray &key)
{
    // Open the encrypted file for reading
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open encrypted file for reading.");
        return false;
    }

    QByteArray encryptedData = inputFile.readAll();
    inputFile.close();

    if (encryptedData.size() < AES_BLOCK_SIZE) {
        qWarning("Encrypted file is too short to contain an IV and data.");
        return false;
    }

    unsigned char iv[AES_BLOCK_SIZE];
    memcpy(iv, encryptedData.constData(), AES_BLOCK_SIZE);

    QByteArray hashKey = QCryptographicHash::hash(key, QCryptographicHash::Sha256).left(32); //256-bit key
    AES_KEY decryptKey;
    AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(hashKey.constData()), 256, &decryptKey);

    QByteArray decryptedData;
    int numBlocks = (encryptedData.size() - AES_BLOCK_SIZE) / AES_BLOCK_SIZE;
    unsigned char buffer[AES_BLOCK_SIZE];
    for (int i = 0; i < numBlocks; ++i) {
        AES_cbc_encrypt(reinterpret_cast<const unsigned char*>(encryptedData.constData()) + AES_BLOCK_SIZE + i * AES_BLOCK_SIZE,
                        buffer, AES_BLOCK_SIZE, &decryptKey, iv, AES_DECRYPT);
        decryptedData.append(reinterpret_cast<const char*>(buffer), AES_BLOCK_SIZE);
    }

    int padding = decryptedData.at(decryptedData.size() - 1);
    decryptedData.chop(padding);

    QFile outputFile(filePath + ".dec");
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qWarning("Failed to open file for writing.");
        return false;
    }
    outputFile.write(decryptedData);
    outputFile.close();

    return true;
}

void MainWindow::on_pushButton_clicked()
{
    ui->textBrowser->setTextColor(Qt::green);

    QString ip = ui->lineEdit->text();

    QString user = ui->lineEdit_2->text();

    QString passwd = ui->lineEdit_3->text();

    QString port = ui->lineEdit_4->text();


    QString chatname = ui->lineEdit_5->text();

    const QString qPath131("server.sh");
    QFile qFile131(qPath131);
    if (qFile131.open(QIODevice::WriteOnly)) {
        QTextStream out131(&qFile131);
        out131 << "sshpass -p "+passwd+" scp -P "+port+" ./chat.tmp.enc "+user+"@"+ip+":./";
        qFile131.close();
    }



    const QString qPath1315("autoread.sh");
    QFile qFile1315(qPath1315);
    if (qFile1315.open(QIODevice::WriteOnly)) {
        QTextStream out1315(&qFile1315);
        out1315 << "sshpass -p "+passwd+" scp -P "+port+" "+user+"@"+ip+":./chat.tmp.enc ./";
        qFile1315.close();
    }



    QString chat = ui->plainTextEdit->toPlainText();

    const QString qPath1311("chat.tmp");
    QFile qFile1311(qPath1311);
    if (qFile1311.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream out1311(&qFile1311);
        out1311 << chatname << " says: "+chat;
        out1311 << "\n";
        qFile1311.close();
    }


    QString filePath = "chat.tmp";

    QString encryptkeystring = ui->lineEdit_6->text();
    QByteArray key = encryptkeystring.toUtf8();
    if (encryptFile(filePath, key)) {
        qDebug("File encrypted successfully.");
    } else {
        qWarning("File encryption failed.");
    }


    int systemRet = system("sh server.sh");
    if(systemRet == -1){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->insertPlainText("sh command failed!\n");
    }


}


void MainWindow::on_pushButton_4_clicked()
{

    int systemRet = system("rm *.tmp *.dec *.sh");
    if(systemRet == -1){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->insertPlainText("rm command failed!\n");
    }

    QCoreApplication::quit();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    int systemRet = system("rm *.tmp *.dec *.sh");
    if(systemRet == -1){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->insertPlainText("rm command failed!\n");
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    const QString qPath1311("chat.tmp");
    QFile qFile1311(qPath1311);
    if (qFile1311.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out1311(&qFile1311);
        out1311 << "chat was cleared on the server!\n";
        qFile1311.close();
    }

    int systemRet = system("sh server.sh");
    if(systemRet == -1){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->insertPlainText("sh command failed!\n");
    }

    ui->textBrowser->setText("chat deleted!\n");

}

void MainWindow::MyTimerSlot()
{
    ui->textBrowser->setTextColor(Qt::green);

    QString ip = ui->lineEdit->text();

    QString user = ui->lineEdit_2->text();

    QString passwd = ui->lineEdit_3->text();

    QString port = ui->lineEdit_4->text();

    const QString qPath1315("autoread.sh");
    QFile qFile1315(qPath1315);
    if (qFile1315.open(QIODevice::WriteOnly)) {
        QTextStream out1315(&qFile1315);
        out1315 << "sshpass -p "+passwd+" scp -P "+port+" "+user+"@"+ip+":./chat.tmp.enc ./";
        qFile1315.close();
    }

    system("sh autoread.sh");


    QString encryptedFilePath = "chat.tmp.enc";

    QString encryptkeystring = ui->lineEdit_6->text();

    QByteArray key = encryptkeystring.toUtf8();
    if (decryptFile(encryptedFilePath, key)) {
        qDebug("File decrypted successfully.");
    } else {
        qWarning("File decryption failed.");
    }


    QString chatread;
    QFile inputFile491("chat.tmp.enc.dec");
    if (inputFile491.open(QIODevice::ReadOnly))
    {
        QTextStream inp491(&inputFile491);
        chatread = inp491.readAll();
        // inp491 >> chatread;
        inputFile491.close();

    }


    ui->textBrowser->setText(chatread);



}
