This reference implements 8-way, 4-way unrolling and handles partial 4 blocks.
Each block consits 64 bytes.

chacha-p10le-ref.S -
	chacha_p10le_8x (main function): Implements 8-way and 4-wyas unrolling.
					 Also handle partial 4 blocks.

Input parameters -
	`chacha_p10le_8x(u32 *state, u8 *dst, u8 *src, size_t len, int nrounds)`
	  - `state`: consists keys
	  - `dst`: encrypted message
	  - `src`: plaintext
	  - `len`: message length
	  - `nrounds`: normally 20 rounds.
