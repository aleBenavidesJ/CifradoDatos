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

void MainWindow::generateRSAKeys()
{
    CryptoPP::AutoSeededRandomPool rng;

    // Generar claves RSA
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 3072);

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

QString MainWindow::encryptData(const QString &data)
{
    CryptoPP::AutoSeededRandomPool rng;

    // Generar clave AES e IV
    CryptoPP::SecByteBlock key(CryptoPP::AES::DEFAULT_KEYLENGTH); // 16 bytes
    CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);         // 16 bytes
    rng.GenerateBlock(key, key.size());
    rng.GenerateBlock(iv, iv.size());

    qDebug() << "Tamaño de la clave AES:" << key.size();
    qDebug() << "Tamaño del IV:" << iv.size();

    // Cifrar datos con AES
    std::string plainText = data.toStdString();
    std::string cipherText;

    try {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption aesEncryption(key, key.size(), iv);
        CryptoPP::StringSource ss(plainText, true,
                                  new CryptoPP::StreamTransformationFilter(aesEncryption,
                                                                           new CryptoPP::StringSink(cipherText)
                                                                           )
                                  );
    } catch (const CryptoPP::Exception &e) {
        qDebug() << "Error en cifrado AES:" << e.what();
        return "";
    }

    qDebug() << "Tamaño de los datos cifrados con AES:" << cipherText.size();

    // Cifrar clave AES con RSA usando SHA-256 en OAEP
    std::string encryptedKey, encryptedIV;
    try {
        CryptoPP::RSAES_OAEP_SHA_Encryptor rsaEncryptor(publicKey, new CryptoPP::SHA256());

        // Cifrar clave AES
        CryptoPP::StringSource ss1(std::string((const char*)key.data(), key.size()), true,
                                   new CryptoPP::PK_EncryptorFilter(rng, rsaEncryptor,
                                                                    new CryptoPP::StringSink(encryptedKey)
                                                                    )
                                   );

        // Cifrar IV
        CryptoPP::StringSource ss2(std::string((const char*)iv.data(), iv.size()), true,
                                   new CryptoPP::PK_EncryptorFilter(rng, rsaEncryptor,
                                                                    new CryptoPP::StringSink(encryptedIV)
                                                                    )
                                   );
    } catch (const CryptoPP::Exception &e) {
        qDebug() << "Error en cifrado RSA:" << e.what();
        qDebug() << "Tamaño total de datos a cifrar:" << key.size() + iv.size();
        return "";
    }

    qDebug() << "Cifrado RSA exitoso.";

    // Combinar clave encriptada, IV y datos cifrados en Base64
    std::string encoded;
    CryptoPP::StringSource ss3(encryptedKey + encryptedIV + cipherText, true,
                               new CryptoPP::Base64Encoder(
                                   new CryptoPP::StringSink(encoded)
                                   )
                               );

    return QString::fromStdString(encoded);
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

    QString cedula = ui->lineEdit->text();
    QString nombre = ui->lineEdit_1->text();
    QString apellidos = ui->lineEdit_2->text();
    QString telefono = ui->lineEdit_3->text();
    QString numeroTarjeta = ui->lineEdit_4->text();
    QString fechaExpiracion = ui->lineEdit_5->text();
    QString cvv = ui->lineEdit_6->text();

    if (cedula.isEmpty() || nombre.isEmpty() || apellidos.isEmpty() || telefono.isEmpty() || numeroTarjeta.isEmpty() || fechaExpiracion.isEmpty() || cvv.isEmpty()) {
        statusBar()->showMessage("Por favor, complete todos los campos.", 3000);
        return;
    }

    // Guardar datos sin cifrar
    newUser["cedula"] = cedula;
    newUser["nombre"] = nombre;
    newUser["apellidos"] = apellidos;
    newUser["telefono"] = telefono;
    newUser["numero_tarjeta"] = numeroTarjeta;
    newUser["fecha_expiracion"] = fechaExpiracion;
    newUser["cvv"] = cvv;

    usersArray.append(newUser);

    // Cifrar datos y guardar
    //encryptedUser["cedula"] = encryptData(cedula);
    //encryptedUser["nombre"] = encryptData(nombre);
    //encryptedUser["apellidos"] = encryptData(apellidos);
    //encryptedUser["telefono"] = encryptData(telefono);
    //encryptedUser["numero_tarjeta"] = encryptData(numeroTarjeta);
    //encryptedUser["fecha_expiracion"] = encryptData(fechaExpiracion);
    encryptedUser["cvv"] = encryptData(cvv);

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
