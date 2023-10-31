# ECC Support

## secp384r1

Provide composable routines for a Solinas' to secp384r1. These routines only implement modular arithmetic, which can be combined to implement point operations. `secp384r1.c` provides the C implementation to each routine, with each routine having an ISA 3.0 equivalent in `secp384r1.S`. Compiling the C variant requires a compiler with support for 128-bit unsigned integers.
