#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_tcpTest.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>


class tcpTest : public QMainWindow
{
	Q_OBJECT

public:
	tcpTest(QWidget *parent = Q_NULLPTR);

private slots:
	void link_clicked();
	void send_clicked();
	void ReadMessage();

private:
	Ui::tcpTestClass ui;
	QTcpSocket *_tcpClient;
	int _cols;
	int _rows;
	int _len;
};
