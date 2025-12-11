Notes:

This MLDSA NTT/INTT implementation support p8/p9/p10+ archs.

Define USE_VMULLHSW for p10 and above, use vmulhsw and vmuluwm to compute
higher and lower order 32 bits of the results.  Without defining USE_VMULLHSW flag,
the implementation runs on p8/p9/p10+ archs.

To facilitate the process of the computation of zetas and coefficients,
we need to set up zeta array specifically for Len=1, the array is setup
as (3, 2, 1, 4) order.  zeta arrays (ntt_qdata and intt_qdata) were pre-arranged
for the implementation.


Files:

  consts.[ch]: Zeta array and constants used in NTT/INTT computations.

  mldsa_ntt_ppc_4x.S:  4x MLDSA NTT computations.
    Entry point: mldsa_ntt_ppc(int32_t *r, const int32_t *ntt_qdata)

  mldsa_intt_ppc_4x.S: 4x MLDSA INTT computations.
    Entry point: mldsa_intt_ppc(int32_t *r, const int32_t *intt_qdata)


