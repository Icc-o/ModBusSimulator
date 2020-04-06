#include "configwidget.h"
#include "ui_configwidget.h"
#include "config.h"

ConfigWidget::ConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
    setWindowTitle("Configs");

    ClientConfig* pClient = Config::getPtr()->get_cli_cfg();
    ui->lineEdit_ip->setText(QString::fromStdString(pClient->servevrIp));
    ui->lineEdit_port->setText(QString::number(pClient->serverPort));
}


ConfigWidget::~ConfigWidget()
{
    delete ui;
}

void ConfigWidget::on_buttonBox_accepted()
{
    ClientConfig* pClient = Config::getPtr()->get_cli_cfg();
    pClient->servevrIp = ui->lineEdit_ip->text().toStdString();
    pClient->serverPort = ui->lineEdit_port->text().toInt();
    Config::getPtr()->set_is_server(ui->comboBox_clientOrSer->currentIndex() == 1);
    setVisible(false);
}

void ConfigWidget::on_buttonBox_rejected()
{
    setVisible(false);
}
