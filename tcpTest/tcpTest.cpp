#include "tcpTest.h"
#include <opencv2/opencv.hpp>
using namespace cv;

tcpTest::tcpTest(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	_tcpClient = new QTcpSocket(this);   //ʵ����tcpClient

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(link_clicked()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(send_clicked()));

	connect(_tcpClient, SIGNAL(readyRead()), this, SLOT(ReadMessage()));	
}


void tcpTest::link_clicked()
{
	if (ui.pushButton->text() == QStringLiteral("������̫��"))
	{
		//ui.textEdit->append(QStringLiteral("**���Ƚ�����IP��Ϊ192.168.1.100������"));
		_tcpClient->connectToHost("192.168.1.10", 7);
		if (_tcpClient->waitForConnected(20))
		{
			ui.textEdit->append(QStringLiteral("���ӳɹ�......"));
			ui.pushButton->setText(QStringLiteral("�Ͽ���̫��"));
		}
		else
			ui.textEdit->append(QStringLiteral("����ʧ��......"));
	}
	else
	{
		_tcpClient->disconnectFromHost();
		if (_tcpClient->state() == QAbstractSocket::UnconnectedState \
			|| _tcpClient->waitForDisconnected(20))
		{
			ui.pushButton->setText(QStringLiteral("������̫��"));
			ui.textEdit->append(QStringLiteral("�ѶϿ�����......"));
		}
	}
}

void tcpTest::send_clicked()
{
	Mat img = imread("img.jpg");
	cvtColor(img,img,CV_BGR2GRAY);
	_cols = img.cols;
	_rows = img.rows;
	_len = img.cols * img.rows;
	//QImage pic(img.data, img.cols, img.rows, img.cols*img.channels(),QImage::Format_Grayscale8);

	//QByteArray block((char *)img.data);
	/*QDataStream out(&block, QIODevice::ReadWrite);
	out << pic;
	out.device()->seek(0);*/

	qint64 l=_tcpClient->write((char*)img.data, _len);

	//imshow("test",img);
}

void tcpTest::ReadMessage()
{
	static QByteArray block;
	QByteArray temp;

	temp=_tcpClient->read(_len);
	if (block.size() < _len) 
	{
		block.append(temp);
	}
	if(block.size()==_len)
	{
		//QPixmap pic;
		ui.textEdit->append(QStringLiteral("�������......"));
		//pic.loadFromData(block);
		//ui.label->setPixmap(pic);

		QImage pic((uchar *)block.data(), _cols, _rows, _cols*1, QImage::Format_Grayscale8);
		ui.label->setPixmap(QPixmap::fromImage(pic));
		block.clear();
	}
}