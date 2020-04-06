#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMenu>
#include "modbusobject.h"
#include "configwidget.h"
#include "config.h"

namespace Ui {
class MainWidget;
}

typedef std::vector<uint8_t> CoilValueVec;
typedef std::vector<uint16_t> RegValueVec;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void update_server(QStringList list);

private slots:
    void on_pushButton_get_clicked();
    void on_pushButton_set_clicked();
    void open_config_wnd();
    void switch_to_client();
    void switch_to_server();

private:
    Ui::MainWidget *ui;

    ModBusObject *m_obj;            //as client
    ModBusServer *m_server;         //as server

    ConfigWidget* m_cfgWnd;

    std::vector<QAction*> m_vecpAction;


    void set_server_addrs();
};

#endif // MAINWIDGET_H
