Files contained in mlkem are ppc64le specific assembly implementation for liboqs
and should be able to be modified to use in other applications.

1. intt_ppc.S - Compute inverse NTT
     entry point: ntt_ppc(int16_t *r, const int16_t *q)

2. ntt_ppc.S - Compute full forward NTT
     entry point: intt_ppc(int16_t *r, const int16_t *q)

3. poly_tomont.S - Convert to Montgomery domain
     entry point: poly_tomont_ppc(int16_t *r, const int16_t *q)

4. reduce.S - Barrett reduction, giving unsigned canonical representative
     entry point: reduce_ppc(int16_t *r, const int16_t *q)

5. consts.c - zeta array and constants
     zeta array: extern const int16_t mlk_ppc_qdata[];

6. consts.h - offset definitions used in assembly.
