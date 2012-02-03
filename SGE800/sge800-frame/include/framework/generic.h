/******************************************************************************
	��Ŀ����	��SGE800���������ն�ҵ��ƽ̨
	�ļ�		��generic.h
	����		�����ļ�������ͨ�÷���ӿ�
	�汾		��0.1
	����		����迡
	��������	��2010.12
******************************************************************************/

#ifndef _GENERIC_H
#define _GENERIC_H

#include "base.h"

/*************************************************
  �ṹ���Ͷ���
*************************************************/


/*************************************************
  API
*************************************************/
/******************************************************************************
*	����:	get_crc32
*	����:	��Ϣ����ģ���ʼ��
*	����:	crc				-	crc��ʼֵ�����¼���Ϊ0��
			buf				-	������
			len				-	���㳤��
*	����:	>=0				-	crc���
*	˵��:
 ******************************************************************************/
u32 get_crc32 (u32 crc, u8 *buf, u32 len);


#endif