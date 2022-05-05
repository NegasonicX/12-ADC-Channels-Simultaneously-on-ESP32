
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static esp_adc_cal_characteristics_t *adc1_chars;           //++ Handle for ADC1
static esp_adc_cal_characteristics_t *adc2_chars;           //++ Handle for ADC2

static const adc1_channel_t channel_0 = ADC_CHANNEL_0;     //++ GPIO36 ( ADC1 CHANNEL 0 )
static const adc1_channel_t channel_3 = ADC_CHANNEL_3;     //++ GPIO39 ( ADC1 CHANNEL 3 )
static const adc1_channel_t channel_1_6 = ADC_CHANNEL_6;     //++ GPIO34 ( ADC1 CHANNEL 6 )
static const adc1_channel_t channel_1_7 = ADC_CHANNEL_7;     //++ GPIO35 ( ADC1 CHANNEL 7 )
static const adc1_channel_t channel_1_4 = ADC_CHANNEL_4;     //++ GPIO32 ( ADC1 CHANNEL 4 )
static const adc1_channel_t channel_1_5 = ADC_CHANNEL_5;     //++ GPIO33 ( ADC1 CHANNEL 5 )

static const adc2_channel_t channel_8 = ADC_CHANNEL_8;     //++ GPIO25 ( ADC2 CHANNEL 8 )
static const adc2_channel_t channel_9 = ADC_CHANNEL_9;     //++ GPIO26 ( ADC2 CHANNEL 9 )
static const adc2_channel_t channel_2_7 = ADC_CHANNEL_7;     //++ GPIO27 ( ADC2 CHANNEL 7 )
static const adc2_channel_t channel_2_6 = ADC_CHANNEL_6;     //++ GPIO14 ( ADC2 CHANNEL 6 )
static const adc2_channel_t channel_2_0 = ADC_CHANNEL_0;     //++ GPIO4 ( ADC2 CHANNEL 0 )
static const adc2_channel_t channel_2_4 = ADC_CHANNEL_4;     //++ GPIO13 ( ADC2 CHANNEL 4 )

static const adc_bits_width_t width = ADC_WIDTH_BIT_12;      //++ 12-bit Resolution

static const adc_atten_t atten = ADC_ATTEN_DB_11;        //++ ADC_ATTEN_DB_11
static const adc_unit_t unit1 = ADC_UNIT_1;              //++ ADC 1 Initialization
static const adc_unit_t unit2 = ADC_UNIT_2;              //++ ADC 2 Initialization

static void check_efuse(void)
{

    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) 
    {
        printf("eFuse Two Point: Supported\n");
    } 
    else 
    {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
    {
        printf("eFuse Vref: Supported\n");
    } 
    else 
    {
        printf("eFuse Vref: NOT supported\n");
    }

}


static void print_char_val_type(esp_adc_cal_value_t val_type1)
{
    if (val_type1 == ESP_ADC_CAL_VAL_EFUSE_TP) 
    {
        printf("Characterized using Two Point Value\n");
    } 
    else if (val_type1 == ESP_ADC_CAL_VAL_EFUSE_VREF) 
    {
        printf("Characterized using eFuse Vref\n");
    } 
    else 
    {
        printf("Characterized using Default Vref\n");
    }
}


void app_main(void)
{
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    adc1_config_width(width);                                           //++ Set ADC1 with 12-bit Resolution

    adc1_config_channel_atten(channel_0, atten);                        //++ Configure ADC1 Channel 0
    adc1_config_channel_atten(channel_3, atten);                        //++ Configure ADC1 Channel 3
    adc1_config_channel_atten(channel_1_6, atten);                      //++ Configure ADC1 Channel 6
    adc1_config_channel_atten(channel_1_7, atten);                      //++ Configure ADC1 Channel 7
    adc1_config_channel_atten(channel_1_4, atten);                      //++ Configure ADC1 Channel 4
    adc1_config_channel_atten(channel_1_5, atten);                      //++ Configure ADC1 Channel 5

    adc2_config_channel_atten((adc2_channel_t)channel_8, atten);        //++ Configure ADC2 Channel 8
    adc2_config_channel_atten((adc2_channel_t)channel_9, atten);        //++ Configure ADC2 Channel 9
    adc2_config_channel_atten((adc2_channel_t)channel_2_7, atten);      //++ Configure ADC2 Channel 7
    adc2_config_channel_atten((adc2_channel_t)channel_2_6, atten);      //++ Configure ADC2 Channel 6
    adc2_config_channel_atten((adc2_channel_t)channel_2_0, atten);      //++ Configure ADC2 Channel 5
    adc2_config_channel_atten((adc2_channel_t)channel_2_4, atten);      //++ Configure ADC2 Channel 4

    //Characterize ADC1
    adc1_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type1 = esp_adc_cal_characterize(unit1, atten, width, DEFAULT_VREF, adc1_chars);
    print_char_val_type(val_type1);

    //Characterize ADC2
    adc2_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type2 = esp_adc_cal_characterize(unit2, atten, width, DEFAULT_VREF, adc2_chars);
    print_char_val_type(val_type2);

    //Continuously sample Both ADCs ( 12 Channels )
    while (1) 
    {
        // Variable Declarations for Raw ADC1 Data 
        uint32_t adc1_raw_C0 = 0;
        uint32_t adc1_raw_C3 = 0;
        uint32_t adc1_raw_C6 = 0;
        uint32_t adc1_raw_C7 = 0;
        uint32_t adc1_raw_C4 = 0;
        uint32_t adc1_raw_C5 = 0;

        // Variable Declarations for Raw ADC2 Data
        uint32_t adc2_raw_C8 = 0;
        uint32_t adc2_raw_C9 = 0;
        uint32_t adc2_raw_C7 = 0;
        uint32_t adc2_raw_C6 = 0;
        uint32_t adc2_raw_C5 = 0;
        uint32_t adc2_raw_C4 = 0;

        int adc2_raw;               //++ ADC2 Raw Data Temporary Variable

        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) 
        {
            
            // Obtain ADC1 Raw Data 
            adc1_raw_C0 += adc1_get_raw((adc1_channel_t)channel_0);
            adc1_raw_C3 += adc1_get_raw((adc1_channel_t)channel_3);
            adc1_raw_C6 += adc1_get_raw((adc1_channel_t)channel_1_6);
            adc1_raw_C7 += adc1_get_raw((adc1_channel_t)channel_1_7);
            adc1_raw_C4 += adc1_get_raw((adc1_channel_t)channel_1_4);
            adc1_raw_C5 += adc1_get_raw((adc1_channel_t)channel_1_5);

            // Obtain ADC2 Raw Data 
            adc2_get_raw((adc2_channel_t)channel_8, width, &adc2_raw);
            adc2_raw_C8 += adc2_raw;
            adc2_get_raw((adc2_channel_t)channel_9, width, &adc2_raw);
            adc2_raw_C9 += adc2_raw;
            adc2_get_raw((adc2_channel_t)channel_2_7, width, &adc2_raw);
            adc2_raw_C7 += adc2_raw;
            adc2_get_raw((adc2_channel_t)channel_2_6, width, &adc2_raw);
            adc2_raw_C6 += adc2_raw;
            adc2_get_raw((adc2_channel_t)channel_2_0, width, &adc2_raw);
            adc2_raw_C5 += adc2_raw;
            adc2_get_raw((adc2_channel_t)channel_2_4, width, &adc2_raw);
            adc2_raw_C4 += adc2_raw;

            
        }

        // Convert ADC1 Raw Data
        adc1_raw_C0 /= NO_OF_SAMPLES;
        adc1_raw_C3 /= NO_OF_SAMPLES;
        adc1_raw_C6 /= NO_OF_SAMPLES;
        adc1_raw_C7 /= NO_OF_SAMPLES;
        adc1_raw_C4 /= NO_OF_SAMPLES;
        adc1_raw_C5 /= NO_OF_SAMPLES;

        // Convert ADC2 Raw Data
        adc2_raw_C8 /= NO_OF_SAMPLES;
        adc2_raw_C9 /= NO_OF_SAMPLES;
        adc2_raw_C7 /= NO_OF_SAMPLES;
        adc2_raw_C6 /= NO_OF_SAMPLES;
        adc2_raw_C5 /= NO_OF_SAMPLES;
        adc2_raw_C4 /= NO_OF_SAMPLES;

        //Convert ADC1 Raw Data to voltage in mV
        uint32_t voltage_1_C0 = esp_adc_cal_raw_to_voltage(adc1_raw_C0, adc1_chars);
        uint32_t voltage_1_C3 = esp_adc_cal_raw_to_voltage(adc1_raw_C3, adc1_chars);
        uint32_t voltage_1_C6 = esp_adc_cal_raw_to_voltage(adc1_raw_C6, adc1_chars);
        uint32_t voltage_1_C7 = esp_adc_cal_raw_to_voltage(adc1_raw_C7, adc1_chars);
        uint32_t voltage_1_C4 = esp_adc_cal_raw_to_voltage(adc1_raw_C4, adc1_chars);
        uint32_t voltage_1_C5 = esp_adc_cal_raw_to_voltage(adc1_raw_C5, adc1_chars);

        //Convert ADC2 Raw Data to voltage in mV
        uint32_t voltage_2_C8 = esp_adc_cal_raw_to_voltage(adc2_raw_C8, adc2_chars);
        uint32_t voltage_2_C9 = esp_adc_cal_raw_to_voltage(adc2_raw_C9, adc2_chars);
        uint32_t voltage_2_C7 = esp_adc_cal_raw_to_voltage(adc2_raw_C7, adc2_chars);
        uint32_t voltage_2_C6 = esp_adc_cal_raw_to_voltage(adc2_raw_C6, adc2_chars);
        uint32_t voltage_2_C5 = esp_adc_cal_raw_to_voltage(adc2_raw_C5, adc2_chars);
        uint32_t voltage_2_C4 = esp_adc_cal_raw_to_voltage(adc2_raw_C4, adc2_chars);

        printf("RAW ADC1: | C0: %d\tVoltage: %dmV | C3: %d\tVoltage: %dmV | C6: %d\tVoltage: %dmV | C7: %d\tVoltage: %dmV | C4: %d\tVoltage: %dmV | C5: %d\tVoltage: %dmV\n", adc1_raw_C0, voltage_1_C0, adc1_raw_C3, voltage_1_C3, adc1_raw_C6, voltage_1_C6, adc1_raw_C7, voltage_1_C7, adc1_raw_C4, voltage_1_C4, adc1_raw_C5, voltage_1_C5);
        printf("RAW ADC2: | C8: %d\tVoltage: %dmV | C9: %d\tVoltage: %dmV | C7: %d\tVoltage: %dmV | C6: %d\tVoltage: %dmV | C5: %d\tVoltage: %dmV | C4: %d\tVoltage: %dmV\n", adc2_raw_C8, voltage_2_C8, adc2_raw_C9, voltage_2_C9, adc2_raw_C7, voltage_2_C7, adc2_raw_C6, voltage_2_C6, adc2_raw_C5, voltage_2_C5, adc2_raw_C4, voltage_2_C4);
        printf("\n");

        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}
