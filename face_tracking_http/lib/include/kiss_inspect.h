
#include "_kiss_fft_guts.h"

#ifndef INSPECT_FFTR_CFG_H
#define INSPECT_FFTR_CFG_H

// Filips addition
// =============================================================================
void inspect_fft_state(struct kiss_fft_state *cfg)
{
    // =============================================================================

    // struct kiss_fft_state{
    //    int nfft;
    //    int inverse;
    //    int factors[2*MAXFACTORS];
    //    kiss_fft_cpx twiddles[1];
    // };

    printf(" KISS FFT state \n");
    printf(" {\n");
    printf(" nfft = %d \n", cfg->nfft);
    printf(" inverse = %d \n", cfg->inverse);
    printf(" factors [%d] \n", 2 * MAXFACTORS);

    for (int i = 0; i < 2 * MAXFACTORS; i++)
    {
        printf(" %8d ", cfg->factors[i]);
    }
    printf("\n");
    printf(" twiddles[0].r = %f \n", cfg->twiddles[0].r);
    printf(" twiddles[0].i = %f \n", cfg->twiddles[0].i);

    printf(" }\n\n");

    return;
}

// =============================================================================

struct kiss_fftr_state
{
    kiss_fft_cfg substate;
    kiss_fft_cpx *tmpbuf;
    kiss_fft_cpx *super_twiddles;
};

// =============================================================================
void inspect_fftr_state(struct kiss_fftr_state *cfg)
{
    // =============================================================================

    // struct kiss_fftr_state{
    //     kiss_fft_cfg substate;
    //     kiss_fft_cpx * tmpbuf;
    //     kiss_fft_cpx * super_twiddles;
    // };

    inspect_fft_state(cfg->substate);

    int limit = cfg->substate->nfft / 2;

    for (int i = 0; i < limit; i++)
    {
        printf("  %f", cfg->super_twiddles[i].r);
        printf("  %f", cfg->super_twiddles[i].i);
    }

    limit = cfg->substate->nfft;

    for (int i = 0; i < limit; i++)
    {
        printf("  %f", cfg->tmpbuf[i].r);
        printf("  %f", cfg->tmpbuf[i].i);
    }

    return;
}

// =============================================================================
void copy_fft_state(struct kiss_fft_state *src, struct kiss_fft_state *dst)
{
    // =============================================================================

    dst->nfft = src->nfft;
    dst->inverse = src->inverse;

    int limit = 2 * MAXFACTORS;
    for (int i = 0; i < limit; i++)
    {
        dst->factors[i] = src->factors[i];
    }

    limit = src->nfft;
    for (int i = 0; i < limit; i++)
    {
        dst->twiddles[i].r = src->twiddles[i].r;
        dst->twiddles[i].i = src->twiddles[i].i;
    }

    return;
}

// =============================================================================
void copy_fftr_state(struct kiss_fftr_state *src, struct kiss_fftr_state *dst)
{
    // =============================================================================

    copy_fft_state(src->substate, dst->substate);

    int limit = src->substate->nfft;

    for (int i = 0; i < limit; i++)
    {
        dst->tmpbuf[i].r = src->tmpbuf[i].r;
        dst->tmpbuf[i].i = src->tmpbuf[i].i;
    }

    limit = src->substate->nfft / 2;

    for (int i = 0; i < limit; i++)
    {
        dst->super_twiddles[i].r = src->super_twiddles[i].r;
        dst->super_twiddles[i].i = src->super_twiddles[i].i;
    }
}

#endif
