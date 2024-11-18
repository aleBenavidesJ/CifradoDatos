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
    QString filePath = R"(C:\Users\benav\OneDrive\Documentos\Datos2\CifradoDatos\users.json)";
    QFile file(filePath);
    QJsonArray usersArray;

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        if (!doc.isNull() && doc.isArray()) {
            usersArray = doc.array();
        }
    }

    QJsonObject newUser;
    newUser["cedula"] = ui->lineEdit->text();
    newUser["nombre"] = ui->lineEdit_1->text();
    newUser["apellidos"] = ui->lineEdit_2->text();
    newUser["telefono"] = ui->lineEdit_3->text();
    newUser["numero_tarjeta"] = ui->lineEdit_4->text();
    newUser["fecha_expiracion"] = ui->lineEdit_5->text();
    newUser["cvv"] = ui->lineEdit_6->text();

    if (newUser["cedula"].toString().isEmpty() ||
        newUser["nombre"].toString().isEmpty() ||
        newUser["apellidos"].toString().isEmpty() ||
        newUser["telefono"].toString().isEmpty() ||
        newUser["numero_tarjeta"].toString().isEmpty() ||
        newUser["fecha_expiracion"].toString().isEmpty() ||
        newUser["cvv"].toString().isEmpty())
    {
        statusBar()->showMessage("Por favor, complete todos los campos.", 3000);
        return;
    }

    usersArray.append(newUser);

    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(usersArray);
        file.write(doc.toJson());
        file.close();
        qDebug() << "Datos guardados en:" << filePath;
        statusBar()->showMessage("Usuario registrado exitosamente.", 3000);
        ui->lineEdit->clear();
        ui->lineEdit_1->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
    } else {
        statusBar()->showMessage("Error al guardar los datos.", 3000);
        qDebug() << "Error al abrir el archivo para escritura en:" << filePath;
    }
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
