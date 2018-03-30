/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *  Written by Jack Tan <jiankemeng@gmail.com>
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "xtos.h"

// Hardware exception handling
struct exception_ctx {
	uint32_t epc;
	uint32_t ps;
	uint32_t sar;
	uint32_t unused;
	union {
		struct {
			uint32_t a0;
			// note: no a1 here!
			uint32_t a2;
			uint32_t a3;
			uint32_t a4;
			uint32_t a5;
			uint32_t a6;
			uint32_t a7;
			uint32_t a8;
			uint32_t a9;
			uint32_t a10;
			uint32_t a11;
			uint32_t a12;
			uint32_t a13;
			uint32_t a14;
			uint32_t a15;
		};
		uint32_t a_reg[15];
	};
	uint32_t cause;
};

void unaligned_handler(struct exception_ctx *ctx, uint32_t cause)
{
	/* If this is not EXCCAUSE_LOAD_STORE_ERROR you're doing it wrong! */
	(void)cause;

	uint32_t epc1 = ctx->epc;
	uint32_t excvaddr;
	uint32_t insn;

	asm(
		"rsr   %0, EXCVADDR;"	/* read out the faulting address */
	    "movi  a4, ~3;"			/* prepare a mask for the EPC */
	    "and   a4, a4, %2;"		/* apply mask for 32bit aligned base */
	    "l32i  a5, a4, 0;"		/* load part 1 */
	    "l32i  a6, a4, 4;"		/* load part 2 */
	    "ssa8l %2;"				/* set up shift register for src op */
	    "src   %1, a6, a5;"		/* right shift to get faulting instruction */
		 : "=r"(excvaddr), "=r"(insn)
		 : "r"(epc1)
		 : "a4", "a5", "a6"
	);

	uint32_t valmask = 0;
	uint32_t what = insn & LOAD_MASK;

	if (what == L8UI_MATCH)
		valmask = 0xffu;
	else if (what == L16UI_MATCH || what == L16SI_MATCH)
		valmask = 0xffffu;
	else {
 die:
		/* 
		 * Turns out we couldn't fix this, trigger a system break instead
		 * and hang if the break doesn't get handled. This is effectively
		 * what would happen if the default handler was installed.
		*/
		fatal_error(FATAL_ERR_ALIGN, (void *)ctx->epc,
			    (void *)rd_align_txt);
#if 0
		asm ("break 1, 1");
		while (1) {}
#endif
	}

	/* Load, shift and mask down to correct size */
	uint32_t val = (*(uint32_t *) (excvaddr & ~0x3));
	val >>= (excvaddr & 0x3) * 8;
	val &= valmask;

	/* Sign-extend for L16SI, if applicable */
	if (what == L16SI_MATCH && (val & 0x8000))
		val |= 0xffff0000;

	int regno = (insn & 0x0000f0u) >> 4;
	if (regno == 1)
		goto die;	/* we can't support loading into a1, just die */
	else if (regno != 0)
		--regno;	/* account for skipped a1 in exception_frame */

	ctx->a_reg[regno] = val;	/* carry out the load */
	ctx->epc += 3;				/* resume at following instruction */
}
