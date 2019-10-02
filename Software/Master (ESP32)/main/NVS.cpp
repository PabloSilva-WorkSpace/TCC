/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  NVS file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/ 
#include "NVS.h"


/********************************************************************************************************************************************************************************************************************************************************
    ### Global Variables into this scope (this file *.c) 
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
 * @brief      Função Responsável em retornar o valor do SSID e PASSWORD salvos na NVS
 *
 * @param      ssid          The ssid
 * @param[in]  ssid_len      The ssid length
 * @param      password      The password
 * @param[in]  password_len  The password length
 *
 * @return     Retorna o status da leitura da nvs;
*********************************************************************************************************************************************************************************************************************************************************/
esp_err_t nvs_read_ssid_password( char * ssid, size_t ssid_len, char * password, size_t password_len )
{
    nvs_handle my_handle;
    esp_err_t err = nvs_open( "storage", NVS_READWRITE, &my_handle );
    if ( err != ESP_OK ) {
        if( DEBUG )
          ESP_LOGI(TAG, "Error (%d) opening NVS handle!\n", err );      
    }
    else{
        /* Leitura do SSID salvo na NVS */
        err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
        if( err != ESP_OK ) {             
            if( err == ESP_ERR_NVS_NOT_FOUND ) {
                if( DEBUG )
                    ESP_LOGI(TAG, "\nKey ssid not found.\n");
            }
        }
        else{
            if( DEBUG )
                ESP_LOGI(TAG, "\nssid is %s\n", ssid );
        }
        /* Leitura do PASSWORD salvo em NVS */
        err = nvs_get_str(my_handle, "password", password, &password_len);
        if( err != ESP_OK ) {
            if( err == ESP_ERR_NVS_NOT_FOUND ) {
                if( DEBUG )
                    ESP_LOGI(TAG, "\nKey password not found.\n");
            }
        }
        else{
            if( DEBUG )
              ESP_LOGI(TAG, "\npassword is %s.\n", ssid );
        }
        nvs_close(my_handle);   
    }
    return err;
}


/********************************************************************************************************************************************************************************************************************************************************
 * @brief      Função responsável pela gravação de uma string na NVS;
 *
 * @param      key    The key
 * @param      value  The value
 *
 * @return     { description_of_the_return_value }
*********************************************************************************************************************************************************************************************************************************************************/
esp_err_t nvs_str_save(char * key, char * value)
{
    nvs_handle my_handle;
    esp_err_t err = nvs_open( "storage", NVS_READWRITE, &my_handle );
    if ( err != ESP_OK ) {          
        if( DEBUG )
            ESP_LOGI(TAG, "Error (%d) opening NVS handle!\n", err );      
    } else {
        err = nvs_set_str(my_handle, key, value );
        if(err != ESP_OK) {
            if( DEBUG )
                ESP_LOGI(TAG, "\nError in %s : (%04X)\n", key, err);      
        }
        /* Salvar em NVS (Non-Volatile Storage) */
        err = nvs_commit(my_handle);
        if(err != ESP_OK) {
            if( DEBUG )
                ESP_LOGI(TAG, "\nError in commit! (%04X)\n", err);
        }         
        nvs_close(my_handle);   
    }
    return err;
}
