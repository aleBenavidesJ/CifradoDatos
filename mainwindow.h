#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QString>
#include <QByteArray>

#include <cryptlib.h>
#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <files.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void saveToJson();
    void autoFormatCedula(const QString &text);
    void autoFormatTelefono(const QString &text);
    void autoFormatTarjeta(const QString &text);
    void autoFormatExpiracion(const QString &text);
    void autoFormatCVV(const QString &text);

private:
    void generateRSAKeys();
    QString encryptData(const QString &data);

private:
    Ui::MainWindow *ui;
    CryptoPP::RSA::PublicKey publicKey;
    CryptoPP::RSA::PrivateKey privateKey;
};
#endif // MAINWINDOW_H
