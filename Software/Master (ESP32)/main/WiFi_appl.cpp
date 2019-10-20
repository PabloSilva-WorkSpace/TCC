/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  WiFi_appl file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/ 
#include "WiFi_appl.h"


/********************************************************************************************************************************************************************************************************************************************************
    ### Extern Global Variables
*********************************************************************************************************************************************************************************************************************************************************/
EventGroupHandle_t gWiFi_appl_event_group = xEventGroupCreate();   /* Criação de um eventgroup para sinalização do status da rede WiFi. */


/********************************************************************************************************************************************************************************************************************************************************
    ### Global Variables into this scope (this file *.c) 
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
 * @brief      Função callback do driver WiFi, referente aos modos Station e Access Point; Qualquer evento detectado no 
 *             driver Wifi implicará na chamada desta função, a qual tratará alguns tipos de eventos.
 *
 * @param      ctx    The context (In this case is null)
 * @param      event  Pointer to struct that contains WiFi status (i.g.: event)
 *
 * @return     { description_of_the_return_value }
*********************************************************************************************************************************************************************************************************************************************************/
void wifi_event_handler( WiFiEvent_t event, WiFiEventInfo_t info )
{
    static int nAttempts = 0;
    switch( event )
    {
        /* Evento disparado quando o stack tcp for inicializado */
        case SYSTEM_EVENT_STA_START:{
            if ( DEBUG )
                Serial.println("WiFi: ESP32 iniciou o driver WiFi no modo STA e vai tentar conectar-se ao AP");
            break;
        }
        /* Evento disparado quando o ESP recebe um IP do roteador (conexão bem sucedida entre ESP e Roteador) */
        case SYSTEM_EVENT_STA_GOT_IP:{
            if ( DEBUG ){
                Serial.printf( "WiFi: ESP32 conectou-se ao AP e recebeu o IP: " );
                Serial.println( WiFi.localIP() );
            }
            xEventGroupSetBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT ); /* Sinalizar, ou informar, por meio deste event group que o ESP32 estabeleceu conecxão com o AP da rede WiFi configurada */
            nAttempts = 0;
            break;
        }
        /* Evento disparado quando o ESP perde a conexão com a rede WiFi ou quando a tentativa de conexão não ocorrer */
        case SYSTEM_EVENT_STA_DISCONNECTED:{
            xEventGroupClearBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT );   /* Sinalizar, ou informar, por meio deste event group que o ESP32 perdeu a conexão com o AP da rede WiFi configurada */
            if(nAttempts < 3){
                Serial.println("WiFi: ESP32 realizará nova tentativa de conexão com o AP");
                wifi_init_sta();
                nAttempts++;
            } else{
                Serial.println("WiFi: ESP32 não conseguiu conectar-se ao AP configurado");
                wifi_init_ap();   /* Configuração do driver WiFi para o modo AP, a fim de configurar o ESP para tentar conectar em outra rede WiFi */
                nAttempts = 0;
            }
            break;
        }
        /* Evento disparado quando o stack tcp for inicializado */
        case SYSTEM_EVENT_AP_START:{
            if ( DEBUG )
                Serial.println("WiFi: ESP32 iniciou o driver WiFi no modo AP");
            xTaskCreatePinnedToCore( wifi_manager, "wifi_manager", 2048*4, NULL, 1, NULL, 1 );
            break;
        }
        /* Evento disparado quando algum dispositivo no modo Station conectar-se ao AP do ESP32 */
        case SYSTEM_EVENT_AP_STACONNECTED:{
            if( DEBUG )
                Serial.println("WiFi: Dispositivo STA conectou-se ao AP do ESP32");
            xEventGroupSetBits( gWiFi_appl_event_group, WIFI_AP_CONNECTED_BIT );   /* Se chegou aqui significa que o Wifi do ESP foi inicializado corretamente no modo AP. Então, sinaliza por meio do event group */
            break;
        }
        /* Evento disparado quando algum dispositivo no modo Station desconectar-se do AP do ESP32 */
        case SYSTEM_EVENT_AP_STADISCONNECTED:{
            if( DEBUG ) 
                Serial.println("WiFi: Dispositivo STA desconectou-se do AP do ESP32"); 
            xEventGroupClearBits( gWiFi_appl_event_group, WIFI_AP_CONNECTED_BIT );   /* Sinaliza, ou informa, por meio do event group que um cliente foi desconectado do AP do ESP*/
            break;
        }
        /* Evento não previsto */
        default:{
            break;
        }
    }
}


/********************************************************************************************************************************************************************************************************************************************************
 * @brief      Função responsável pela configuração do servidor http.
 *             Na prática, esta função abre uma porta (80) na rede wifi, a fim de escutar os clientes/Stations
 *             que estão conectados ao Access Point do ESP;
 *
 * @param      ctx    The context (In this case is null)
 * @param      event  Pointer to struct that contains WiFi status (i.g.: event)
 *
 * @return     { description_of_the_return_value }
*********************************************************************************************************************************************************************************************************************************************************/
void wifi_manager( void *pvParameters )
{
    struct netconn *conn, *newconn;
    err_t err;
  
    for(;;)
    {
        conn = netconn_new( NETCONN_TCP );   /* Cria uma nova conexão de rede TCP */
        netconn_bind( conn, NULL, 80 );      /* Associa o endereço IP do ESP32 com a porta 80 */
        netconn_listen(conn);                /* A partir de agora o servidor ficará aguardando a conexão socket de algum cliente */
        
        if( DEBUG )
            ESP_LOGI(TAG, "HTTP Server listening...\n" ); 
        
        do {
            /* A função netconn_accept é bloqueante, portanto, o programa ficará aguardando a conexão de um cliente nesta linha; Quando um cliente se conectar o handle da conexão "newconn"
             * será passada para a função http_server, para que seja feita a leitura e envio dos cabeçalhos HTTP e HTML. Atenção: Cada request no HTML, GET, POST, será criado um novo socket; */
            err = netconn_accept(conn, &newconn);
            if( DEBUG )
                ESP_LOGI(TAG, "Novo dispositivo conectado: Handler: %p\n", newconn );
            /* Se a conexão do cliente ocorreu com sucesso, chamar a função http_server */
            if (err == ERR_OK) {
                http_server(newconn);   /* Esta função é responsável em enviar a primeira pagina HTML para o cliente e de receber as requisições socket do client */
                netconn_delete(newconn);
            }
            vTaskDelay(1000/portTICK_PERIOD_MS);;  /* Bloquear esta task, a fim de dar chance das demais serem processadas/executadas */
        } while(err == ERR_OK);
    
        /* Caso ocorra algum erro durante a conexão socket com o cliente: (i) Fechar/encerrar a conexão socket e (ii) Deletar o handle da conexão socket */
        netconn_close(conn);
        netconn_delete(conn);
        
        if( DEBUG )
          ESP_LOGI(TAG, "Conexão Socket Encerrada: Handle:  %p\n", newconn );
    }
    vTaskDelete(NULL);   /* Em teoria, esta linha nunca será executada */
}


/********************************************************************************************************************************************************************************************************************************************************
 * @brief      Função responsável em tratar os request do HTML; Resumidamente é um Web Server.
 *
 * @param      ctx    The context (In this case is null)
 * @param      event  Pointer to struct that contains WiFi status (i.g.: event)
 *
 * @return     { void }
*********************************************************************************************************************************************************************************************************************************************************/
void http_server( struct netconn *conn ) 
{    
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;
  
    /* Recebe o request HTTP do client; conn é o handle da conexão e inbuf é a struct que internamente contém o buffer. */
    err = netconn_recv(conn, &inbuf);
     
    if (err == ERR_OK) {
        
        netbuf_data(inbuf, (void**)&buf, &buflen);   /* Os dados recebidos são populados em buf. Em buflen é carregado a quantidade de bytes que foram recebidos e armazenados em buf. */
        
        /* Imprime o header do request HTTP enviado pelo client; Este debug é importante pois informa o formato do request para os diferentes navegadores utilizados, e ajuda a identificar os requests do javascrip. */
        if( DEBUG ) {
            static int n_request = 0;
                ESP_LOGI(TAG, "\r\n------------------\n");
                ESP_LOGI(TAG, "Request N: %d: %.*s", ++n_request, buflen, buf); 
                ESP_LOGI(TAG, "\n------------------\r\n");
        }
            
        char *first_line = strtok(buf, "\n");   /* Extrair a primeira linha da string buf; Atenção strtok altera a string. No lugar de '\n' será adicionado '\0' */
        
        if(first_line) {
            /* Analisar se o request padrão enviado pelo cliente trata-se de um request inicial - Dispositivo acessa o IP do ESP32 via navegador */
            if( (strstr(first_line, "GET / ")) || (strstr(first_line, "GET /favicon.ico")) ) {
                if( DEBUG )
                    ESP_LOGI(TAG, "Enviar página HTML de entrada.\n");                   
                netconn_write(conn, html_page, sizeof(html_page), NETCONN_NOCOPY);
                //netconn_write( conn, server_html_page_form_start, server_html_page_form_end - server_html_page_form_start, NETCONN_NOCOPY );
            }
            /* Algoritmo responsável em receber os dados do formulário <form> com o login e senha de acesso ao dashboard */
            else if(strstr(first_line, "POST / ")) {
                if( DEBUG )
                    ESP_LOGI(TAG, "POST / recebido.\n" ); 
      
                /* Separar o campo Header do Body do request post; A separação é feita por meio do CRLFCRLF */
                for( int i = 0; i < buflen; ++i ) 
                {
                    if( buf[i+0]==0x0d && buf[i+1]==0x0a && buf[i+2]==0x0d && buf[i+3]==0x0a)
                    {
                        /* Calcula o tamanho do conteúdo data recebido no buffer */
                        int payload_size = buflen - (&buf[i+4] - buf);
            
                        if( DEBUG )
                            ESP_LOGI(TAG, "Paload_size: %d", payload_size );
                        
                        /* netbuf_data retorna um vetor de char, e não uma string, sendo assim
                         * não é garantido o NULL no final do vetor. Portanto, será feito uma
                         * cópia somente do payload data do POST em um novo buffer utilizando malloc; */
                        char * payload = (char*)pvPortMalloc( payload_size + 1 );
                        if( payload == NULL ) {
                            if ( DEBUG )
                                ESP_LOGI(TAG, "Error: malloc.\n"); 
                            break;
                        }
      
                        /* Copia o conteúdo data recebido no request POST no buffer payload */
                        sprintf( payload, "%.*s", payload_size, &buf[i+4]);
            
                        /* Imprime o valor do body contendo o ssid e pwd */
                        if( DEBUG )
                            ESP_LOGI(TAG, "Payload: %s", payload );
            
                        /* A separação dos campos do content-type/x-www-form-urlencoded (Exemplo: username=esp32&password=esp32pwd) */
                        char delim[] = "&";
            
                        /* Substitui o delimitador '&' por NULL; */
                        char *ptr = strtok( payload, delim );
                        char user_pwd_ok = 0;
                        while ( ptr != NULL )
                        {
                            /* Separa os campos do content-type/x-www-form-urlencoded conforme: (i) username=esp32 e (ii) password=esp32pwd */
                            if( DEBUG )
                                ESP_LOGI(TAG, "'%s'\n", ptr );
              
                            /* Separa os campos do content-type/x-www-form-urlencoded conforme: (i) username (ii) esp32 (iii) password e (iv) esp32pwd
                             * onde ptr armazena o valor de username e esp32, e ts+1 armazena a senha e password do usuário */
                            char * ts = NULL; 
                            if( ( ts = strstr( ptr, "=" ) ) ) 
                            {
                                /* Verifica se ptr é igual a string padrao username */
                                if( !memcmp( ptr, "ssid", ts - ptr ) ) {
                                    /* Imprime o username do formulário */
                                    if ( DEBUG )
                                        ESP_LOGI(TAG, "SSID: %s\n", ts+1 ); 
                                    /* Compara o username enviado no formulário pelo username padrão; Caso igual, seta um flag apenas, pois tanto o username quanto o password precisam ser iguais para chamar a pagina HTML do dashboard. */
                                    if( nvs_str_save("ssid", ts+1 ) == ESP_OK )
                                    {
                                        user_pwd_ok = 1;
                                    }
                                }
              
                                /* Verificar se ptr é igual ao password; */
                                if( !memcmp( ptr, "password", ts - ptr ) ) 
                                {
                                    if( DEBUG )
                                        ESP_LOGI( TAG, "Password: %s\n", ts+1 );  
                                    /* Compara o password enviado no formulário pelo password padrão; Caso igual, seta um flag apenas, pois tanto o username quanto o password precisam ser iguais para chamar a pagina HTML do dashboard. */
                                    if( nvs_str_save("password", ts+1 ) == ESP_OK ) 
                                    {
                                        if(user_pwd_ok) 
                                        {
                                            if( DEBUG )
                                                ESP_LOGI( TAG, "ssid e password salvos em nvs.\n"); 
                      
                                            /* Envia um retorno para a página HTML informando o status do led0. Esse retorno é importante pois o usuário saberá se o led acendeu ou não por meio da página web. */
                                            char str_Number[] = "1"; 
                                            /* Envia o cabeçalho com a confirmação do status - code:200 */
                                            netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);
                                            /* Envia '1' para a pagina HTML para notificar o retorno do javascript que a gravação do ssid e password ocorreu com sucesso */
                                            netconn_write(conn, str_Number, strlen(str_Number), NETCONN_NOCOPY);
                      
                                            /* Reseta o ESP32 por software */
                                            //ESP_ERROR_CHECK( esp_wifi_stop() );
                                            //ESP_ERROR_CHECK( esp_wifi_deinit() );
                                            //esp_restart(); 
                      
                                            break;
                                        }
                                    }
                                }
                            }
                            /* Convido o leitor a buscar mais informações na net sobre o strtok do C */
                            ptr = strtok(NULL, delim);
                        }
                        if( user_pwd_ok == 0 )
                        {
                            /* Caso não seja possível processar ssid ou password carrega novamente a mesma página HTML do formulário */
                            netconn_write(conn, html_page, sizeof(html_page), NETCONN_NOCOPY);
                            //netconn_write( conn, server_html_page_form_start, server_html_page_form_end - server_html_page_form_start, NETCONN_NOCOPY );
                        }
                        /* Desaloca o buffer que foi criado */
                        vPortFree(payload);
                        break;
                    }
                }
            }
        }
        else {
            if( DEBUG )
                ESP_LOGI( TAG, "Request Desconhecido.\n" );
        } 
    }
    /* Encerra a conexão e desaloca o buffer */
    netconn_close(conn);
    netbuf_delete(inbuf);
}
