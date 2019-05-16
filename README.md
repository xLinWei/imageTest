# imageTest
该项目是基于ZYNQ的图像测试平台，其包括：

- 在PC上搭建QT GUI
- 通过QTcpSocket传输图片到ZYNQ PS
- ZYNQ通过LWIP接收图片并处理(该项目做翻转处理)，然后传回PC

可在该测试平台的基础上添加自己的图像处理算法，搭建项目。

## Deve Tool&Board
- Tool: vivado、Visual Studio、Qt、OpenCv
- Board: MicroZed 7020

## Build Project
### Step1:Build VS Project

- 在VS中新建QT GUI Application(需提前安装QT和Qt VS Tools)，并勾选Network
- 将工程命名为tcpTest，并将“tcpTest/”目录内的三个文件覆盖工程内文件
- 重新编译.ui文件和工程

### Step2:Build Vivado Project

- 在Vivado新建工程，命名为imageTest
- 新建block design，添加ZYNQ IP
<div align=left><img src=image\bd.jpg width=40%></div>

- Generat bitstream，然后导出SDK
- 在SDK里新建工程，并选择LWIP模板
<div align=left><img src=image\lwip.jpg width=50%></div>

- 将"image_test"目录内文件替换SDK工程内文件
- 将myparam.h的H_Image、W_Image修改成测试图片的大小

## Test 
- 连接网线，将电脑IP设为"192.168.1.100"
- 运行QT GUI、下载bit流、运行SDK工程
- 点击"连接以太网"按钮，连接以太网
- 点击"send"按钮选择发送的图片,查看运行效果
<div align=left><img src=image\result.jpg width=50%></div>
