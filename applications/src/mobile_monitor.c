#include "mobile_device.h"
#include "mobile_monitor.h"
#include "at32f403a_407_gpio.h"
#include "utils.h"
#include "gt_flash.h"

TaskHandle_t mobile_monitor_task_handle;
static uint32_t last_ip_down_time = 0;

static char mobile_model[16] = {0};
static char mobile_ver[10] = {0};
static char mobile_imei[16] = {0};
static char sim_imsi[20] = {0};
static char sim_iccid[21] = {0};
static char sim_num[21] = {0};
static int rssi = -113;
static char ipv4_addr[16] = {0};

static bool is_sim_card_ready = false;
static bool is_network_ready = false;

static MOBILE_STATUS_t mobile_monitor_status = MOBILE_STATUS_UNKNOWN;
static uint32_t at_cmd_retry = 0;

char resp_buff[128] = {0};

void clear_mobile_status(void)
{
    is_sim_card_ready = 0;
    is_network_ready = 0;
    at_cmd_retry = 0;
    mobile_monitor_status = MOBILE_STATUS_UNKNOWN;
    last_ip_down_time = rtc_counter_get();
}

void reset_mobile_device(void)
{
    if (!is_sim_card_ready)
    {
        gpio_bits_reset(GPIOB, GPIO_PINS_0);
        vTaskDelay(2000);
        gpio_bits_set(GPIOB, GPIO_PINS_0);
    }

    clear_mobile_status();
}

char *get_mobile_imei(void)
{
    return mobile_imei;
}

char *get_sim_imsi(void)
{
    return sim_imsi;
}

char *get_sim_iccid(void)
{
    return sim_iccid;
}

int get_rssi(void)
{
    return rssi;
}

char *get_ipv4_addr(void)
{
    return ipv4_addr;
}

uint8_t get_sim_card_ready(void)
{
    return is_sim_card_ready;
}

uint8_t get_network_ready(void)
{
    return is_network_ready;
}

char *get_mobile_model(void)
{
    return mobile_model;
}

char *get_mobile_ver(void)
{
    return mobile_ver;
}

int32_t mobile_monitor_check_device(void)
{
    memset(resp_buff, 0, sizeof(resp_buff));
    int res = mobile_dev_send_cmd("AT\r\n", resp_buff, sizeof(resp_buff) - 1);
    if (res == 0 && strstr(resp_buff, "OK"))
    {
        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        mobile_dev_send_cmd("ATE0\r\n", resp_buff, sizeof(resp_buff) - 1);

        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        mobile_dev_send_cmd("AT+NVETOSUPERDS=A,1\r\n", resp_buff, sizeof(resp_buff) - 1);

        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        mobile_dev_send_cmd("AT+CMEE=1\r\n", resp_buff, sizeof(resp_buff) - 1);

        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        res = mobile_dev_send_cmd("AT+CGMM\r\n", resp_buff, sizeof(resp_buff) - 1);
        if (res == 0 && strstr(resp_buff, "OK"))
        {
            char *p = strstr(resp_buff, "\r\n");
            if (p)
            {
                p = p + strlen((char *)"\r\n");
                char *p2 = strchr(p, '\r');
                if (p2)
                {
                    *p2 = '\0';
                    p = trim(p);
                    p = trimQuotes(p);
                    strcpy(mobile_model, p);
                    if (strlen(mobile_model) < 4)
                    {
                        return -1;
                    }
                }
            }
        }
        else
        {
            return -1;
        }

        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        res = mobile_dev_send_cmd("AT+CGMR\r\n", resp_buff, sizeof(resp_buff) - 1);
        if (res == 0 && strstr(resp_buff, "OK"))
        {
            char *p = strstr(resp_buff, "\r\n");
            if (p)
            {
                p = p + strlen((char *)"\r\n");
                char *p2 = strchr(p, '\r');
                if (p2)
                {
                    *p2 = '\0';
                    p = trim(p);
                    p = trimQuotes(p);
                    strcpy(mobile_ver, p);
                    if (strlen(mobile_ver) < 4)
                    {
                        return -1;
                    }
                }
            }
        }
        else
        {
            return -1;
        }

        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        res = mobile_dev_send_cmd("AT+CGSN\r\n", resp_buff, sizeof(resp_buff) - 1);
        if (res == 0 && strstr(resp_buff, "OK"))
        {
            char *p = strstr(resp_buff, "+CGSN:");
            if (p)
            {
                p = p + strlen((char *)"+CGSN:");
                p = trim(p);
                p = trimQuotes(p);
                strncpy(mobile_imei, p, 15);
            }
        }
        else
        {
            return -1;
        }

        at_cmd_retry = 0;
        mobile_monitor_status = MOBILE_STATUS_DEVICE_READY;
        return 0;
    }
    else
    {
        at_cmd_retry++;
        if (at_cmd_retry >= 180)
        {
            reset_mobile_device();
        }
        return -1;
    }
}

int32_t mobile_monitor_check_sim_card(void)
{
    memset(resp_buff, 0, sizeof(resp_buff));
    int res = mobile_dev_send_cmd("AT+CPIN?\r\n", resp_buff, sizeof(resp_buff) - 1);
    if (res == 0 && strstr(resp_buff, "READY"))
    {
        is_sim_card_ready = 1;
        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        mobile_dev_send_cmd("AT+ICCID?\r\n", resp_buff, sizeof(resp_buff) - 1);
        char *p = strstr(resp_buff, "+ICCID:");
        if (p)
        {
            p = p + strlen((char *)"+ICCID:");
            p = trim(p);
            p = trimQuotes(p);
            strncpy(sim_iccid, p, 20);
        }
        else
        {
            return -1;
        }

        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        res = mobile_dev_send_cmd("AT+CIMI\r\n", resp_buff, sizeof(resp_buff) - 1);
        if (res == 0 && strstr(resp_buff, "OK"))
        {
            strncpy(sim_imsi, resp_buff + 2, 15);
        }
        else
        {
            return -1;
        }

        at_cmd_retry = 0;
        mobile_monitor_status = MOBILE_STATUS_SIM_CARD_READY;
        return 0;
    }
    else
    {
        at_cmd_retry++;
        if (at_cmd_retry >= 180)
        {
            reset_mobile_device();
        }
        return -1;
    }
}

int32_t mobile_monitor_check_reg(void)
{
    memset(resp_buff, 0, sizeof(resp_buff));
    vTaskDelay(100);
    int res = mobile_dev_send_cmd("AT+CREG?\r\n", resp_buff, sizeof(resp_buff) - 1);
    if (res == 0 && strstr(resp_buff, "OK"))
    {
        char *p = strstr(resp_buff, ",");
        if (p)
        {
            p = p + 1;
            if (p[0] == '1')
            {
                is_network_ready = 1;
            }
            else
            {
                is_network_ready = 0;
            }
        }
        else
        {
            return -1;
        }

        if (is_network_ready)
        {
            memset(resp_buff, 0, sizeof(resp_buff));
            vTaskDelay(100);
            res = mobile_dev_send_cmd("AT+CCLK?\r\n", resp_buff, sizeof(resp_buff) - 1);
            if (res == 0 && strstr(resp_buff, "OK"))
            {
                char *p = strstr(resp_buff, "\"");
                if (p)
                {
                    p = p + strlen((char *)"\"");
                    char *p2 = strstr(p, "+");
                    if (p2)
                    {
                        *p2 = 0;
                        calendar_type date_time;
                        int8_t ret = parse_utc_time(p, &date_time);
                        if (ret == 0)
                        {
                            rtc_time_set(&date_time);
                        }
                    }
                }
            }
            else
            {
                return -1;
            }
        }

        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        mobile_dev_send_cmd("AT+CSQ\r\n", resp_buff, sizeof(resp_buff) - 1);
        p = strstr(resp_buff, "+CSQ:");
        if (p)
        {
            p = p + strlen((char *)"+CSQ:");
            p = trim(p);
            p = trimQuotes(p);
            int tmp1, tmp2;
            sscanf(p, "%d,%d", &tmp1, &tmp2);
            rssi = tmp1 * 2 - 113;
        }
        else
        {
            return -1;
        }
        if (is_network_ready)
        {
            mobile_monitor_status = MOBILE_STATUS_REG_READY;
            at_cmd_retry = 0;
        }
        else
        {
            vTaskDelay(3000);
        }
        return 0;
    }
    else
    {
        at_cmd_retry++;
        if (at_cmd_retry >= 180)
        {
            reset_mobile_device();
        }
        return -1;
    }
}

int32_t mobile_monitor_check_network(void)
{
    memset(resp_buff, 0, sizeof(resp_buff));
    int res = mobile_dev_send_cmd("AT+CGATT?\r\n", resp_buff, sizeof(resp_buff) - 1);
    if (res == 0 && strstr(resp_buff, "OK"))
    {
        char *p = strstr(resp_buff, "+CGATT:");
        if (p)
        {
            p = p + strlen((char *)"+CGATT:");
            p = trim(p);
            p = trimQuotes(p);
            if (p[0] == '1')
            {
                is_network_ready = 1;
                mobile_monitor_status = MOBILE_STATUS_NETWORK_READY;
                last_ip_down_time = 0;
            }
            else
            {
                is_network_ready = 0;
                mobile_monitor_status = MOBILE_STATUS_UNKNOWN;
                last_ip_down_time = rtc_counter_get();
                at_cmd_retry = 0;
                return 0;
            }
        }
        else
        {
            return -1;
        }
        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        mobile_dev_send_cmd("AT+CGPADDR\r\n", resp_buff, sizeof(resp_buff) - 1);
        if (res == 0 && strstr(resp_buff, "OK"))
        {
            p = strstr(resp_buff, "\"");
            p = p + 1;
            char *q = strstr(p, "\"");
            if (q)
            {
                memset(ipv4_addr, 0, sizeof(ipv4_addr));
                strncpy(ipv4_addr, p, q - p);
            }
        }
        else
        {
            return -1;
        }
        memset(resp_buff, 0, sizeof(resp_buff));
        vTaskDelay(100);
        mobile_dev_send_cmd("AT+CSQ\r\n", resp_buff, sizeof(resp_buff) - 1);
        p = strstr(resp_buff, "+CSQ:");
        if (p)
        {
            p = p + strlen((char *)"+CSQ:");
            p = trim(p);
            p = trimQuotes(p);
            int tmp1, tmp2;
            sscanf(p, "%d,%d", &tmp1, &tmp2);
            rssi = tmp1 * 2 - 113;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        at_cmd_retry++;
        if (at_cmd_retry >= 180)
        {
            reset_mobile_device();
        }
        return -1;
    }
    return 0;
}

void mobile_monitor_task(void *pvParameters)
{
    gt_flash_write_log("mobile_monitor_task enter\n");
    int32_t ret;
    clear_mobile_status();
    vTaskDelay(3000);
    while (1)
    {
        // gt_flash_write_log("mobile_monitor_task main\n");
        switch (mobile_monitor_status)
        {
        case MOBILE_STATUS_UNKNOWN:
            ret = mobile_monitor_check_device();
            break;
        case MOBILE_STATUS_DEVICE_READY:
            ret = mobile_monitor_check_sim_card();
            break;
        case MOBILE_STATUS_SIM_CARD_READY:
            ret = mobile_monitor_check_reg();
            break;
        case MOBILE_STATUS_REG_READY:
            ret = mobile_monitor_check_network();
            break;
        case MOBILE_STATUS_NETWORK_READY:
            ret = mobile_monitor_check_network();
            vTaskDelay(10000);
            break;
        }
        vTaskDelay(ret == 0 ? 100 : 1000);
        if (!is_network_ready && ((rtc_counter_get() - last_ip_down_time) > 240))
        {
            reset_mobile_device();
            vTaskDelay(3000);
        }
        if (mobile_dev_has_reboot() == 0)
        {
            clear_mobile_status();
        }
    }
}

void mobile_monitor_create_task(void)
{
    xTaskCreate(mobile_monitor_task,
                "mobile_monitor_TASK",
                512,
                NULL,
                4,
                &mobile_monitor_task_handle);
}
