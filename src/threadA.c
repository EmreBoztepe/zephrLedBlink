#include <zephyr/drivers/gpio.h>


#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec ledGreen = GPIO_DT_SPEC_GET(LED0_NODE, gpios);


void Thread_A(void *dummy1, void *dummy2, void *dummy3)
{
    if (!gpio_is_ready_dt(&ledGreen)) 
    {
    }

    gpio_pin_configure_dt(&ledGreen, GPIO_OUTPUT_ACTIVE);
    
    while(1)
    {
        gpio_pin_toggle_dt(&ledGreen);

        k_msleep(2000);
    }
    
}