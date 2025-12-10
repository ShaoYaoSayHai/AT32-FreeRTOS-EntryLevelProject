#include "data_process.h"


/**
 * @brief ?JSON?????????????,??????????
 * @param json_str ???JSON???
 * @param key ??????
 * @param output_value ????????????
 * @return ?????????true,????false
 */
bool json_extract_int(const char *json_str, const char *key, int *output_value)
{
    if (!json_str || !key || !output_value)
    {
        return false;
    }

    size_t key_len = strlen(key);
    const char *ptr = json_str;

    // ??JSON??????
    while (*ptr != '\0')
    {
        // 1. ????????
        if (*ptr == '\"')
        {
            ptr++;
            const char *key_start = ptr;

            // ???????????
            while (*ptr != '\0' && *ptr != '\"')
            {
                ptr++;
            }
            if (*ptr == '\0')
                break;

            size_t current_key_len = ptr - key_start;

            // 2. ??????????????
            if (current_key_len == key_len && strncmp(key_start, key, key_len) == 0)
            {
                ptr++; // ??????"

                // 3. ???????
                while (*ptr != '\0' && *ptr != ':')
                {
                    ptr++;
                }
                if (*ptr == '\0')
                    break;
                ptr++; // ????

                // 4. ??????????
                while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r'))
                {
                    ptr++;
                }
                if (*ptr == '\0')
                    break;

                // 5. ?????
                if (*ptr >= '0' && *ptr <= '9')
                {
                    int value = 0;

                    // ???????????
                    while (*ptr >= '0' && *ptr <= '9')
                    {
                        value = value * 10 + (*ptr - '0');
                        ptr++;
                    }

                    *output_value = value;
                    return true;
                }
                // 6. ???????(??????),???
                else if (*ptr == '\"')
                {
                    // ??????????
                    while (*ptr != '\0' && *ptr != '\"')
                    {
                        ptr++;
                    }
                    if (*ptr == '\0')
                        break;
                    ptr++; // ??????
                    // ??????,?????,????
                }
            }
        }
        ptr++;
    }
    return false; // ?????????????
}

/**
 * @brief ?JSON????????????????
 * @param json_str ???JSON???
 * @param key ??????
 * @param output_value ??????????????????
 * @param output_size ????????
 * @return ?????????true,????false
 */
bool json_extract_string(const char *json_str, const char *key, char *output_value, size_t output_size)
{
    if (!json_str || !key || !output_value || output_size == 0)
    {
        return false;
    }

    size_t key_len = strlen(key);
    const char *ptr = json_str;
    output_value[0] = '\0';

    // ??JSON??????
    while (*ptr != '\0')
    {
        // 1. ????????
        if (*ptr == '\"')
        {
            ptr++;
            const char *key_start = ptr;

            // ???????????
            while (*ptr != '\0' && *ptr != '\"')
            {
                ptr++;
            }
            if (*ptr == '\0')
                break;

            size_t current_key_len = ptr - key_start;

            // 2. ??????????????
            if (current_key_len == key_len && strncmp(key_start, key, key_len) == 0)
            {
                ptr++; // ??????"

                // 3. ???????
                while (*ptr != '\0' && *ptr != ':')
                {
                    ptr++;
                }
                if (*ptr == '\0')
                    break;
                ptr++; // ????

                // 4. ??????????
                while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r'))
                {
                    ptr++;
                }
                if (*ptr == '\0')
                    break;

                // 5. ?????? (??????)
                if (*ptr == '\"')
                {
                    const char *value_start = ++ptr; // ???????????

                    // ?????????
                    while (*ptr != '\0' && *ptr != '\"')
                    {
                        ptr++;
                    }
                    if (*ptr == '\0')
                        break;

                    // ??????????????
                    size_t value_len = ptr - value_start;
                    if (value_len >= output_size)
                    {
                        value_len = output_size - 1;
                    }

                    strncpy(output_value, value_start, value_len);
                    output_value[value_len] = '\0';
                    return true;
                }
            }
        }
        ptr++;
    }
    return false; // ??????????????
}

/**
 * @brief ?JSON?????????????,?????????????? (double)
 * @param json_str ???JSON???
 * @param key ??????
 * @param output_value ????????????????
 * @return ?????????true,????false
 */
bool json_extract_double(const char *json_str, const char *key, double *output_value)
{
    if (!json_str || !key || !output_value)
    {
        return false;
    }

    size_t key_len = strlen(key);
    const char *ptr = json_str;
    char buffer[64]; // ???????????????,????????
    size_t buf_index = 0;

    // ??JSON??????
    while (*ptr != '\0')
    {
        // 1. ????????
        if (*ptr == '\"')
        {
            ptr++;
            const char *key_start = ptr;

            // ???????????
            while (*ptr != '\0' && *ptr != '\"')
            {
                ptr++;
            }
            if (*ptr == '\0')
                break;

            size_t current_key_len = ptr - key_start;

            // 2. ??????????????
            if (current_key_len == key_len && strncmp(key_start, key, key_len) == 0)
            {
                ptr++; // ??????"

                // 3. ???????
                while (*ptr != '\0' && *ptr != ':')
                {
                    ptr++;
                }
                if (*ptr == '\0')
                    break;
                ptr++; // ????

                // 4. ??????????
                while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r'))
                {
                    ptr++;
                }
                if (*ptr == '\0')
                    break;

                // 5. ????? (???????????????)
                // ????? '+'?'-' ?????,??????????????('e'?'E')
                if (*ptr == '+' || *ptr == '-' || (*ptr >= '0' && *ptr <= '9'))
                {
                    buf_index = 0;
                    // ???????????
                    while (*ptr != '\0' &&
                           (*ptr == '+' || *ptr == '-' || *ptr == '.' ||
                            *ptr == 'e' || *ptr == 'E' ||
                            (*ptr >= '0' && *ptr <= '9')))
                    {
                        if (buf_index < sizeof(buffer) - 1)
                        {
                            buffer[buf_index++] = *ptr;
                        }
                        ptr++;
                    }
                    buffer[buf_index] = '\0'; // ??????null??

                    // ??strtod???????double
                    char *endptr;
                    double value = strtod(buffer, &endptr);

                    // ???????? (endptr??????????'\0',??????????)
                    if (endptr != buffer) // ???????????
                    {
                        *output_value = value;
                        return true;
                    }
                    // ????,????(??????,??????)
                }
                // 6. ??????????(?????????????),????????
                else if (*ptr == '\"')
                {
                    const char *value_start = ++ptr; // ???????????

                    // ?????????
                    while (*ptr != '\0' && *ptr != '\"')
                    {
                        ptr++;
                    }
                    if (*ptr == '\0')
                        break;

                    // ????????????
                    size_t value_len = ptr - value_start;
                    if (value_len >= sizeof(buffer))
                    {
                        value_len = sizeof(buffer) - 1;
                    }

                    // ???????????
                    strncpy(buffer, value_start, value_len);
                    buffer[value_len] = '\0';

                    // ??strtod???????double
                    char *endptr;
                    double value = strtod(buffer, &endptr);

                    // ????????
                    if (endptr != buffer)
                    {
                        *output_value = value;
                        return true;
                    }
                    // ????,????
                }
            }
        }
        ptr++;
    }
    return false; // ??????????????????
}
