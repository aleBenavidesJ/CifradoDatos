#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QRegularExpression>
#include <cryptlib.h>
#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <files.h>
#include <aes.h>
#include <modes.h>
#include <osrng.h>
#include <filters.h>
#include <secblock.h>
#include <string>

using namespace std;
using namespace CryptoPP;

void MainWindow::generateRSAKeys()
{
    CryptoPP::AutoSeededRandomPool prng;

    // Generar claves RSA
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(prng, 3072);

    privateKey = CryptoPP::RSA::PrivateKey(params);
    publicKey = CryptoPP::RSA::PublicKey(params);

    // Opcional: guardar las claves en archivos (solo para referencia)
    CryptoPP::Base64Encoder privKeysink(new CryptoPP::FileSink("private.key"));
    privateKey.DEREncode(privKeysink);
    privKeysink.MessageEnd();

    CryptoPP::Base64Encoder pubKeysink(new CryptoPP::FileSink("public.key"));
    publicKey.DEREncode(pubKeysink);
    pubKeysink.MessageEnd();
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::saveToJson);
    connect(ui->lineEdit, &QLineEdit::textEdited, this, &MainWindow::autoFormatCedula);
    connect(ui->lineEdit_3, &QLineEdit::textEdited, this, &MainWindow::autoFormatTelefono);
    connect(ui->lineEdit_4, &QLineEdit::textEdited, this, &MainWindow::autoFormatTarjeta);
    connect(ui->lineEdit_5, &QLineEdit::textEdited, this, &MainWindow::autoFormatExpiracion);
    connect(ui->lineEdit_6, &QLineEdit::textEdited, this, &MainWindow::autoFormatCVV);
    setStyleSheet(
        "QWidget { background-color: #f0f0f0; }"
        "QLineEdit { padding: 8px; border: 1px solid #ccc; border-radius: 4px; }"
        "QPushButton { background-color: #4CAF50; color: white; padding: 10px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #45a049; }"
        );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveToJson()
{
    QString filePath = R"(/home/braulio/Datos/CifradoDatos/users.json)";
    QString encryptedFilePath = R"(/home/braulio/Datos/CifradoDatos/encrypted_users.json)";
    QFile file(filePath);
    QFile encryptedFile(encryptedFilePath);
    QJsonArray usersArray;
    QJsonArray encryptedArray;

    // Leer archivo original
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        if (!doc.isNull() && doc.isArray()) {
            usersArray = doc.array();
        }
    }

    // Leer archivo encriptado
    if (encryptedFile.exists() && encryptedFile.open(QIODevice::ReadOnly)) {
        QByteArray fileData = encryptedFile.readAll();
        encryptedFile.close();

        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        if (!doc.isNull() && doc.isArray()) {
            encryptedArray = doc.array();
        }
    }

    QJsonObject newUser;
    QJsonObject encryptedUser;

    QString cedulaQT = ui->lineEdit->text();
    QString nombreQT = ui->lineEdit_1->text();
    QString apellidosQT = ui->lineEdit_2->text();
    QString telefonoQT = ui->lineEdit_3->text();
    QString numeroTarjetaQT = ui->lineEdit_4->text();
    QString fechaExpiracionQT = ui->lineEdit_5->text();
    QString cvvQT = ui->lineEdit_6->text();

    string cedula = ui->lineEdit->text().toStdString();
    string nombre = ui->lineEdit_1->text().toStdString();
    string apellidos = ui->lineEdit_2->text().toStdString();
    string telefono = ui->lineEdit_3->text().toStdString();
    string numeroTarjeta = ui->lineEdit_4->text().toStdString();
    string fechaExpiracion = ui->lineEdit_5->text().toStdString();
    string cvv = ui->lineEdit_6->text().toStdString();

    if (cedulaQT.isEmpty() || nombreQT.isEmpty() || apellidosQT.isEmpty() || telefonoQT.isEmpty() || numeroTarjetaQT.isEmpty() || fechaExpiracionQT.isEmpty() || cvvQT.isEmpty()) {
        statusBar()->showMessage("Por favor, complete todos los campos.", 3000);
        return;
    }

    // Guardar datos sin cifrar
    newUser["cedula"] = cedulaQT;
    newUser["nombre"] = nombreQT;
    newUser["apellidos"] = apellidosQT;
    newUser["telefono"] = telefonoQT;
    newUser["numero_tarjeta"] = numeroTarjetaQT;
    newUser["fecha_expiracion"] = fechaExpiracionQT;
    newUser["cvv"] = cvvQT;

    usersArray.append(newUser);

    AutoSeededRandomPool prng;

    InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(prng, 2048);

    RSA::PrivateKey privateKey(params);
    RSA::PublicKey publicKey(params);
    RSAES_OAEP_SHA_Encryptor encryptor(publicKey);


    std::string cipherCedula, cipherNombre, cipherApellidos, cipherTelefono, cipherTarjeta, cipherExpiracion, cipherCVV;
    // Cifrar datos y guardar

    // Cedula
    StringSource(cedula, true, new PK_EncryptorFilter(prng, encryptor, new StringSink(cedula)));
    encryptedUser["cedula"] = QString::fromStdString(cedula);

    // Nombre
    StringSource(nombre, true, new PK_EncryptorFilter(prng, encryptor, new StringSink(nombre)));
    encryptedUser["nombre"] = QString::fromStdString(nombre);

    // Apellidos
    StringSource(apellidos, true, new PK_EncryptorFilter(prng, encryptor, new StringSink(apellidos)));
    encryptedUser["apellidos"] = QString::fromStdString(apellidos);

    // Telefono
    StringSource(telefono, true, new PK_EncryptorFilter(prng, encryptor, new StringSink(telefono)));
    encryptedUser["telefono"] = QString::fromStdString(telefono);

    // Tarjeta
    StringSource(numeroTarjeta, true, new PK_EncryptorFilter(prng, encryptor, new StringSink(numeroTarjeta)));
    encryptedUser["numero_tarjeta"] = QString::fromStdString(numeroTarjeta);

    // Expiracion
    StringSource(fechaExpiracion, true, new PK_EncryptorFilter(prng, encryptor, new StringSink(fechaExpiracion)));
    encryptedUser["fecha_expiracion"] = QString::fromStdString(fechaExpiracion);

    // CVV
    StringSource(cvv, true, new PK_EncryptorFilter(prng, encryptor, new StringSink(cvv)));
    encryptedUser["cvv"] = QString::fromStdString(cvv);

    encryptedArray.append(encryptedUser);

    // Escribir datos sin cifrar
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(usersArray);
        file.write(doc.toJson());
        file.close();
    }

    // Escribir datos cifrados
    if (encryptedFile.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(encryptedArray);
        encryptedFile.write(doc.toJson());
        encryptedFile.close();
    }

    statusBar()->showMessage("Usuario registrado exitosamente.", 3000);

    // Limpiar campos
    ui->lineEdit->clear();
    ui->lineEdit_1->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
}


void MainWindow::autoFormatCedula(const QString &text)
{
    static const QRegularExpression onlyDigits("[^0-9]");
    QString formattedText = text;
    formattedText.remove(onlyDigits);
    formattedText.remove(' ');
    formattedText = formattedText.left(9);
    if (formattedText.length() > 1) formattedText.insert(1, ' ');
    if (formattedText.length() > 6) formattedText.insert(6, ' ');
    ui->lineEdit->blockSignals(true);
    ui->lineEdit->setText(formattedText);
    ui->lineEdit->setCursorPosition(formattedText.length());
    ui->lineEdit->blockSignals(false);
}

void MainWindow::autoFormatTelefono(const QString &text)
{
    static const QRegularExpression onlyDigits("[^0-9]");
    QString formattedText = text;
    formattedText.remove(onlyDigits);
    formattedText.remove(' ');
    formattedText = formattedText.left(8);
    if (formattedText.length() > 4) formattedText.insert(4, ' ');
    ui->lineEdit_3->blockSignals(true);
    ui->lineEdit_3->setText(formattedText);
    ui->lineEdit_3->setCursorPosition(formattedText.length());
    ui->lineEdit_3->blockSignals(false);
}

void MainWindow::autoFormatTarjeta(const QString &text)
{
    static const QRegularExpression onlyDigits("[^0-9]");
    QString formattedText = text;
    formattedText.remove(onlyDigits);
    formattedText.remove(' ');
    formattedText = formattedText.left(16);
    if (formattedText.length() > 4) formattedText.insert(4, ' ');
    if (formattedText.length() > 9) formattedText.insert(9, ' ');
    if (formattedText.length() > 14) formattedText.insert(14, ' ');
    ui->lineEdit_4->blockSignals(true);
    ui->lineEdit_4->setText(formattedText);
    ui->lineEdit_4->setCursorPosition(formattedText.length());
    ui->lineEdit_4->blockSignals(false);
}

void MainWindow::autoFormatExpiracion(const QString &text)
{
    static const QRegularExpression onlyDigits("[^0-9]");
    QString formattedText = text;
    formattedText.remove(onlyDigits);
    formattedText.remove('/');
    formattedText = formattedText.left(4);
    if (formattedText.length() > 2) formattedText.insert(2, '/');
    ui->lineEdit_5->blockSignals(true);
    ui->lineEdit_5->setText(formattedText);
    ui->lineEdit_5->setCursorPosition(formattedText.length());
    ui->lineEdit_5->blockSignals(false);
}

void MainWindow::autoFormatCVV(const QString &text)
{
    static const QRegularExpression onlyDigits("[^0-9]");
    QString formattedText = text;
    formattedText.remove(onlyDigits);
    formattedText = text.left(3);
    ui->lineEdit_6->blockSignals(true);
    ui->lineEdit_6->setText(formattedText);
    ui->lineEdit_6->setCursorPosition(formattedText.length());
    ui->lineEdit_6->blockSignals(false);
}
