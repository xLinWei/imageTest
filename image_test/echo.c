/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/tcp.h"
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif

////////////////////////////////////////////////////////////
#include "xparameters.h"
#include "xil_cache.h"
#include "mypara.h"
int RecvDone = 0; ////判断数据是否接收完毕
int ProcessDone=0;////判断图片是否处理
struct tcp_pcb *tp;
////////////////////////////////////////////////////////////
////图像的处理在该函数内定义
int ImageProcess()
{
	////这里是简单的图像翻转
	int i;
	u8 *TxBufferPtr=(u8 *)TX_BUFFER_BASE;
	u8 *RxBufferPtr=(u8 *)RX_BUFFER_BASE;
	for(i=0;i<MAX_PKT_LEN;i++)
	{
		TxBufferPtr[i]=255-RxBufferPtr[i];////进行图像反转
	}
}

int transfer_data() {
	err_t err;
	u16_t len;
	static int num = 0;//已发送数据量
	static u8 *SendBufferPtr=(u8 *) TX_BUFFER_BASE;////未发送数据起始地
	if((RecvDone==1)&&(ProcessDone==0))//LWIP接收完毕
	{
		ImageProcess();
		ProcessDone=1;
	}
	if(ProcessDone){	//开始发送
		if(num<MAX_PKT_LEN){
			if((MAX_PKT_LEN-num)<tcp_sndbuf(tp))    ////一次发送tcp_sndbuf(tb)字节数据，不足的额外发送
				len=MAX_PKT_LEN-num;
			else
				len=tcp_sndbuf(tp);
			err = tcp_write(tp,SendBufferPtr,len , 1);  ////发送数据包，但不是立即发出去
			if (err != ERR_OK) {
				xil_printf("Error on tcp_write: %d\r\n", err);
				return err;
			}
			err = tcp_output(tp);////立即发出数据包
			if (err != ERR_OK) {
				xil_printf("Error on tcp_output: %d\r\n",err);
				return err;
			}
			SendBufferPtr=SendBufferPtr+len;
			num=num+len;
		}
		else{
			if(num!=MAX_PKT_LEN)
				xil_printf("Send failed \r\n");
			//else
			//	xil_printf("Send success \r\n");
			num=0;		////重置变量，以便下次接收数据
			RecvDone=0;
			ProcessDone=0;
			SendBufferPtr=(u8 *) TX_BUFFER_BASE;  ////这里是DMA的RX_Buffer,LWIP的TX_Buffer
		}

	}
	return 0;
}

void print_app_header()
{
	xil_printf("\n\r\n\r-----lwIP TCP echo server ------\n\r");
	xil_printf("TCP packets sent to port 6001 will be echoed back\n\r");
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err)
{
	tp=tpcb;
	static int num = 0;//统计接收数据量
	u8 *RxBufferPtr=(u8 *) RX_BUFFER_BASE;

	/* do not read the packet if we are not in ESTABLISHED state */
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);

	/* echo back the payload */
	/* in this case, we assume that the payload is < TCP_SND_BUF */
	//if (tcp_sndbuf(tpcb) > p->len) {
	//	err = tcp_write(tpcb, p->payload, p->len, 1);
	//} else
	//	xil_printf("no space in tcp_sndbuf\n\r");

	memcpy(RxBufferPtr+num,(u8 *)p->payload,p->len);
	num += p->len;
	xil_printf("num=%d\n",num);
	if(num==MAX_PKT_LEN){
		xil_printf("Receive success\n\r");
		RecvDone = 1;
		Xil_DCacheFlushRange((u32)RxBufferPtr, MAX_PKT_LEN);//把Cache里的数据刷到DDR中
		num = 0;
	}
	if(num > MAX_PKT_LEN){
		xil_printf("Receive error\n\r");
		num = 0;
	}

	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	static int connection = 1;

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);

	/* just use an integer number indicating the connection id as the
	   callback argument */
	tcp_arg(newpcb, (void*)(UINTPTR)connection);

	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}


int start_application()
{
	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 7;

	/* create new TCP PCB structure */
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	xil_printf("TCP echo server started @ port %d\n\r", port);

	return 0;
}
