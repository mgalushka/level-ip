#include "syshead.h"
#include "utils.h"

extern int debug;

int run_cmd(char *cmd, ...)
{
    va_list ap;
    char buf[CMDBUFLEN];
    va_start(ap, cmd);
    vsnprintf(buf, CMDBUFLEN, cmd, ap);

    va_end(ap);

    if (debug) {
        printf("EXEC: %s\n", buf);
    }

    return system(buf);
}

void print_hexdump(char *str, int len)
{
    printf("Printing hexdump:\n");
    for (int i = 0; i < len; i ++) {
        if (i % 8 == 0) printf("\n");
        printf("%02x ", (unsigned char)str[i]);
    }

    printf("\n");
}

void print_err(char *str, ...)
{
    va_list ap;
    char buf[200];
    va_start(ap, str);
    vsnprintf(buf, 200, str, ap);

    va_end(ap);

    fprintf(stderr, buf);
}

void print_error(char *str, ...)
{
    va_list ap;
    char buf[200];
    va_start(ap, str);
    vsnprintf(buf, 200, str, ap);

    va_end(ap);

    perror(buf);
}

void print_debug(char *str, ...)
{
    if (!debug) return;

    va_list ap;
    char buf[200];
    va_start(ap, str);
    vsnprintf(buf, 200, str, ap);

    va_end(ap);

    printf(buf);
}

uint32_t sum_every_16bits(void *addr, int count)
{
    register uint32_t sum = 0;
    uint16_t * ptr = addr;

    while( count > 1 )  {
        /*  This is the inner loop */
        sum += * ptr++;
        count -= 2;
    }

    /*  Add left-over byte, if any */
    if( count > 0 )
        sum += * (uint8_t *) ptr;

    return sum;
}

uint16_t checksum(void *addr, int count, int start_sum)
{
    /* Compute Internet Checksum for "count" bytes
     *         beginning at location "addr".
     * Taken from https://tools.ietf.org/html/rfc1071
     */
    uint32_t sum = start_sum;

    sum += sum_every_16bits(addr, count);

    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

int get_address(char *host, char *port, struct sockaddr *addr)
{
    addrinfo hints;
    addrinfo *result, *rp;
    int s;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    s = getaddrinfo(host, port, &hints, &result);

    if (s != 0) {
        print_error("getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        *addr = *rp->ai_addr;
        freeaddrinfo(result);
        return 0;
    }

    return 1;
}

uint32_t parse_ipv4_string(char* addr) {
    uint8_t addr_bytes[4];
    sscanf(addr, "%hhu.%hhu.%hhu.%hhu", &addr_bytes[3], &addr_bytes[2], &addr_bytes[1], &addr_bytes[0]);
    return addr_bytes[0] | addr_bytes[1] << 8 | addr_bytes[2] << 16 | addr_bytes[3] << 24;
}
