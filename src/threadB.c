#include <zephyr/drivers/gpio.h>


#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec ledOrange = GPIO_DT_SPEC_GET(LED1_NODE, gpios);


void Thread_B(void *dummy1, void *dummy2, void *dummy3)
{
    if (!gpio_is_ready_dt(&led1)) 
    {
    }

    gpio_pin_configure_dt(&ledOrange, GPIO_OUTPUT_ACTIVE);
    
    while(1)
    {
        gpio_pin_toggle_dt(&ledOrange);

        k_msleep(500);
    }
    


}