#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/* WS2816 timing requirement in nano seconds */
#define T0H_MIN     (200)
#define T0H_MAX     (320)
#define T0L_MIN     (800)
#define T0L_MAX     (1200)
#define T1H_MIN     (520)
#define T1H_MAX     (800)
#define T1L_MIN     (480)
#define T1L_MAX     (1000)

#define MIN_PERIOD  (1250)

#define BIT_NUM_INIT    (3)
#define BIT_NUM_MAX     (16)

#define d_printf(...)

int duty_cycle_check(double bit_period_ns, int bits_per_symbol)
{
    int i, j;
    int n_t0h_min = ceil(T0H_MIN / bit_period_ns);
    int n_t0h_max = floor(T0H_MAX / bit_period_ns);
    int n_t0l_min = ceil(T0L_MIN / bit_period_ns);
    int n_t0l_max = floor(T0L_MAX / bit_period_ns);
    int n_t1h_min = ceil(T1H_MIN / bit_period_ns);
    int n_t1h_max = floor(T1H_MAX / bit_period_ns);
    int n_t1l_min = ceil(T1L_MIN / bit_period_ns);
    int n_t1l_max = floor(T1L_MAX / bit_period_ns);

    d_printf("bit period: %lf ns\n", bit_period_ns);
    /*  basic logic check */
    if (n_t0h_min > bits_per_symbol)
        return -1;
    if (n_t0l_min > bits_per_symbol)
        return -1;
    if (n_t1h_min > bits_per_symbol)
        return -1;
    if (n_t1l_min > bits_per_symbol)
        return -1;

    /* minimum bits count should not exceed bits_per_symbol */
    if ((n_t0l_min + n_t0h_min) > bits_per_symbol)
        return -1;
    if ((n_t1l_min + n_t1h_min) > bits_per_symbol)
        return -1;

    /* maximu bits count should not smaller than bits_per_symbol, 
     * otherwise there is no possibility to match bits_per_symbol.
     */
    if ((n_t0h_max + n_t0l_max) < bits_per_symbol) {
        d_printf("T0 bits count does not meet!!!\n");
        return -1;
    }
    if ((n_t1h_max + n_t1l_max) < bits_per_symbol) {
        d_printf("T0 bits count does not meet!!!\n");
        return -1;
    }

    /* one cycle must at least MIN_PERIOD */
    if (bit_period_ns * bits_per_symbol < MIN_PERIOD) {
        d_printf("Time period is too short!!!\n");
        return -1;
    }

    /* logical check */
    if (n_t0h_min > n_t0h_max) {
        d_printf("T0H: Impossible setup!!!\n");
        return -1;
    }
    if (n_t0l_min > n_t0l_max) {
        d_printf("T0L: Impossible setup!!!\n");
        return -1;
    }
    if (n_t1h_min > n_t1h_max) {
        d_printf("T1H: Impossible setup!!!\n");
        return -1;
    }
    if (n_t1l_min > n_t1l_max) {
        d_printf("T1L: Impossible setup!!!\n");
        return -1;
    }

    d_printf("bit period: %lf ns\n", bit_period_ns);

    /* list possible setup */
    for (i = n_t0h_min; i <= n_t0h_max; i++) {
        for (j = n_t0l_min; j <= n_t0l_max; j++) {
            if ((i + j == bits_per_symbol))
                printf("Possible T0 range is high: %d, low: %d\n", i, j);
        }
    }
    for (i = n_t1h_min; i <= n_t1h_max; i++) {
        for (j = n_t1l_min; j <= n_t1l_max; j++) {
            if ((i + j == bits_per_symbol))
                printf("Possible T1 range is high: %d, low: %d\n", i, j);
        }
    }

    d_printf("T0H num bits range: %d ~ %d\n", n_t0h_min, n_t0h_max);
    d_printf("T0L num bits range: %d ~ %d\n", n_t0l_min, n_t0l_max);
    d_printf("T1H num bits range: %d ~ %d\n", n_t1h_min, n_t1h_max);
    d_printf("T1L num bits range: %d ~ %d\n", n_t1l_min, n_t1l_max);
    return 0;
}

int sanity_check_clk(double clk_mhz)
{
    double bit_period_ns = 1000 / clk_mhz;
    int bits = BIT_NUM_INIT;

    while (bits <= BIT_NUM_MAX) {
        if (duty_cycle_check(bit_period_ns, bits) == 0) {
            printf("%lf (Mhz), period: %lf ns, bits_per_symbole: %d\n",
              clk_mhz, bit_period_ns, bits);
            printf("\n");
        }
        bits += 1;
    }
    return 0;
}

void i2s_example()
{
    int i, j;
    int mck_div[] = {2, 3, 4, 5, 6, 8, 10, 11, 15, 16, 21, 23, 30, 31, 32, 42, 63, 125};
    int ratio[] = {32, 48, 64, 96, 128, 192, 256, 384, 512};

    for (i = 0; i < (sizeof(mck_div) / sizeof(int)); i++) {
        for (j = 0; j < (sizeof(ratio) / sizeof(int)); j++) {
            double lrck = (double)32 / mck_div[i] / ratio[j];
            double sck = 32 * lrck;
            sanity_check_clk(sck);
        }
    }
}

int main(int argc, const char *argv[])
{
    i2s_example();

    return 0;
}
