/*
 * mypara.h
 *
 *  Created on: 2019Äê5ÔÂ7ÈÕ
 *      Author: Seaton
 */

#ifndef SRC_MYPARA_H_
#define SRC_MYPARA_H_


#define MEM_BASE_ADDR		0x01000000

#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)//0x00300000

#define H_Image 226
#define W_Image 218
#define MAX_PKT_LEN		H_Image*W_Image

#endif /* SRC_MYPARA_H_ */
