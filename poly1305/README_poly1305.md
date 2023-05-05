Poly1305 reference implementation:

To run the test,
	2. Test base 26 bits : poly1305_test 26 or poly1305_test
	2. Test base 64 bits : poly1305_test 64
	3. Test base 44 bits : poly1305_test 44

  poly1305-p10le-ref.s -
    poly1305_p10le_4blocks: Implements with 5 26-bit limbs using instructions
                            vmulouw and vmuleuw for multiplication with 8-way unrolling.
			    The input messsge length must be multiple of 64 bytes.
    poly1305_64s: implements 64 x 64 bits multiplication using instruction vmsumudm.
			     The input messsge length must be multiple of 16 bytes.
    poly1305_emit_64: Final digest calculation.

  poly1305-p10le-44-ref.s -
    poly1305_44: Implements with 44 bits limbs using instructions vmsumudm.
		 The input messsge length must be multiple of 16 bytes.
    poly1305_emit_44: Final digest calculation.

Function call input parameters:

	poly1305_p10le_4blocks(u8 *k, u8 *m, u32 mlen)
	  - k = 32 bytes key + H
	  - r3 = k (r, s), H
	  - r4 = m
	  - r5 = mlen

	poly1305_64s(unisgned char *state, const byte *src, size_t len, int highbit)
	poly1305_44
	  - no highbit if final leftover block (highbit = 0)

	poly1305_emit_64(u8 *h, u8 *s, u8* mac)
	poly1305_emit_44
	  - r3 = h
	  - r4 = s
	  - r5 = mac (mac = h + s)


