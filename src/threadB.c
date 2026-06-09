#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>

#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec ledOrange = GPIO_DT_SPEC_GET(LED1_NODE, gpios);



static const struct adc_dt_spec adc_channels[] =
{    
    ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0),
    ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 1)
};
uint32_t bufArray[2];

struct adc_sequence sequences[] =
{
    {
        .buffer = &bufArray[0],
        .buffer_size = sizeof(uint32_t)
    },
    {
        .buffer = &bufArray[1],
        .buffer_size = sizeof(uint32_t)
    }

};


void Thread_B(void *dummy1, void *dummy2, void *dummy3)
{
    if (!gpio_is_ready_dt(&ledOrange)) 
    {
    }

    if(!adc_is_ready_dt(&adc_channels[0]))
    {

    }
    
    if(!adc_is_ready_dt(&adc_channels[1]))
    {
        
    }

    adc_channel_setup_dt(&adc_channels[0]);
    adc_channel_setup_dt(&adc_channels[1]);

    gpio_pin_configure_dt(&ledOrange, GPIO_OUTPUT_ACTIVE);

    (void)adc_sequence_init_dt(&adc_channels[0], &sequences[0]);
	(void)adc_sequence_init_dt(&adc_channels[1], &sequences[1]);
    
    while(1)
    {
        gpio_pin_toggle_dt(&ledOrange);



        adc_read_dt(&adc_channels[0], &sequences[0]);

        adc_read_dt(&adc_channels[1], &sequences[1]);

        k_msleep(500);
    }
    


}