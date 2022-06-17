//MdE Oximetro y Medidor de Pulso - Principal.
#include "ssd1306.h"
#include "fonts.h"
#include "test.h"
#include "max30102_for_stm32_hal.h"
////////////////Definiciones MdE//////////////////////////

#define INI_CONSOLA 1
#define INI_DISPLAY 2
#define INI_BOTONERA 3
#define CFG_MAX30100 4
#define FIN_INI 5


#define INT_MAX30100 1
#define ESCRIBIR_DISPLAY 2
#define LEER_BOTONERA 3
#define CALCULO 4
#define MENU_CALIBRACION 5

uint8_t estado_ini = 1;
uint8_t estado_main = 2;
uint8_t valor_teclado;
uint8_t flag_int_max;
max30102_t max30102;

////////////////Codigo de la interrupcion INT EXT del MAX30100 ///////
///Ext Int PC1
/*


*/
///////////////MdE Inicializacion//////////////////////////////////////
void main()
{
   HAL_Init();
   SystemClock_Config();
    while(estado_ini != 100)
    {
        switch(estado_ini)
        {
            case INI_CONSOLA:
                //Rutina de inicializacion USART2
                MX_USART2_UART_Init();
                HAL_UART_Transmit(&huart2,"Iniciando...",1,100);
                estado_ini = INI_BOTONERA;
            break;
            case INI_BOTONERA:
                //Rutina de inicializacion GPIO
                MX_GPIO_Init();
                estado_ini = INI_DISPLAY;
            break;
            case INI_DISPLAY:
                //Rutina de inicializacion I2C
                MX_I2C3_Init();
                SSD1306_Init (); // initialize the display 
                SSD1306_GotoXY (10,10); // goto 10, 10 
                SSD1306_Puts ("Inicio ", &Font_11x18, 1); // print Hello 
                SSD1306_GotoXY (10, 30); 
                SSD1306_Puts ("Pantalla", &Font_11x18, 1); 
                SSD1306_UpdateScreen(); // update screen
                estado_ini = CFG_MAX30100;
            break;
            case CFG_MAX30100:
                //Rutina de configuracion  de MAX30100
                MX_I2C1_Init();
                // Initiation
                max30102_init(&max30102, &hi2c1);
                max30102_reset(&max30102);
                max30102_clear_fifo(&max30102);
                max30102_set_fifo_config(&max30102, max30102_smp_ave_8, 1, 7);                
                // Sensor settings
                max30102_set_led_pulse_width(&max30102, max30102_pw_16_bit);
                max30102_set_adc_resolution(&max30102, max30102_adc_2048);
                max30102_set_sampling_rate(&max30102, max30102_sr_800);
                max30102_set_led_current_1(&max30102, 6.2);
                max30102_set_led_current_2(&max30102, 6.2);
                // Enter SpO2 mode
                max30102_set_mode(&max30102, max30102_spo2);
                max30102_set_a_full(&max30102, 1);                
                // Initiate 1 temperature measurement
                max30102_set_die_temp_en(&max30102, 1);
                max30102_set_die_temp_rdy(&max30102, 1);                
                uint8_t en_reg[2] = {0};
                max30102_read(&max30102, 0x00, en_reg, 1);
                estado_ini = FIN_INI;
            break;
            case FIN_INI:
                //Configuramos el lazo principal
                estado_main = ESCRIBIR_DISPLAY;
                estado_ini = 100;
            break;
        }
    }
/////////MdE Principal//////////////////////////////////////////////
    while(1)
    {
        switch(estado_main)
        {
            case ESCRIBIR_DISPLAY:
                escribir_display();
                if (flag_int_max == 1) estado_main = INT_MAX30100;
                else  estado_main = LEER_BOTONERA;
            break;
            case LEER_BOTONERA:
                valor_teclado = leer_teclado();
                //Falta la condicion de teclado que envia la MdE a MENU_CALIBRACION
                if (flag_int_max == 1) estado_main = INT_MAX30100;
                else  estado_main = ESCRIBIR_DISPLAY;                
            break;
            case INT_MAX30100:
                max30102_interrupt_handler(&max30102);
                if (flag_calculo == 1) estado_main = CALCULO;
                else estado_main = ESCRIBIR_DISPLAY;                
            break;
            case CALCULO:
                calculo_sp_hr();
                estado_main = ESCRIBIR_DISPLAY;
            break;
            case MENU_CALIBRACION:
                calibracion();
                estado_main = ESCRIBIR_DISPLAY;
            break;
        }
    }

}
