# USART编写逻辑以及代码理解
## 1. USART Establish
1. 开 GPIO 时钟
2. 开 USART 时钟
3. 配置 GPIO 为复用模式
4. 配置 GPIO 复用功能 AF
5. 配置 USART 参数          常用的配置是115200,8N1（8位数据位，No parity无校验位，1位停止位）   所以发送一个字节线路上将会传输10个bite
6. 开 USART 接收中断 
7. 配置 NVIC
8. 使能 USART
9. 接收不同类型字符的函数需要自己写，发送函数不需要自己写

## 2. USART的部分组成
GPIO复用 + USART参数 + NVIC中断 + 回调函数

## 3. 在stm32f407中常见的USART搭配
USART1：PA9 / PA10，挂在 APB2

USART2：PA2 / PA3，挂在 APB1

GPIOA：挂在AHB1

## 4. 字节与比特
**1个字节（byte）= 8个比特（bite）**；字符 'A' 在 ASCII 码中是：十进制：65；二进制：0100 0001；十六进制：0x41

## 5. RXNE 和TXE
**RXNE看接收：有没有数据可以读；TXE 看发送：有没有空位可以写**
```c
//接收数据寄存器非空，也就是USART 已经收到了一个字节，这个字节现在放在接收数据寄存器里，还没有被你读走
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);						        //在读取到这个字节之后，RXNE会自动清除
    }
```
```c
//发送数据寄存器为空，也就是说：USART 的发送数据寄存器现在是空的，可以写入新的数据。
    void usart_send_byte(typedef_usart *usart, uint8_t data)
    {
        while (USART_GetFlagStatus(usart->USARTx, USART_FLAG_TXE) == RESET);
    
        USART_SendData(usart->USARTx, data);
    }
```

## 6. 代码分析
###  6.1 USART.h
```c   
    #define USART_RX_BUF_SIZE    128						            //定义缓冲区
    
    typedef struct   //定义
    {
        USART_TypeDef* USARTx;
    
        uint32_t USART_CLK;
    
        GPIO_TypeDef* GPIOx;
        uint32_t GPIO_CLK;
    
        uint16_t TX_Pin;
        uint16_t RX_Pin;
    
        uint8_t TX_PinSource;
        uint8_t RX_PinSource;
    
        uint8_t GPIO_AF;
    
        IRQn_Type IRQ_Channel;
    
        uint32_t BaudRate;
    }typedef_usart;
    
    
    //变量声明     在.c文件中已经定义过了
    extern volatile uint8_t usart1_rx_byte;
    extern char usart1_rx_buf[USART_RX_BUF_SIZE];
    extern volatile uint16_t usart1_rx_len;
    extern volatile uint8_t usart1_rx_flag;
    
    extern typedef_usart usart1;
    
    void usart_init(typedef_usart * usart);
    
    void usart_send_byte(typedef_usart * usart, uint8_t data);
    void usart_send_string(typedef_usart * usart, char* str);
    void usart_send_data(typedef_usart * usart,uint8_t * data, uint16_t len);
    
    void usart1_process_data(void);
    void usart1_clear_buffer(void);
```    
    
###    6.2 USART.c
```c
    //定义一个USART1的配置对象
    typedef_usart usart1 =       
    {
        .USARTx = USART1,      							            //使用USART1外设
        .USART_CLK = RCC_APB2Periph_USART1,
    
        .GPIOx = GPIOA,
        .GPIO_CLK = RCC_AHB1Periph_GPIOA,
    
        .TX_Pin = GPIO_Pin_9,							            //GPIO_Pin_9        -> 表示具体引脚，用于 GPIO_Init
        .RX_Pin = GPIO_Pin_10,							
    
        .TX_PinSource = GPIO_PinSource9,						    //GPIO_PinSource9   -> 表示引脚编号，用于GPIO_PinAFConfig
        .RX_PinSource = GPIO_PinSource10,
    
        .GPIO_AF = GPIO_AF_USART1,						            //将PA9，PA10复用为USART1
    
        .IRQ_Channel = USART1_IRQn,						            //USART1 的中断通道
    
        .BaudRate = 115200
    };
    
    volatile uint8_t usart1_rx_byte = 0;						    //表示当前接收到的一个字节，USART 中断每进来一次，通常说明收到 1 个字节。
    
    char usart1_rx_buf[USART_RX_BUF_SIZE] = {0};				    //这是接收缓冲区，用来保存一整串数据
    
    volatile uint16_t usart1_rx_len = 0;						    //表示当前已经接收了多少个字符
    
    volatile uint8_t usart1_rx_flag = 0;						    //这是接收完成标志位。0：当前还没接收完；1：一帧数据已经接收完成，可以处理了
    
    void usart_init (typedef_usart * usart)
    {
        RCC_AHB1PeriphClockCmd(usart->GPIO_CLK, ENABLE);            //将USART的时钟打开
        if (usart->USARTx == USART1 || usart->USARTx == USART6)  
        {
            RCC_APB2PeriphClockCmd(usart->USART_CLK, ENABLE);
        }
        else
        {
            RCC_APB1PeriphClockCmd(usart->USART_CLK, ENABLE);
        }
    
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;					//配置GPIO的输出模式    USART -> 推挽输出 PP，IIC   -> 开漏输出 OD
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;					//指定所选引脚的上拉/下拉操作
        GPIO_InitStruct.GPIO_Pin = usart->TX_Pin | usart->RX_Pin;   //这里的 | 是按位或，意思是一次配置两个引脚
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
        GPIO_Init(usart->GPIOx, &GPIO_InitStruct);
        
        //复用配置
        GPIO_PinAFConfig(usart->GPIOx, usart->RX_PinSource, usart->GPIO_AF);
        GPIO_PinAFConfig(usart->GPIOx, usart->TX_PinSource, usart->GPIO_AF);
        
       //对USART的配置
        USART_InitTypeDef USART_InitStruct;
        USART_InitStruct.USART_BaudRate = usart->BaudRate;				                    //配置波特率
        USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;        //不适用硬件控流
        USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		                //将USART的接收和发送同时打开
        USART_InitStruct.USART_Parity = USART_Parity_No;				                    //没有校验位
        USART_InitStruct.USART_StopBits = USART_StopBits_1;				                    //一个停止位
        USART_InitStruct.USART_WordLength = USART_WordLength_8b;			                //8位数据位
    
        USART_Init(usart->USARTx, &USART_InitStruct);
    
        //打开USART的接收中断
        USART_ITConfig(usart->USARTx, USART_IT_RXNE, ENABLE);			                    //USART 收到一个字节后，就触发中断，收到一个字符之后就会进入USART1_IRQHandler
    
        //NVIC初始化设置，NVIC 是 ARM Cortex-M 内核的中断控制器      因为之前让USART产生了中断，所以需要配置NVIC
        NVIC_InitTypeDef NVIC_InitStruct;
        NVIC_InitStruct.NVIC_IRQChannel = usart->IRQ_Channel;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;				                        //表示使能这个中断通道
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;				                //抢占优先级
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;				                        //相应优先级
        NVIC_Init(&NVIC_InitStruct);
        
        //使能USART
        USART_Cmd(usart->USARTx, ENABLE);
    }
    
    
    //发送一个字节
    void usart_send_byte(typedef_usart * usart, uint8_t data)
    {
        uint32_t timeout = 1000000;
    
        while (USART_GetFlagStatus(usart->USARTx, USART_FLAG_TXE) == RESET)		            //Transmit Data Register Empty发送数据寄存器为空。如果 TXE = 1，说明可以写入新数据
        {
            if (timeout-- == 0)							                                    //加入timeout，避免程序死卡
            {
                return;
            }
        }
    
        USART_SendData(usart->USARTx, data);					                            //发送信息
    }
    
    
    //逐个字符发送字符串
    void usart_send_string(typedef_usart * usart, char* str)
    {
        while(*str != '\0')
        {
            usart_send_byte(usart, *str);
            str++;
        }
    }
    
    //发送指定的数据长度
    void usart_send_data(typedef_usart * usart, uint8_t * data, uint16_t len)
    {
        uint16_t i;
    
        for (i = 0; i < len; i ++)
        {
            usart_send_byte(usart, data[i]);						                    //这个用于发送二进制数据，不依赖 '\0'。字符串发送函数遇到 '\0' 会停止。
        }
    }
    
    
    //清空缓冲区     作用：1. 清空接收数组  2. 长度归零  3. 接收完成标志清零
    void usart1_clear_buffer(void)
    {
        uint16_t i;
    
        for (i = 0; i < USART_RX_BUF_SIZE; i++)
        {
            usart1_rx_buf[i] = 0;
        }
    
        usart1_rx_len = 0;
        usart1_rx_flag = 0;
    }
    
    
    /*主循环处理接收数据
      逻辑是：如果 usart1_rx_flag == 1；说明一帧数据接收完成；然后把收到的内容发回电脑；最后清空缓冲区，准备下一次接收*/
    void usart1_process_data(void)
    {
        if (usart1_rx_flag == 1)
        {
            usart_send_string(&usart1, "Received: ");
            usart_send_string(&usart1, usart1_rx_buf);
            usart_send_string(&usart1, "\r\n");
            usart1_clear_buffer();
        }
    }
    
    
    //中断函数       因为开启了USART_ITConfig，所以 USART1 收到一个字节时，RXNE 置位，进入
    void USART1_IRQHandler(void)
    {
        uint8_t data;
    
        if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)			    //这次中断是不是因为接收到了数据，如果是就处理接收
        {
            data = USART_ReceiveData(USART1);					            //读取USART1 收到的数据，在 STM32 USART 里，读取 DR 寄存器通常会清除 RXNE 标志
    
            usart1_rx_byte = data;
    
            if(data == '\r' || data == '\n')						        //收到 \r 或 \n 表示一帧结束
            {
                if (usart1_rx_len > 0)
                {
                    usart1_rx_buf[usart1_rx_len] = '\0';
                    usart1_rx_flag = 1;
                }
            }
            else if(usart1_rx_flag == 0)						            //普通字符转入缓冲区
            {
                if (usart1_rx_len < USART_RX_BUF_SIZE - 1 )				    //最后要留位置给‘\0’，如果缓冲区是128，则最多村127个有效字符
                {
                    usart1_rx_buf[usart1_rx_len] = data;
                    usart1_rx_len++;
                }
                else
                {
                    usart1_clear_buffer();
                }
            }
        }
    }
    
    //给printf重定位
    int fputc(int ch, FILE *f)
    {
        usart_send_byte(&usart1, (uint8_t)ch);
        return ch;
    }
```
