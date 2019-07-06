#ifndef __SCHEDULE_H
#define __SCHEDULE_H

#include "sys.h"
typedef enum
{
  stop = 0, //����״̬
  claim_goods,   //ȡ��״̬
	track,      //�����Ĵ���
	back,
	showoff,
	still_run,
	report
}MachineState;

/* ʱ��ṹ��
 *open_box_time�� ���е������ʱ��
 *close_box_time���غе������ʱ��
 *show_time��     չʾʱ��
*/
typedef struct
{
  uint8_t open_box_time;
	uint8_t close_box_time;
	uint8_t show_time;
}mTime;
/* ʱ�������־
 *open_box_time_flag�� ���е������ʱ�����
 *close_box_flag���غе������ʱ�����
 *show_time_flag��     չʾʱ�����
*/
typedef struct
{
  uint8_t open_box_time_flag;
	uint8_t close_box_flag;
	uint8_t show_time_flag;
}mTimeflag;

/*���粿�ֽṹ��
 *server_ip:������IP��ַ
 *tcp_port���������Ķ˿�
 *tcp_socket��
 *dhcp_socket��
 *dns_socket:
 *ischange: 1��ʾ����Ҫ���ķ�����IP
*/
typedef struct
{
	uint8_t*  server_ip;
	uint16_t tcp_port; 
  uint8_t tcp_socket;
	uint8_t dhcp_socket;	
	uint8_t dns_socket;	
  uint8_t ischange;	
}NetConcig;
	
/* �豸�ṹ��
 *state������Ŀǰ��״̬
 *time�� ����״̬��ʱ��
*/
typedef struct
{
	char*  deviceid;
	char*  hard_ver;
	char*  soft_ver;
	mTime   time;
  uint8_t state;	
	mTimeflag timeflag;	
  NetConcig netcon;
}mMachine;

void Start_Schedule(void);
void Start_Repay(void);
void Start_Borrow(void);
void test(void);
#endif
