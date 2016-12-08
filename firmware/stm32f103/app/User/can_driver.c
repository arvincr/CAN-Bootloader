/**
  ******************************************************************************
  * @file    can_driver.c
  * $Author: wdluo $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   CAN�����շ���غ���.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can_app.h"
/* Private typedef -----------------------------------------------------------*/
typedef  struct {
  unsigned char   SJW;
  unsigned char   BS1;
  unsigned char   BS2;
  unsigned short  PreScale;
} tCAN_BaudRate;
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
CanRxMsg CAN1_RxMessage;
volatile uint8_t CAN1_CanRxMsgFlag=0;//���յ�CAN���ݺ�ı�־
volatile uint8_t TimeOutFlag;				///<��ʱ����ʱ��־
tCAN_BaudRate  CAN_BaudRateInitTab[]= {      // CLK=36MHz
   {CAN_SJW_1tq,CAN_BS1_10tq,CAN_BS2_1tq,3},     // 1M
   {CAN_SJW_1tq,CAN_BS1_8tq,CAN_BS2_1tq,4},     // 900K
   {CAN_SJW_2tq,CAN_BS1_13tq,CAN_BS2_1tq,13},     // 800K
   {CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_1tq,3},     // 666K
   {CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_1tq,4},     // 600K
   {CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_1tq,4},     // 500K
   {CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_1tq,5},     // 400K
   {CAN_SJW_1tq,CAN_BS1_15tq,CAN_BS2_1tq,7},    // 300K
   {CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_1tq,8},    // 250K
   {CAN_SJW_1tq,CAN_BS1_14tq,CAN_BS2_1tq,10},	// 225K
   {CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_1tq,10},    // 200K
   {CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_1tq,15},	// 160K
   {CAN_SJW_1tq,CAN_BS1_14tq,CAN_BS2_1tq,15},    // 150K
   {CAN_SJW_1tq,CAN_BS1_8tq,CAN_BS2_1tq,25},	// 144K
   {CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_1tq,36},   // 125K
   {CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_1tq,20},	// 120K
   {CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_1tq,45},    // 100K
   {CAN_SJW_1tq,CAN_BS1_14tq,CAN_BS2_1tq,25},   // 90K
   {CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_1tq,25},   // 80K
   {CAN_SJW_1tq,CAN_BS1_14tq,CAN_BS2_1tq,30},	// 75K
   {CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_1tq,40},    // 60K
   {CAN_SJW_1tq,CAN_BS1_14tq,CAN_BS2_1tq,45},    // 50K
   {CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_1tq,50},    // 40K
   {CAN_SJW_1tq,CAN_BS1_14tq,CAN_BS2_1tq,75},   // 30K
   {CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_1tq,100},   // 20K
};
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CANʱ�����ñ���CLK=36MHz 
  */
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,   6,     // 1M     36/(1+3+2)/6 =1M  
// CAN_SJW_1tq, CAN_BS1_4tq , CAN_BS2_3tq,   5,     // 900K   36/(1+4+3)/5 =0.9  
// CAN_SJW_1tq, CAN_BS1_5tq , CAN_BS2_3tq,   5,     // 800K   36/(1+5+3)/5 =0.8  
// CAN_SJW_1tq, CAN_BS1_6tq , CAN_BS2_3tq,   6,     // 600K   36/(1+6+3)/6=0.6  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  12,     // 500K     
// CAN_SJW_1tq, CAN_BS1_5tq , CAN_BS2_3tq,  10,     // 400K     
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  20,     // 300K   36/(1+3+2)/20  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  24,     // 250K     
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  30,     // 200K    
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  40,     // 150K   36/(1+3+2)/40  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  48,     // 125K   36/(1+3+2)/48  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  60,     // 100K     
// CAN_SJW_1tq, CAN_BS1_4tq , CAN_BS2_3tq,  50,     //  90K   36/(1+4+3)/50 =0.09  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  75,     //  80K    
// CAN_SJW_1tq, CAN_BS1_6tq , CAN_BS2_3tq,  60,     //  60K    
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq, 120,     //  50K    
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq, 150,     //  40K   
// CAN_SJW_1tq, CAN_BS1_6tq , CAN_BS2_3tq, 120,     //  30K    
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq, 300,     //  20K       
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq, 600,     //  10K  
// CAN_SJW_2tq, CAN_BS1_6tq , CAN_BS2_4tq, 600,     //  5K   36/(2+6+4)/600=5K   
// CAN_SJW_2tq, CAN_BS1_6tq , CAN_BS2_4tq, 1000,    //  3K   36/(2+6+4)/1000  
// CAN_SJW_2tq, CAN_BS1_10tq, CAN_BS2_6tq, 1000     //  2K   36/(2+10+6)/1000=2K   

/**
  * @brief  ͨ�������ʵ�ֵ��ȡ�����ʲ���������ֵ
  * @param  BaudRate CAN���߲����ʣ���λΪbps
  * @retval �����ʲ���������ֵ
  */
uint32_t CAN_GetBaudRateNum(uint32_t BaudRate)
{
    switch(BaudRate){
        case 1000000 :return 0;
        case 900000 :return 1;
        case 800000 :return 2;
        case 666000 :return 3;
        case 600000 :return 4;
        case 500000 :return 5;
        case 400000 :return 6;
        case 300000 :return 7;
        case 250000 :return 8;
        case 225000:return 9;
        case 200000 :return 10;
        case 160000:return 11;
        case 150000 :return 12;
        case 144000:return 13;
        case 125000 :return 14;
        case 120000:return 15;
        case 100000 :return 16;
        case 90000 :return 17;
        case 80000 :return 18;
        case 75000:return 19;
        case 60000 :return 20;
        case 50000 :return 21;
        case 40000 :return 22;
        case 30000 :return 23;
        case 20000 :return 24;
        default:return 0;
    }
}

/**
  * @brief  TIM��ʱ�����ã�����CAN���ݷ��ͳ�ʱ�ж�
  * @param  None
  * @retval None
  */
void BOOT_TIM_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// ʹ������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	/* ʱ�Ӽ���Ƶ���� */
	/* 0.5ms */
	TIM_TimeBaseStructure.TIM_Prescaler = 36000-1;// Ԥ��Ƶ����Ƶ���Ƶ��Ϊ2K   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //����ģʽ:���ϼ���
	// TIM������ֵ��������ʱʱ�䳤�ȣ�
	TIM_TimeBaseStructure.TIM_Period =0;	  // ��������ֵ�����ϼ���ʱ����������ֵ��������¼���ʱ���Ӹ�ֵ��ʼ����
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // �������˲����Ĳ������й�       
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //���¼�������ʼֵ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TimeOutFlag = 0;
	
	/* Disable the TIM Counter */
	TIM2->CR1 &= (uint16_t)(~TIM_CR1_CEN);

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //��������ж�
	
	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;							// �ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	// ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					// �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 						// ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);		
}
/**
  * @brief  ��ʱ
  * @param  ms ��ʱ������
  * @retval None
  */
void BOOT_DelayMs(uint32_t ms)
{
	TimeOutFlag = 0;
	TIM2->ARR = ms;
	/* Enable the TIM Counter */
	TIM2->CR1 |= (uint16_t)TIM_CR1_CEN;	
	while(!TimeOutFlag);
	/* Disable the TIM Counter */
	TIM2->CR1 &= (uint16_t)(~TIM_CR1_CEN);
}
/**
  * @brief  TIM��ʱ����ʱ�жϴ�������
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)== SET)
	{
		TimeOutFlag = 1;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //��������־
	}
}


/**
  * @brief  CAN��������
  * @param  None
  * @retval None
  */
void CAN_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   	
  	/*����ʱ������*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* Configure CAN pin: RX PA11*/									          
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	/* Configure CAN pin: TX PA12 */									               
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}
/**
  * @brief  CAN�����ж�����
  * @param  None
  * @retval None
  */
void CAN_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable CAN1 RX0 interrupt IRQ channel */	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  ����CAN���չ�����
  * @param  FilterNumber ��������
  * @param  can_addr CAN�ڵ��ַ���ò����ǳ���Ҫ��ͬһ��CAN����������ڵ��ַ�����ظ�
  * @retval None
  */
void CAN_ConfigFilter(uint8_t FilterNumber,uint16_t can_addr)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	//����CAN���չ�����
  CAN_FilterInitStructure.CAN_FilterNumber=FilterNumber;//������1
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;//����λģʽ
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//32bitģʽ
	//����4��Ϊ0��ʾ�����κ�����
  CAN_FilterInitStructure.CAN_FilterIdHigh=can_addr>>(16-CMD_WIDTH-3);
  CAN_FilterInitStructure.CAN_FilterIdLow=(can_addr<<(CMD_WIDTH+3))|0x04;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=ADDR_MASK>>(16-CMD_WIDTH-3);;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=(ADDR_MASK<<(CMD_WIDTH+3))|0x04;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//ʹ�ܹ�����
  CAN_FilterInit(&CAN_FilterInitStructure);	
}
/**
  * @brief  ��ʼ��CAN
  * @param  BaudRate CAN���߲�����
  * @retval None
  */
void CAN_Configuration(uint32_t BaudRate)
{
  CAN_InitTypeDef        CAN_InitStructure;
  /* CAN register init */
  CAN_NVIC_Configuration();
  CAN_GPIO_Configuration();
	
	CAN_Address_GPIO_Config();

  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = ENABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_BaudRateInitTab[CAN_GetBaudRateNum(BaudRate)].SJW;//���ò�����Ϊ1M
  CAN_InitStructure.CAN_BS1 = CAN_BaudRateInitTab[CAN_GetBaudRateNum(BaudRate)].BS1;
  CAN_InitStructure.CAN_BS2 = CAN_BaudRateInitTab[CAN_GetBaudRateNum(BaudRate)].BS2;
  CAN_InitStructure.CAN_Prescaler = CAN_BaudRateInitTab[CAN_GetBaudRateNum(BaudRate)].PreScale;

  CAN_Init(CAN1,&CAN_InitStructure);
	//����CAN���չ�����
	CAN_ConfigFilter(0,0x00);//�㲥��ַ�����ܹ㲥����
	CAN_ConfigFilter(1,CAN_BOOT_GetAddrData());//���ڵ���ʵ��ַ
	
	//ʹ�ܽ����ж�
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
	//���ö�ʱ��
	BOOT_TIM_Config();
}


/**
  * @brief  ����һ֡CAN����
  * @param  CANx CANͨ����
	* @param  TxMessage CAN��Ϣָ��
  * @retval None
  */
uint8_t CAN_WriteData(CanTxMsg *TxMessage)
{
	uint8_t TransmitMailbox;   
	uint32_t	TimeOut=0;
  TransmitMailbox = CAN_Transmit(CAN1,TxMessage);
	while(CAN_TransmitStatus(CAN1,TransmitMailbox)!=CAN_TxStatus_Ok){
		TimeOut++;
		if(TimeOut > 10000000){
			return 1;
		}
	}
	return 0;
}
/**
  * @brief  CAN�����жϴ�������
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CAN_Receive(CAN1,CAN_FIFO0, &CAN1_RxMessage);
  CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
	CAN1_CanRxMsgFlag = 1;
}

/**
  * @brief  ��ȡCAN�ڵ��ַ���ú��������Լ���ʵ����������޸�
  * @param  None
  * @retval None
  */
uint16_t Read_CAN_Address(void)
{
  return 0x123;//���صĵ�ֵַ��Ҫ����ʵ����������޸�
}

/**
  * @brief  ��ʼ�����û�ȡCAN���ߵ�ַ��GPIO����
  * @param  None
  * @retval None
  */
void CAN_Address_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* config GPIOE clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	/* CANADDRESS GPIO config */
	/* Configure PE0-PE6 */
	GPIO_DeInit(GPIOE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU /*GPIO_Mode_IN_FLOATING*/;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);    	  
}

/*********************************END OF FILE**********************************/