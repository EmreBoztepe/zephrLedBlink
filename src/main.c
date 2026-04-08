#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Discovery kartındaki yeşil LED genellikle "led0" alias'ına bağlıdır */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)


static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
int main(void)
{
    int ret;
    int emre;
    if (!gpio_is_ready_dt(&led0)) {
        return 0;
    }

    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
    ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
    ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return 0;
    }

    while (1) {
        ret = gpio_pin_toggle_dt(&led0);
        ret = gpio_pin_toggle_dt(&led1);
        ret = gpio_pin_toggle_dt(&led2);
        if (ret < 0) {
            return 0;
        }
        k_msleep(500); // 500ms bekle (Blink hızı)
    }

    return 0;
}