#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QMenu>
#include <QLayout>
#include <QToolButton>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    m_obj(nullptr),
    m_server(nullptr)
{
    ui->setupUi(this);
    ui->lineEdit_valueRead->setEnabled(false);

    Config::getPtr()->init();

    m_cfgWnd = new ConfigWidget();
    m_cfgWnd->setVisible(false);

    //这里的选项可以从配置文件里面去配置好
    m_vecpAction.push_back(new QAction("Config", this));
    m_vecpAction.push_back(new QAction(tr("Work as client"), this));
    m_vecpAction.push_back(new QAction(tr("Work as server"), this));

    connect(m_vecpAction[0], SIGNAL(triggered()), this, SLOT(open_config_wnd()));
    connect(m_vecpAction[1], SIGNAL(triggered()), this, SLOT(switch_to_client()));
    connect(m_vecpAction[2], SIGNAL(triggered()), this, SLOT(switch_to_server()));

    for(auto& a: m_vecpAction)
    {
        addAction(a);
    }
    setContextMenuPolicy(Qt::ActionsContextMenu);


//    m_obj = new ModBusObject(TCP);
//    m_obj->connect("127.0.0.1", 1502);
//    CoilValueVec values{1,1,1,1,1,1,1,1};
//    m_obj->write_coil(304,values);
}

MainWidget::~MainWidget()
{
    for(auto& p: m_vecpAction)
        delete p;
    delete m_cfgWnd;
    delete Config::getPtr();
    delete ui;
}

void MainWidget::on_pushButton_get_clicked()
{
    if(nullptr == m_obj)
        return;
    int startAddr = ui->lineEdit_addressRead->text().toInt();
    int count = ui->lineEdit_countRead->text().toInt();
    CoilValueVec coilValues;
    RegValueVec  regValues;
    QString result;
    if(ui->comboBox_typeRead->currentIndex() == 0)
    {
        //AI
        m_obj->read_register(startAddr,count,regValues);
        for(auto &a: regValues)
        {
            result+=QString::number(a)+".";
        }
        result.remove(result.lastIndexOf("."),1);
    }
    else
    {
        //DI
        m_obj->read_coil(startAddr,count,coilValues);
        for(auto &a: coilValues)
        {
            result+=QString::number(a)+".";
        }
    }

    ui->lineEdit_valueRead->setText(result);
}

void MainWidget::on_pushButton_set_clicked()
{
    if(nullptr == m_obj)
        return;
    int startAddr = ui->lineEdit_addressWrite->text().toInt();
    int count = ui->lineEdit_countWrite->text().toInt();
    QString values = ui->lineEdit_valueWrite->text();

    RegValueVec regValuesVec;
    CoilValueVec coillValuesVec;

    QStringList valuesList = values.split(".");

    if(ui->comboBox_type_2->currentIndex() == 0)
    {
        //AI
        for(int i=0; i<valuesList.size(); i++)
        {
            regValuesVec.push_back(atoi(valuesList.at(i).toStdString().c_str()));
        }
        m_obj->write_register(startAddr, regValuesVec);
    }
    else
    {
        //DI
        for(int i=0; i<valuesList.size(); i++)
        {
            coillValuesVec.push_back(atoi(valuesList.at(i).toStdString().c_str()));
        }
        m_obj->write_coil(startAddr, coillValuesVec);
    }
}


void MainWidget::open_config_wnd()
{
    m_cfgWnd->show();
}

void MainWidget::switch_to_client()
{
    ui->stackedWidget->setCurrentIndex(0);
    ClientConfig* client = Config::getPtr()->get_cli_cfg();
    if(m_server)
    {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
    if(m_obj == nullptr)
        m_obj = new ModBusObject(client->type);
    client->isConnected = m_obj->connect(client->servevrIp, client->serverPort);
    ui->label_connected->setText(client->isConnected?"Y":"N");

}

void MainWidget::switch_to_server()
{
    ui->stackedWidget->setCurrentIndex(1);
    //ServerConfig
    if(m_server == nullptr)
    {
        m_server = new ModBusServer(TCP);
    }
    std::string ip = "127.0.0.1";
    m_server->init(ip, 1502);       //可以改成从配置中读取
    if(m_obj)
    {
        m_obj->disconnect();
        delete m_obj;
        m_obj = nullptr;
    }
    m_server->set_update_func(std::bind(&MainWidget::update_server, this, std::placeholders::_1));

    set_server_addrs();
}

void MainWidget::update_server(QStringList list)
{
    if(list.size()<3)
        return;
    ui->lineEdit_bits->setText(list[0]);
    ui->lineEdit_inputBits->setText(list[1]);
    ui->lineEdit_registers->setText(list[2]);
    ui->lineEdit_inputRegisters->setText(list[3]);
}

void MainWidget::set_server_addrs()
{
    if(m_server == nullptr)
        return;
    //获取起始地址并显示
    std::vector<uint16_t> addrVec;
    m_server->get_start_addrs(addrVec);
    if(addrVec.size() != 4)
        return;
    ui->label_bits->setText(ui->label_bits->text()+QString::number(addrVec[0]));
    ui->label_inputBits->setText(ui->label_inputBits->text()+QString::number(addrVec[1]));
    ui->label_registers->setText(ui->label_registers->text()+QString::number(addrVec[2]));
    ui->label_inputRegisters->setText(ui->label_inputRegisters->text()+QString::number(addrVec[3]));
}
