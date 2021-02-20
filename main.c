#include <stdio.h>
#include <time.h>
#include <metal/gpio.h>
#include <metal/cpu.h>

struct metal_gpio* gpio_device;
struct metal_cpu* cpu;
struct metal_interrupt* cpu_intr;
struct metal_interrupt* gpio_ic;
int gpio_irq, rc, count=0;

void delayForSeconds(unsigned seconds) {
    time_t timeout;
    timeout = time(NULL) + seconds;
  while (timeout > time(NULL));
}

void kypd_isr(int id, void* data){
    struct metal_gpio* gpio_device;
    gpio_device = metal_gpio_get_device(0);
    metal_gpio_config_interrupt(gpio_device, 12, METAL_GPIO_INT_DISABLE);
    metal_gpio_clear_interrupt(gpio_device, 12, METAL_GPIO_INT_FALLING);
    metal_gpio_toggle_pin(gpio_device, 20);
    if(count<3){
        count+=1;
    }
    else{
        count=0;
    }
}
void zero(){
    metal_gpio_set_pin(gpio_device, 5,1);
    metal_gpio_set_pin(gpio_device, 19,1);
    metal_gpio_set_pin(gpio_device, 3,1);
    metal_gpio_set_pin(gpio_device, 2,1);
    metal_gpio_set_pin(gpio_device, 23,1);
    metal_gpio_set_pin(gpio_device, 22,1);
    metal_gpio_set_pin(gpio_device, 21,0);
}
void one(){
    metal_gpio_set_pin(gpio_device, 5,0);
    metal_gpio_set_pin(gpio_device, 19,1);
    metal_gpio_set_pin(gpio_device, 3,1);
    metal_gpio_set_pin(gpio_device, 2,0);
    metal_gpio_set_pin(gpio_device, 23,0);
    metal_gpio_set_pin(gpio_device, 22,0);
    metal_gpio_set_pin(gpio_device, 21,0);
}

void two(){
    metal_gpio_set_pin(gpio_device, 5,1);
    metal_gpio_set_pin(gpio_device, 19,1);
    metal_gpio_set_pin(gpio_device, 3,0);
    metal_gpio_set_pin(gpio_device, 2,1);
    metal_gpio_set_pin(gpio_device, 23,1);
    metal_gpio_set_pin(gpio_device, 22,0);
    metal_gpio_set_pin(gpio_device, 21,1);
}

void three(){
    metal_gpio_set_pin(gpio_device, 5,1);
    metal_gpio_set_pin(gpio_device, 19,1);
    metal_gpio_set_pin(gpio_device, 3,1);
    metal_gpio_set_pin(gpio_device, 2,1);
    metal_gpio_set_pin(gpio_device, 23,0);
    metal_gpio_set_pin(gpio_device, 22,0);
    metal_gpio_set_pin(gpio_device, 21,1);
}

int main(void) {
    gpio_device = metal_gpio_get_device(0);
    if (!gpio_device) {
        printf("Unable to obtain GPIO device\n");
        return -1;
    }

    metal_gpio_disable_input(gpio_device, 20);
    metal_gpio_enable_output(gpio_device, 20);
    metal_gpio_disable_pinmux(gpio_device, 20);
    metal_gpio_set_pin(gpio_device, 20,1);

    metal_gpio_disable_input(gpio_device, 5);
    metal_gpio_enable_output(gpio_device, 5);
    metal_gpio_disable_pinmux(gpio_device, 5);
    // metal_gpio_set_pin(gpio_device, 5,1);

    metal_gpio_disable_input(gpio_device, 19);
    metal_gpio_enable_output(gpio_device, 19);
    metal_gpio_disable_pinmux(gpio_device, 19);
    // metal_gpio_set_pin(gpio_device, 19,1);

    metal_gpio_disable_input(gpio_device, 3);
    metal_gpio_enable_output(gpio_device, 3);
    metal_gpio_disable_pinmux(gpio_device, 3);
    // metal_gpio_set_pin(gpio_device, 3,1);

    metal_gpio_disable_input(gpio_device, 2);
    metal_gpio_enable_output(gpio_device, 2);
    metal_gpio_disable_pinmux(gpio_device, 2);
    // metal_gpio_set_pin(gpio_device, 2,1);

    metal_gpio_disable_input(gpio_device, 23);
    metal_gpio_enable_output(gpio_device, 23);
    metal_gpio_disable_pinmux(gpio_device, 23);
    // metal_gpio_set_pin(gpio_device, 23,1);

    metal_gpio_disable_input(gpio_device, 22);
    metal_gpio_enable_output(gpio_device, 22);
    metal_gpio_disable_pinmux(gpio_device, 22);
    // metal_gpio_set_pin(gpio_device, 22,1);

    metal_gpio_disable_input(gpio_device, 21);
    metal_gpio_enable_output(gpio_device, 21);
    metal_gpio_disable_pinmux(gpio_device, 21);
    // metal_gpio_set_pin(gpio_device, 21,1);

    metal_gpio_disable_input(gpio_device, 13);
    metal_gpio_enable_output(gpio_device, 13);
    metal_gpio_disable_pinmux(gpio_device, 13);
    // metal_gpio_set_pin(gpio_device, 13,0);
    
    metal_gpio_disable_output(gpio_device, 12);
    metal_gpio_enable_input(gpio_device, 12);
    metal_gpio_disable_pinmux(gpio_device, 12);

    cpu = metal_cpu_get(metal_cpu_get_current_hartid());
    if(!cpu){
        printf("Unable to get cpu");
        return 1;
    }
    cpu_intr = metal_cpu_interrupt_controller(cpu);
    if(!cpu_intr){
        printf("Failed to get cpu interrrupt conroller");
        return 2;
    }
    metal_interrupt_init(cpu_intr);

    gpio_ic = metal_gpio_interrupt_controller(gpio_device);
    if(!gpio_ic){
        printf("Failed to get gpio interrupt controller");
        return 3;
    }

    metal_interrupt_init(gpio_ic);

    gpio_irq = metal_gpio_get_interrupt_id(gpio_device, 12);
    while(1){
        if(count==0){
            zero();
        }
        if(count==1){
            one();
        }
        else if(count==2){
            two();
        }
        else if(count==3){
            three();
        }
        rc = metal_interrupt_register_handler(gpio_ic, gpio_irq, kypd_isr, gpio_device);
        if (rc < 0) {
            printf("KYPD interrupt handler registration failed\n");
            return (rc * -1);
        }
        metal_gpio_config_interrupt(gpio_device, 12, METAL_GPIO_INT_FALLING);
        metal_interrupt_enable(gpio_ic, gpio_irq);
        metal_interrupt_enable(cpu_intr, 0);
    }
  return 0;
}