/******************************************************************************************************************************************************************************************************************************************************** 
 * Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Headers includes
*********************************************************************************************************************************************************************************************************************************************************/
#include "General_types.h"
#include "Comm_appl.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Global Variables into this scope (this file *.c)
*********************************************************************************************************************************************************************************************************************************************************/
/* Tabela de todos os comandos disponíveis do ESP para o Slave. Formatação da tabela: {{SID,TYPE,ID}, byte, callback function} */
#ifdef _MODULE_TYPE_PLUGS
static const Kostia_CmdTable_t CmdTable_FromMasterToSlave[] = {
    {{0x01U, 0x01U, 0x01U}, 0x01U, Comm_appl_QueryID},       /* Query if slave is configured */
    {{0x02U, 0x01U, 0x01U}, 0x01U, Comm_appl_SetID},         /* Set ID to slave */
    {{0x03U, 0x02U, 0x01U}, 0x01U, Comm_appl_ConfigSlave},   /* Config slave command */
    {{0x04U, 0x01U, 0x01U}, 0x01U, Comm_appl_RequestData},   /* Request slave's data */
    {{0x00U, 0x00U, 0x00U}, 0x00U, Comm_appl_CmdTableError}  /* Must be the last element */
};
#endif

#ifdef _MODULE_TYPE_LIGTH
static const Kostia_CmdTable_t CmdTable_FromMasterToSlave[] = {
    {{0x01U, 0x01U, 0x01U}, 0x01U, Comm_appl_QueryID},       /* Query if slave is configured */
    {{0x02U, 0x01U, 0x01U}, 0x01U, Comm_appl_SetID},         /* Set ID to slave */
    {{0x03U, 0x03U, 0x01U}, 0x01U, Comm_appl_ConfigSlave},   /* Config slave command */
    {{0x04U, 0x01U, 0x01U}, 0x01U, Comm_appl_RequestData},   /* Request slave's data */
    {{0x00U, 0x00U, 0x00U}, 0x00U, Comm_appl_CmdTableError}  /* Must be the last element */
};
#endif


/******************************************************************************************************************************************************************************************************************************************************** 
    Função: Frame Send Machine (FSM)
    Descrição: Esta função é uma máquina de estados que controla o envio de frames
    \Parametros: Uart_t *pUart - estrutura de dados principal
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_FSM( Uart_t *pUart )
{
    switch (pUart->FSM_State){
        case FSM_State_Idle:
        {
            break;
        }
        case FSM_State_Send:   /* Start execute of frame sending on bus */
        {
            int TxBuff_Length;
            TxBuff_Length = Comm_appl_FrameToBuffer(pUart);
            Comm_protocol_Frame_Send_Request(&pUart->TxBuffer, TxBuff_Length);
            Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Sending);
            break;
        }
        case FSM_State_Sending:   /* Executing frame sending on bus */
        {
            if(Comm_protocol_Get_TxFIFO_Lenght() == 0){
                Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Idle);
            }
            break;
        }
        case FSM_State_Error:
        {
            /* ToDo[PENS] - error handler */
            Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Idle);
            break;
        } 
        default:
        {
            /* ToDo[PENS] - default handler */
        }
    }
    return 0;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função: Frame Receive Machine (FRM)
    Descrição: Esta função é uma máquina de estados que controla a recepção de frames
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_FRM(Uart_t *pUart)
{
    static int RxBuff_Timeout = 0;
    static int RxBuff_Length = 0;
    static int RxBuff_Length_Previous = 0;
    switch (pUart->FRM_State){
        case FRM_State_Idle:
        {
            if(Comm_protocol_Get_RxFIFO_Lenght() > 0){   /* Verifica a quantidade de bytes armazenados no Rx_Buffer. Se for > 0, então significa que há bytes chegando e é necessário alterar o estado da FRM */
                Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Receiving);
            }
            break;
        }
        case FRM_State_Receiving:
        {
            RxBuff_Length = Comm_protocol_Get_RxFIFO_Lenght();
            if(RxBuff_Length != RxBuff_Length_Previous){
                RxBuff_Length_Previous = RxBuff_Length;
                RxBuff_Timeout = 0;
            }else{
                RxBuff_Timeout++;
            }
            if(RxBuff_Timeout >= 3){   /* Timeout de 30ms */
                RxBuff_Timeout = 0;
                RxBuff_Length_Previous = 0;
                Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Received);
            }
            break;                                                                                                                                              
        }
        case FRM_State_Received:  /* This state get datas received in Rx serial buffer and put it in a SW buffer (pUart->RxBuffer) */
        {
            int nDataRead;
            nDataRead = Comm_protocol_Frame_Read_Request(&pUart->RxBuffer, RxBuff_Length);
            if(Comm_appl_Check_Frame_IsEcho(pUart) == FALSE){
                if(Comm_appl_Check_Frame_IsValid(pUart) == TRUE){
                    Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Idle);
                    Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Process);
                }else{
                    Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Error);
                }
            }else{
                Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Idle);
            }
            break;
        }
        case FRM_State_Error:
        {
            /* ToDo[PENS] error handler */
            Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Idle);
            break;
        }
        default:
        {
            /* ToDo[PENS] - error handler to FSRM */
        }
    }    
    return 0;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função: Response Handling Machine (RHM)
    Descrição: Esta função é uma máquina de estados que avalia o comando de entrada, recebido da Serial Rx, e manipula uma resposta
    \Parametros: Uart_t *pUart - estrutura de dados principal
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_RHM(Uart_t *pUart)
{
    switch (pUart->RHM_State){
        case RHM_State_Idle:
        {
            break;
        }
        case RHM_State_Process:
        {
            if(Comm_appl_FindCommand(&pUart->RxBuffer[_SID], pUart) == KOSTIA_OK){
                pUart->RHM_State = RHM_State_Idle;
                if(digitalRead(COMM_ENALE_IN) == LOW){
                    Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Send);   /* Ordem para a FSM (Frame Send Machine) transmitir um frame de resposta (previamente configurada em alguma das função da tabela de comandos)  */
                }
            }else{
                pUart->RHM_State = RHM_State_Idle;
            }
            break;
        }
        default:
        {
            /* ToDo[PENS] - error handler to RHM */
        }
    }
    return 0;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função realiza a mudança da variável de estado da FSM
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_FSM_State(Uart_t *pUart, FSM_States_t nextState)
{
    pUart->FSM_State = nextState;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função realiza a mudança da variável de estado da FRM
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_FRM_State(Uart_t *pUart, FRM_States_t nextState)
{
    pUart->FRM_State = nextState;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função realiza a mudança da variável de estado da RHM
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_RHM_State(Uart_t *pUart, RHM_States_t nextState)
{
    if(pUart->RHM_State != RHM_State_Process){
        pUart->RHM_State = nextState;
    }
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função configura os campos do Frame Header
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Init_Slave_Data(Uart_t *pUart, byte Type , byte Data_size)
{
    int i;
    /* Inicializar o Frame */
    Comm_appl_Set_Frame_Header(&pUart->frame, 0x00, 0x55, 0x00, Type, 0x00, 0x01, Data_size + 1); /* Function signature:  Comm_appl_Set_Frame_Header(Frame_t *pFrame, byte Break, byte Synch, byte SID, byte Type, byte Id_Source, byte Id_Target, byte Lenght) */
    for(i = 0; i < _FRAME_MAX_DATA_SIZE; i++){
        pUart->frame.Data[i] = 0x00;
    }
    Comm_appl_Set_Frame_Checksum(&pUart->frame);
    /* Inicializar os buffers que armazenam os dados TX e RX */
    for(i = 0; i < _RX_BUFFER_SIZE; i++){
        pUart->RxBuffer[i] = 0x00;
    }
    for(i = 0; i < _TX_BUFFER_SIZE; i++){
        pUart->TxBuffer[i] = 0x00;
    }
    /* Inicializar as máquinas de estado da UART */
    pUart->FSM_State = FSM_State_Idle;
    pUart->FRM_State = FRM_State_Idle;
    pUart->RHM_State = RHM_State_Idle;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função configura os campos do Frame Header
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Set_Frame_Header(Frame_t *pFrame, byte Break, byte Synch, byte SID, byte Type, byte Id_Source, byte Id_Target, byte Lenght)
{
    pFrame->Break = Break;
    pFrame->Synch = Synch;
    pFrame->SID = SID;
    pFrame->Type = Type;
    pFrame->Id_Source = Id_Source;
    pFrame->Id_Target = Id_Target;
    pFrame->Lenght = Lenght;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função configura o dados do Frame Data
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Set_Frame_Data(Frame_t *pFrame, byte *Data, int Size)
{
    for(int i=0; i<Size; i++){
        pFrame->Data[i] = *(Data + i);
    }
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função configura o valor do Frame Checksum
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Set_Frame_Checksum(Frame_t *pFrame)
{
    pFrame->Checksum = pFrame->Type | pFrame->SID;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função configura o Buffer Tx que será passado para a camada Comm_protocol transmitir na UART
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_appl_FrameToBuffer(Uart_t *pUart)
{
    int i;
    pUart->TxBuffer[_BREAK]  = pUart->frame.Break;
    pUart->TxBuffer[_SYNCH]  = pUart->frame.Synch;
    pUart->TxBuffer[_SID]    = pUart->frame.SID;
    pUart->TxBuffer[_TYPE]   = pUart->frame.Type;
    pUart->TxBuffer[_ID_SRC] = pUart->frame.Id_Source;
    pUart->TxBuffer[_ID_TRG] = pUart->frame.Id_Target;
    pUart->TxBuffer[_LENGHT] = pUart->frame.Lenght;
    for(i = 0; i < pUart->frame.Lenght - 1; i++){
        pUart->TxBuffer[_DATA+i] = pUart->frame.Data[i];
    }
    pUart->TxBuffer[_DATA+i] = pUart->frame.Checksum;
    return (7 + pUart->frame.Lenght);
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Descrição: Verifica se o frame recebido na serial Rx é eco do frame transmitido na serial Tx.
    
    \Parameters: *pUart - Instancia da estrutura de dados principal Main Data.
    
    \Return value: 0 - significa que não é eco
    \Return value: 1 - significa que é eco
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_appl_Check_Frame_IsEcho(Uart_t *pUart)
{
    if(pUart->RxBuffer[_ID_SRC] == pUart->TxBuffer[_ID_SRC]){
        return 1;
    }
    return 0;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Descrição: Valida se o frame recebido na serial Rx é valido. Checa os campos: Break, Synch e Checksum
    
    \Parameters: *pUart - Instancia da estrutura de dados principal Main Data.
    
    \Return value: 0 - significa que o frame não é valido
    \Return value: 1 - significa que o frame é valido
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_appl_Check_Frame_IsValid(Uart_t *pUart)
{
    if(pUart->RxBuffer[_BREAK] != 0x00 || pUart->RxBuffer[_SYNCH] != 0x55){
        return 0;
    }
    return 1;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Performs the search of the received command in the tables of the test execution.
    
    \Parameters: *psInst Kostia Cmd instance.
    
    \Return value: KOSTIA_OK
    \Return value: KOSTIA_ER_TYPE_NOTFIND
    \Return value: KOSTIA_ER_CMD_NOTFIND
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_FindCommand(byte *pAddr, Uart_t *pUart)
{
    byte lData[_CMD_CODE_FILTER_SIZE];
    byte u08CounterCmd = 0U;
    Kostia_Rsp_t eRsp = KOSTIA_ER_TYPE_NOTFIND;

    u08CounterCmd = 0;
    lData[0] = *(pAddr+0);
    lData[1] = *(pAddr+1);
    lData[2] = *(pAddr+2);
    
    while (CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[0] != 0){
        if((lData[0] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[0]) && (lData[1] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[1]) && (lData[2] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[2])){
            eRsp = CmdTable_FromMasterToSlave[u08CounterCmd].pfExecute(pAddr, pUart); /* Chamada da função que manipula um frame de resposta para o comando recebido */
            break;
        }else{
            /* Command not find */
            eRsp = KOSTIA_ER_CMD_NOTFIND;
        }
        u08CounterCmd++;
    }
    return eRsp;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Verifica se o campo mainData.frame.Id_Source =! 0  
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_QueryID(byte *pCmd, Uart_t *pUart)
{
    if(pUart->frame.Id_Source == 0x00){      
        pUart->frame.SID = *pCmd;                 /* Identificador de serviço da mensagem */
        pUart->frame.Checksum = 0x00;             /* Checksum */
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
    /* Get values of:  Temperature, Hour */
    sensor.temperatura = pUart->RxBuffer[8];
    time_now.hour_msb  = pUart->RxBuffer[17];
    time_now.hour_lsb  = pUart->RxBuffer[18];
    time_now.hour = ((time_now.hour_msb << 8)|(time_now.hour_lsb << 0));
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Function to read teh HW
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_SetID(byte *pCmd, Uart_t *pUart)
{
    if(pUart->frame.Id_Source == 0x00){
        pUart->frame.SID = *pCmd;                                         /* Identificador de serviço da mensagem */
        pUart->frame.Id_Source = pUart->RxBuffer[_ID_TRG];                /* Novo ID deste módulo */
        pUart->frame.Checksum = 0x00;                                     /* Checksum */
        digitalWrite(COMM_ENABLE_OUT, LOW);                               /* Habilita o próximo slave a comunicar-se */
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Function to read teh HW
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
#ifdef _MODULE_TYPE_PLUGS
static Kostia_Rsp_t Comm_appl_ConfigSlave(byte *pCmd, Uart_t *pUart)    /* Copy datas of config frame to this module (Copia os dados de configuração para este frame) */
{
    if(pUart->RxBuffer[_TYPE] == pUart->frame.Type && pUart->RxBuffer[_ID_TRG] == 0xFF){
        /* Preparação do frame de resposta: Neste caso, responde uma resposta positiva, isto é, retorna um frame com o mesmo ID do serviço Config Slave */
        pUart->frame.SID = *pCmd;       /* Identificador de serviço da mensagem */
        /* Copia os dados recebidos no frame de configuração para a estrutura modulo, a qual será usada no módulo Control_appl para controlar o módulo */
        /* Get setting to plug 1 */
        switch(pUart->RxBuffer[_PLUG_ID]){
            case _PLUG_1:
            {
                set_module.plug_1.id           = pUart->RxBuffer[_PLUG_ID ];
                set_module.plug_1.mode         = pUart->RxBuffer[_PLUG_MODE];
                set_module.plug_1.on_off       = pUart->RxBuffer[_PLUG_ON_OFF];
                set_module.plug_1.sensor_ref   = pUart->RxBuffer[_PLUG_SENSOR_ID];
                set_module.plug_1.min_max      = pUart->RxBuffer[_PLUG_MIN_MAX];
                set_module.plug_1.set_point    = pUart->RxBuffer[_PLUG_SET_POINT ];
                set_module.plug_1.hour_on_msb  = pUart->RxBuffer[_PLUG_HOUR_ON_MSB];
                set_module.plug_1.hour_on_lsb  = pUart->RxBuffer[_PLUG_HOUR_ON_LSB];
                set_module.plug_1.hour_off_msb = pUart->RxBuffer[_PLUG_HOUR_OFF_MSB];
                set_module.plug_1.hour_off_lsb = pUart->RxBuffer[_PLUG_HOUR_OFF_LSB];
                break;
            }
            case _PLUG_2:   /* Start execute of frame sending on bus */
            {
                set_module.plug_2.id           = pUart->RxBuffer[_PLUG_ID ];
                set_module.plug_2.mode         = pUart->RxBuffer[_PLUG_MODE];
                set_module.plug_2.on_off       = pUart->RxBuffer[_PLUG_ON_OFF];
                set_module.plug_2.sensor_ref   = pUart->RxBuffer[_PLUG_SENSOR_ID];
                set_module.plug_2.min_max      = pUart->RxBuffer[_PLUG_MIN_MAX];
                set_module.plug_2.set_point    = pUart->RxBuffer[_PLUG_SET_POINT ];
                set_module.plug_2.hour_on_msb  = pUart->RxBuffer[_PLUG_HOUR_ON_MSB];
                set_module.plug_2.hour_on_lsb  = pUart->RxBuffer[_PLUG_HOUR_ON_LSB];
                set_module.plug_2.hour_off_msb = pUart->RxBuffer[_PLUG_HOUR_OFF_MSB];
                set_module.plug_2.hour_off_lsb = pUart->RxBuffer[_PLUG_HOUR_OFF_LSB];
                break;
            }
            case _PLUG_3:   /* Executing frame sending on bus */
            {
                set_module.plug_3.id           = pUart->RxBuffer[_PLUG_ID ];
                set_module.plug_3.mode         = pUart->RxBuffer[_PLUG_MODE];
                set_module.plug_3.on_off       = pUart->RxBuffer[_PLUG_ON_OFF];
                set_module.plug_3.sensor_ref   = pUart->RxBuffer[_PLUG_SENSOR_ID];
                set_module.plug_3.min_max      = pUart->RxBuffer[_PLUG_MIN_MAX];
                set_module.plug_3.set_point    = pUart->RxBuffer[_PLUG_SET_POINT ];
                set_module.plug_3.hour_on_msb  = pUart->RxBuffer[_PLUG_HOUR_ON_MSB];
                set_module.plug_3.hour_on_lsb  = pUart->RxBuffer[_PLUG_HOUR_ON_LSB];
                set_module.plug_3.hour_off_msb = pUart->RxBuffer[_PLUG_HOUR_OFF_MSB];
                set_module.plug_3.hour_off_lsb = pUart->RxBuffer[_PLUG_HOUR_OFF_LSB];
                break;
            }
            case _PLUG_4:
            {
                set_module.plug_4.id           = pUart->RxBuffer[_PLUG_ID ];
                set_module.plug_4.mode         = pUart->RxBuffer[_PLUG_MODE];
                set_module.plug_4.on_off       = pUart->RxBuffer[_PLUG_ON_OFF];
                set_module.plug_4.sensor_ref   = pUart->RxBuffer[_PLUG_SENSOR_ID];
                set_module.plug_4.min_max      = pUart->RxBuffer[_PLUG_MIN_MAX];
                set_module.plug_4.set_point    = pUart->RxBuffer[_PLUG_SET_POINT ];
                set_module.plug_4.hour_on_msb  = pUart->RxBuffer[_PLUG_HOUR_ON_MSB];
                set_module.plug_4.hour_on_lsb  = pUart->RxBuffer[_PLUG_HOUR_ON_LSB];
                set_module.plug_4.hour_off_msb = pUart->RxBuffer[_PLUG_HOUR_OFF_MSB];
                set_module.plug_4.hour_off_lsb = pUart->RxBuffer[_PLUG_HOUR_OFF_LSB];
                break;
            } 
            default:
            {
                /* ToDo[PENS] - default handler */
            }
        }
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}
#endif


#ifdef _MODULE_TYPE_LIGTH
static Kostia_Rsp_t Comm_appl_ConfigSlave(byte *pCmd, Uart_t *pUart)    /* Copy datas of config frame to this module (Copia os dados de configuração para este frame) */
{
    if(pUart->RxBuffer[_TYPE] == pUart->frame.Type && pUart->RxBuffer[_ID_TRG] == 0xFF){
        /* Preparação do frame de resposta: Neste caso, responde uma resposta positiva, isto é, retorna um frame com o mesmo ID do serviço Config Slave */
        pUart->frame.SID = *pCmd;       /* Identificador de serviço da mensagem */
        
        /* Copia os dados recebidos no frame de configuração para a estrutura modulo, a qual será usada no módulo Control_appl para controlar o módulo */
        /* Get setting to LIGHT module */
        set_module.start_hour      = pUart->RxBuffer[_LIGHT_START_HOUR];    /* Byte 08 from RX_Buffer */
        set_module.start_min       = pUart->RxBuffer[_LIGHT_START_MIN];     /* Byte 09 from RX_Buffer */
        set_module.day_hour        = pUart->RxBuffer[_LIGHT_DAY_HOUR];      /* Byte 10 from RX_Buffer */
        set_module.day_min         = pUart->RxBuffer[_LIGHT_DAY_MIN];       /* Byte 11 from RX_Buffer */
        set_module.night_hour      = pUart->RxBuffer[_LIGHT_NIGHT_HOUR];    /* Byte 12 from RX_Buffer */
        set_module.night_min       = pUart->RxBuffer[_LIGHT_NIGHT_MIN];     /* Byte 13 from RX_Buffer */
        set_module.sweep_time      = pUart->RxBuffer[_LIGHT_SWEEP_TIME];    /* Byte 14 from RX_Buffer */
        set_module.led_white.day   = pUart->RxBuffer[_LIGHT_WHITE_DAY];     /* Byte 15 from RX_Buffer */
        set_module.led_white.night = pUart->RxBuffer[_LIGHT_WHITE_NIGHT];   /* Byte 16 from RX_Buffer */
        set_module.led_blue.day    = pUart->RxBuffer[_LIGHT_BLUE_DAY];      /* Byte 17 from RX_Buffer */
        set_module.led_blue.night  = pUart->RxBuffer[_LIGHT_BLUE_NIGHT];    /* Byte 18 from RX_Buffer */
        set_module.led_red.day     = pUart->RxBuffer[_LIGHT_RED_DAY];       /* Byte 19 from RX_Buffer */
        set_module.led_red.night   = pUart->RxBuffer[_LIGHT_RED_NIGHT];     /* Byte 20 from RX_Buffer */
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}
#endif


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Function to read teh HW
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
#ifdef _MODULE_TYPE_PLUGS
static Kostia_Rsp_t Comm_appl_RequestData(byte *pCmd, Uart_t *pUart)
{
    if(pUart->RxBuffer[_ID_TRG] == pUart->frame.Id_Source){
        pUart->frame.SID = *pCmd;       /* Identificador de serviço da mensagem */
        pUart->frame.Checksum = 0x00;   /* Checksum */
        /* Preparação do frame de resposta: Copia os dados da estrutura status_module, a qual contém as informações do módulo */
        /* Plug 1 */
        pUart->frame.Data[0]  = status_module.plug_1.id;
        pUart->frame.Data[1]  = status_module.plug_1.mode;
        pUart->frame.Data[2]  = status_module.plug_1.on_off;
        pUart->frame.Data[3]  = status_module.plug_1.potency;
        /* Plug 2 */
        pUart->frame.Data[4]  = status_module.plug_2.id;
        pUart->frame.Data[5]  = status_module.plug_2.mode;
        pUart->frame.Data[6]  = status_module.plug_2.on_off;
        pUart->frame.Data[7]  = status_module.plug_2.potency;
        /* Plug 3 */
        pUart->frame.Data[8]  = status_module.plug_3.id;
        pUart->frame.Data[9]  = status_module.plug_3.mode;
        pUart->frame.Data[10] = status_module.plug_3.on_off;
        pUart->frame.Data[11] = status_module.plug_3.potency;
        /* Plug 4 */
        pUart->frame.Data[12] = status_module.plug_4.id;
        pUart->frame.Data[13] = status_module.plug_4.mode;
        pUart->frame.Data[14] = status_module.plug_4.on_off;
        pUart->frame.Data[15] = status_module.plug_4.potency;
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}
#endif


#ifdef _MODULE_TYPE_LIGTH
static Kostia_Rsp_t Comm_appl_RequestData(byte *pCmd, Uart_t *pUart)
{
    if(pUart->RxBuffer[_ID_TRG] == pUart->frame.Id_Source){
        pUart->frame.SID = *pCmd;       /* Identificador de serviço da mensagem */
        pUart->frame.Checksum = 0x00;   /* Checksum */
        /* Preparação do frame de resposta: Copia os dados da estrutura status_module, a qual contém as informações do módulo */
        /* Plug 1 */
        pUart->frame.Data[0]  = status_module.start_hour;
        pUart->frame.Data[1]  = status_module.start_min;
        pUart->frame.Data[2]  = status_module.day_hour;
        pUart->frame.Data[3]  = status_module.day_min;
        pUart->frame.Data[4]  = status_module.night_hour;
        pUart->frame.Data[5]  = status_module.night_min;
        pUart->frame.Data[6]  = status_module.sweep_time;
        pUart->frame.Data[7]  = status_module.led_white.day;
        pUart->frame.Data[8]  = status_module.led_white.night;
        pUart->frame.Data[9]  = status_module.led_blue.day;
        pUart->frame.Data[10] = status_module.led_blue.night;
        pUart->frame.Data[11] = status_module.led_red.day;
        pUart->frame.Data[12] = status_module.led_red.night;
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}
#endif



/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Function to report error in the command table
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_CmdTableError(byte *pCmd, Uart_t *pUart)
{
    /* Trata a chegada de uma mensagem não registrada */
    return KOSTIA_NOK;
}
