//MdE Oximetro y Medidor de Pulso - Principal.

////////////////Definiciones MdE//////////////////////////

#define INI_CONSOLA 1
#define INI_DISPLAY 2
#define INI_I2C 3
#define INI_BOTONERA 4
#define CFG_MAX30100 5
#define FIN_INI 6


#define INT_MAX30100 1
#define ESCRIBIR_DISPLAY 2
#define LEER_BOTONERA 3
#define CALCULO 5
#define MENU_CALIBRACION 6

uint8_t estado_ini = 1;
uint8_t estado_main = 2;
uint8_t valor_teclado;
uint8_t flag_int_max;

///////////////MdE Inicializacion////////////////////////////////
void main()
{
    while(estado_ini != 100)
    {
        switch(estado_ini)
        {
            case INI_CONSOLA:
                //Rutina de inicializacion USART2
                estado_ini = INI_BOTONERA;
            break;
            case INI_BOTONERA:
                //Rutina de inicializacion GPIO
                estado_ini = INI_DISPLAY;
            break;
            case INI_DISPLAY:
                //Rutina de inicializacion I2C
                estado_ini = INI_I2C;
            break;
            case INI_I2C:
                //Rutina de inicializacion MAX
                estado_ini = INI_I2C;
            break;
            case CFG_MAX30100:
                //Rutina de configuracion  de MAX30100
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