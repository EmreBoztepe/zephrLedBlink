#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec ledOrange = GPIO_DT_SPEC_GET(LED1_NODE, gpios);



static const struct adc_dt_spec adc_channels[] =
{    
    ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0),
    ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 1)
};
	int err;
	uint32_t count = 0;
	uint32_t buf = 0;
struct adc_sequence sequence = {
    .buffer = &buf,
    /* buffer size in bytes, not number of samples */
    .buffer_size = sizeof(buf),

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
    
    while(1)
    {
        gpio_pin_toggle_dt(&ledOrange);

        (void)adc_sequence_init_dt(&adc_channels[0], &sequence);
		(void)adc_sequence_init_dt(&adc_channels[1], &sequence);

        adc_read_dt(&adc_channels[0], &sequence);

        adc_read_dt(&adc_channels[1], &sequence);

        k_msleep(500);
    }
    


}