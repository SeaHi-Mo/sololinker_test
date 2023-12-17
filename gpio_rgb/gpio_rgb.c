/**
 * @file gpio_rgb.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-12-17
 *
 * @copyright Copyright (c) 2023
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int gpio_numble=0;
/**
 * @brief 导出GPIO 例如： IO2-A0
 *
 * @param gpio
 * @return int
*/
static int export_gpio_numble(char* gpio)
{
    int gpio_blank = 0;
    int gpio_group = 0;
    int gpio_numX = 0;
    FILE* export_file = fopen("/sys/class/gpio/export", "w");

    if (export_file==NULL) {
        perror("Failed to open GPIO export file");
        return -1;
    }
    char* IO_STR = strtok(gpio, "-");
    if (IO_STR!=NULL) {

        gpio_blank = atoi(IO_STR+2);

    }
    else {
        perror("IO_STR Is NULL");
        return -1;
    }

    IO_STR = strtok(NULL, "-");

    if (IO_STR!=NULL) {

        if (IO_STR[0]=='A') gpio_group = 0;
        if (IO_STR[0]=='B') gpio_group = 1;
        if (IO_STR[0]=='C') gpio_group = 2;
        if (IO_STR[0]=='D') gpio_group = 3;
        gpio_numX = atoi(&IO_STR[1]);
    }
    else {
        perror("IO_STR Is NULL");
        return -1;
    }
    fprintf(export_file, "%d", gpio_blank*32+(gpio_group*8+gpio_numX));
    fclose(export_file);
    return gpio_blank*32+(gpio_group*8+gpio_numX);
}
/**
 * @brief Set the gpio direction object
 *
 * @param gpio_pin
 * @param direction "out" or "in"
 * @return int
*/
static int set_gpio_direction(int gpio_pin, const char* direction)
{
    char* gpio_pin_path = malloc(64);
    memset(gpio_pin_path, 0, 64);
    sprintf(gpio_pin_path, "/sys/class/gpio/gpio%d/direction", gpio_pin);
    FILE* direction_file = fopen(gpio_pin_path, "w");

    if (direction_file==NULL) {
        perror("Failed to open GPIO direction file");
        return -1;
    }
    fprintf(direction_file, "%s", direction);
    fclose(direction_file);
    free(gpio_pin_path);
    return 0;
}
/**
 * @brief Get the gpio direction object
 *
 * @param gpio_pin
 * @return int 0 is "in" 1 is "out"
*/
static int get_gpio_direction(int gpio_pin)
{
    char* gpio_direction = malloc(64);
    char* buff = malloc(8);
    int ret = 0;
    memset(gpio_direction, 0, 64);
    memset(buff, 0, 8);
    sprintf(gpio_direction, "/sys/class/gpio/gpio%d/direction", gpio_pin);
    FILE* direction_file = fopen(gpio_direction, "r");
    if (direction_file==NULL) {
        perror("Failed to open GPIO direction file");
        return -1;
    }
    fread(buff, sizeof(buff), 1, direction_file);
    if (buff==NULL) {
        perror("Failed to read GPIO direction file");
        fclose(direction_file);
        free(buff);
        free(gpio_direction);
        return -1;
    }
    ret = strcmp(buff, "in")==0?0:1;
    fclose(direction_file);
    free(buff);
    free(gpio_direction);

    return ret;
}

/**
 * @brief Set the gpio value object
 *
 * @param gpio_pin
 * @param value char "0"(Low) or "1"(High)
 * @return int
*/
static int set_gpio_value(int gpio_pin, int value)
{
    char* gpio_value_path = malloc(64);
    memset(gpio_value_path, 0, 64);
    sprintf(gpio_value_path, "/sys/class/gpio/gpio%d/value", gpio_pin);
    FILE* value_file = fopen(gpio_value_path, "w");
    if (value_file==NULL) {
        perror("Failed to open GPIO direction file");
        return -1;
    }
    fprintf(value_file, "%d", value);
    fflush(value_file);

    fclose(value_file);
    free(gpio_value_path);
    return 0;
}
/**
 * @brief Set the gpio unexport object
 * 
 * @param gpio_pin 
 * @return int 
*/
static int set_gpio_unexport(int gpio_pin)
{
    char* gpio_value_path = malloc(64);
    memset(gpio_value_path, 0, 64);
    sprintf(gpio_value_path, "/sys/class/gpio/unexport");
    FILE* unexport_file = fopen(gpio_value_path, "w");

    if (unexport_file==NULL) {
        perror("Failed to open GPIO direction file");
        return -1;
    }
    fprintf(unexport_file, "%d", gpio_pin);
    fclose(unexport_file);
    free(gpio_value_path);
      return 0;
}

static void unexport_gpio(int sing_num)
{
    printf("\nReceived signal: %d\r\n", sing_num);
    if(gpio_numble>0){
        printf("set gpio%d and unexport it\r\n",gpio_numble);
        set_gpio_value(gpio_numble, 1);
        set_gpio_unexport(gpio_numble);
    }
    exit(1);
}

int main(int argc, char** argv)
{
    //注册信号处理
    signal(SIGINT,unexport_gpio);
    if (argc==0) {
        perror("No GPIO input");
        scanf("%s", *argv);
    }
    else
        argv++;
    int value = 0;
    if (strcmp(*argv, "unexport")==0) goto _unexport;
    //导出引脚
    gpio_numble = export_gpio_numble(*argv);
    if (gpio_numble>0) printf("[%s:%d]:%s is gpio%d,export success\r\n", __func__, __LINE__, *argv, gpio_numble);
    //配置引脚为输出
    argv++;
    if (!set_gpio_direction(gpio_numble, *argv)) printf("[%s:%d]:gpio%d direction =%s\r\n", __func__, __LINE__, gpio_numble, get_gpio_direction(gpio_numble)?"out":"in");
    //控制引脚电平
    while (1) {
        if (get_gpio_direction(gpio_numble)) {
            printf("Please output the level of gpio%d control (0 or 1):", gpio_numble);
            scanf("%d", &value);
            set_gpio_value(gpio_numble, value);
            set_gpio_value(gpio_numble, value);
        }

    }
    //取消导出引脚
_unexport:
    
    return 0;
}