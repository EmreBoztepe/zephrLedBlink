#include <zephyr/kernel.h>

#define STACK_SIZE 1024U
/* Discovery kartındaki yeşil LED genellikle "led0" alias'ına bağlıdır */
extern void Thread_B(void *, void *, void *);
extern void Thread_A(void *, void *, void *);

K_THREAD_DEFINE(thread_b, STACK_SIZE,
				Thread_B, NULL, NULL, NULL,
				3, 0, 0);

K_THREAD_DEFINE(thread_a, STACK_SIZE,
				Thread_A, NULL, NULL, NULL,
				44, 0, 0);
int main(void)
{


    return 0;
}