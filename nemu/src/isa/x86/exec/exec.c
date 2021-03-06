#include "cpu/exec.h"
#include "all-instr.h"

static inline void set_width(int width) {
  /*PA2.2: adjust for movzx*/
	if(width == 3){/*movzbw, movzbl*/
		decinfo.src.width = decinfo.src2.width = 1;
		decinfo.dest.width = decinfo.isa.is_operand_size_16 ? 2 : 4; //will do it in make_EHelper(movzx)
		return;
	}
	if(width == 5){/*movzwl*/
		decinfo.src.width = decinfo.src2.width = 2;
		decinfo.dest.width = decinfo.isa.is_operand_size_16 ? 2 : 4;; //will do it in make_EHelper(movzx)
		return;
	}
	/*adjust end*/
	if (width == 0) {
    width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  }
  decinfo.src.width = decinfo.dest.width = decinfo.src2.width = width;
}

static make_EHelper(2byte_esc);

#define make_group(name, item0, item1, item2, item3, item4, item5, item6, item7) \
  static OpcodeEntry concat(opcode_table_, name) [8] = { \
    /* 0x00 */	item0, item1, item2, item3, \
    /* 0x04 */	item4, item5, item6, item7  \
  }; \
static make_EHelper(name) { \
  idex(pc, &concat(opcode_table_, name)[decinfo.isa.ext_opcode]); \
}

/* 0x80, 0x81, 0x83 */
make_group(gp1,
    /*0x83 /0 add*//*EXW(add, 3)*/EX(add), /*80 /1 or*/EX(or), /*80 /2 adc*/EX(adc), /*/3*/EX(sbb),
    /*0x83 /4 and*/EX(and), /*0x83 /5 sub*/EX(sub), EX(xor), /*83 /7 cmp*/EX(cmp))

/* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 */
make_group(gp2,
		/*d3 /0*/EX(rol), EMPTY, EMPTY, EMPTY,
    /*d3 /4*/EX(shl), /*/5 shr*/EX(shr), EMPTY, /*c1 /7*/EX(sar))

/* 0xf6, 0xf7 */
make_group(gp3,
    /*f6 /0 test*/EX(test), EMPTY, /*f7 /2 not*/EX(not), /*f7 /3 neg*/EX(neg),
    /*f7 /4 mul*/EX(mul), /*/5 imul*/EX(imul1), /*f7 /6*/EX(div), /*f7 /7 idiv*/EX(idiv))

/* 0xfe */
make_group(gp4,
    EMPTY, /*/1 dec*/EX(dec), EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY)

/* 0xff */
make_group(gp5,
    /*ff /0 inc*/EX(inc), /*ff /1*/EX(dec), /*0xff /2 call*/EX(call), EMPTY,
    /*ff /4 jmp*/EX(jmp), EMPTY, /*0xff /6 pushw/l*/EX(push), /*ff /7*/EMPTY)

/* 0x0f 0x01*/
make_group(gp7,
    EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY)

/* TODO: Add more instructions!!! */

static OpcodeEntry opcode_table [512] = {
  /* 0x00 */	/*00 add*/IDEXW(G2E, add, 1), /*0x01 add*/IDEX(G2E, add), IDEXW(E2G, add, 1), /*03 add r/mv to rv*/IDEX(E2G, add),
  /* 0x04 */	/*add*/IDEXW(I2a, add, 1), IDEX(I2a, add), EMPTY, EMPTY,
  /* 0x08 */	EMPTY, /*09 or*/IDEX(G2E, or), /*0a or*/IDEXW(E2G, or, 1), /*0b or*/IDEX(E2G, or),
  /* 0x0c */	EMPTY, /*0d*/IDEX(I2a, or), EMPTY, EX(2byte_esc),
  /* 0x10 */	EMPTY, EMPTY, EMPTY, /*13 adc r/mv to rv*/IDEX(E2G, adc),
  /* 0x14 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x18 */	/*18 sbb*/IDEXW(G2E, sbb, 1), /*sbb*/IDEX(G2E, sbb), IDEXW(E2G, sbb, 1), /*1b sbb*/IDEX(E2G, sbb),
  /* 0x1c */	/*sbb*/IDEXW(I2a, sbb, 1), IDEX(I2a, sbb), EMPTY, EMPTY,
  /* 0x20 */	/*20 and*/IDEXW(G2E, and, 1), /*21 and*/IDEX(G2E, and), /*22 and*/IDEXW(E2G, and, 1), IDEX(E2G, and),
  /* 0x24 */	IDEXW(I2a, and, 1), /*25 and*/IDEX(I2a, and), EMPTY, EMPTY,
  /* 0x28 */	/*28 sub*/IDEXW(G2E, sub, 1), /*29 sub*/IDEX(G2E, sub), IDEXW(E2G, sub, 1), IDEX(E2G, sub),
  /* 0x2c */	/*2c sub*/IDEXW(I2a, sub, 1), IDEX(I2a, sub), EMPTY, EMPTY,
  /* 0x30 */	/*30 xor*/IDEXW(G2E, xor, 1), /*0x31 xor*/IDEX(G2E, xor), IDEXW(E2G, xor, 1), /*33 xor*/IDEX(E2G, xor),
  /* 0x34 */	/*xor*/IDEXW(I2a, xor, 1), IDEX(I2a, xor), EMPTY, EMPTY,
  /* 0x38 */	/*38 cmp*/IDEXW(G2E, cmp, 1), /*39 cmp*/IDEX(G2E, cmp), IDEXW(E2G, cmp, 1), /*0x3b cmp*/IDEX(E2G, cmp),
  /* 0x3c */	IDEXW(I2a, cmp, 1), /*3d cmp*/IDEX(I2a, cmp), EMPTY, EMPTY,

	/*inc 0-7 dec 8-f gpr*/
  /* 0x40 */	/*40 inc*/IDEX(r, inc), IDEX(r, inc), IDEX(r, inc), IDEX(r, inc),
  /* 0x44 */	IDEX(r, inc), IDEX(r, inc), IDEX(r, inc), IDEX(r, inc),
  /* 0x48 */	IDEX(r, dec), IDEX(r, dec), IDEX(r, dec), /*4b dec*/IDEX(r, dec),
  /* 0x4c */	IDEX(r, dec), IDEX(r, dec), IDEX(r, dec), IDEX(r, dec),
	
	/*push gpr16/32 from 50 to 57*/
  /* 0x50 */	/*0x50 push r16/32*/IDEX(r, push), /*0x51*/IDEX(r, push), IDEX(r, push), IDEX(r, push),
  /* 0x54 */	IDEX(r, push), /*0x55 push r16/32*/IDEX(r, push), IDEX(r, push), IDEX(r, push),
  
	/*pop gpr16/32 from 58 to 5f*/
	/* 0x58 */	IDEX(r, pop), IDEX(r, pop), IDEX(r, pop), IDEX(r, pop),
  /* 0x5c */	IDEX(r, pop), /*0x5d pop r16/32*/IDEX(r, pop), IDEX(r, pop), IDEX(r, pop),

  /* 0x60 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */	EMPTY, EMPTY, EX(operand_size), EMPTY,
  /* 0x68 */	/*TODO 0x68 push Iv*/IDEX(I, push), /*69 imul3*/IDEX(I_E2G, imul3), /*6a push imm8*/IDEXW(I, push, 1), EMPTY,
  /* 0x6c */	EMPTY, EMPTY, EMPTY, EMPTY,
 
 	/*jcc from 70 to 7f*/
	/* 0x70 */	IDEXW(J, jcc, 1), IDEXW(J, jcc, 1), IDEXW(J, jcc, 1), IDEXW(J, jcc, 1),
	/* 0x74 */	IDEXW(J, jcc, 1), IDEXW(J, jcc, 1), IDEXW(J, jcc, 1), IDEXW(J, jcc, 1),
	/* 0x78 */	IDEXW(J, jcc, 1), IDEXW(J, jcc, 1), IDEXW(J, jcc, 1), IDEXW(J, jcc, 1),
	/* 0x7c */	IDEXW(J, jcc, 1), IDEXW(J, jcc, 1), IDEXW(J, jcc, 1), IDEXW(J, jcc, 1),
  
	/* 0x80 */	IDEXW(I2E, gp1, 1), IDEX(I2E, gp1), EMPTY, IDEX(SI2E, gp1),
  /* 0x84 */	/*84 test*/IDEXW(G2E, test, 1), /*85 test*/IDEX(G2E, test), EMPTY, EMPTY,
  /* 0x88 */	IDEXW(mov_G2E, mov, 1), IDEX(mov_G2E, mov), IDEXW(mov_E2G, mov, 1), IDEX(mov_E2G, mov),
  /* 0x8c */	EMPTY, /*0x8d lea*/IDEX(lea_M2G, lea), EMPTY, EMPTY,
  /* 0x90 */	/*0x90 nop*/EX(nop), EMPTY, EMPTY, EMPTY,
  /* 0x94 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x98 */	/*98 cbw*/EX(cwtl), /*99 cltd*/EX(cltd), EMPTY, EMPTY,
  /* 0x9c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa0 */	IDEXW(O2a, mov, 1), IDEX(O2a, mov), IDEXW(a2O, mov, 1), IDEX(a2O, mov),
  /* 0xa4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */	/*a8 test*/IDEXW(I2a, test, 1), /*test*/IDEX(I2a, test), EMPTY, EMPTY,
  /* 0xac */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xb0 */	IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1),
  /* 0xb4 */	IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1),
  /* 0xb8 */	IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov),
  /* 0xbc */	IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov),
  /* 0xc0 */	IDEXW(gp2_Ib2E, gp2, 1), /*TODO c1 shift r/l*/IDEX(gp2_Ib2E, gp2), EMPTY, /*0xc3 ret16/32*/EX(ret),
  /* 0xc4 */	EMPTY, EMPTY, IDEXW(mov_I2E, mov, 1), IDEX(mov_I2E, mov),
  /* 0xc8 */	EMPTY, /*c9 leave*/EX(leave), EMPTY, EMPTY,
  /* 0xcc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd0 */	IDEXW(gp2_1_E, gp2, 1), IDEX(gp2_1_E, gp2), IDEXW(gp2_cl2E, gp2, 1), /*d3 shift*/IDEX(gp2_cl2E, gp2),
  /* 0xd4 */	EMPTY, EMPTY, EX(nemu_trap), EMPTY,
  /* 0xd8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe8 */	/*0xe8 call r16/32*/IDEX(push_SI, call), /*TODO e9 jmp*/IDEX(J, jmp), EMPTY, /*TODO eb jmp*/IDEXW(J, jmp, 1),
  /* 0xec */	/*ec in*/IDEXW(in_dx2a, in, 1), /*ed in*/IDEX(in_dx2a, in), /*ee out*/IDEXW(out_a2dx, out, 1), /*ef out*/IDEX(out_a2dx, out),
  /* 0xf0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */	EMPTY, EMPTY, IDEXW(I2E, gp3, 1), IDEX(E, gp3),
  /* 0xf8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */	EMPTY, EMPTY, /*fe dec*/IDEXW(E, gp4, 1), IDEX(E, gp5),

/*2 byte_opcode_table */

  /* 0x00 */	EMPTY, IDEX(gp7_E, gp7), EMPTY, EMPTY,
  /* 0x04 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x08 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x0c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x10 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x14 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x18 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x1c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x20 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x24 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x28 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x2c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x30 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x34 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x38 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x3c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x40 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x44 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x48 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x4c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x50 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x54 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x58 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x5c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x60 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x68 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x6c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x70 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x74 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x78 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x7c */	EMPTY, EMPTY, EMPTY, EMPTY,

	/*long jcc from 80 to 8f*/
  /* 0x80 */	IDEX(J, jcc), IDEX(J, jcc), IDEX(J, jcc), IDEX(J, jcc),
  /* 0x84 */	IDEX(J, jcc), IDEX(J, jcc), IDEX(J, jcc), IDEX(J, jcc),
  /* 0x88 */	IDEX(J, jcc), IDEX(J, jcc), IDEX(J, jcc), IDEX(J, jcc),
  /* 0x8c */	IDEX(J, jcc), IDEX(J, jcc), IDEX(J, jcc), IDEX(J, jcc),

  /*from 90 to 9f all setcc one byte*/
	/* 0x90 */	IDEXW(E, setcc, 1), IDEXW(E, setcc, 1), IDEXW(E, setcc, 1), IDEXW(E, setcc, 1),
  /* 0x94 */	IDEXW(E, setcc, 1), IDEXW(E, setcc, 1), IDEXW(E, setcc, 1), IDEXW(E, setcc, 1),
  /* 0x98 */	IDEXW(E, setcc, 1), IDEXW(E, setcc, 1), IDEXW(E, setcc, 1), IDEXW(E, setcc, 1),
  /* 0x9c */	IDEXW(E, setcc, 1), IDEXW(E, setcc, 1), IDEXW(E, setcc, 1), IDEXW(E, setcc, 1),
  
	/* 0xa0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xac */	EMPTY, EMPTY, EMPTY, /*0f af imul*/IDEX(E2G, imul2),
  /* 0xb0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xb4 */	EMPTY, EMPTY, /*0f b6 movzbv TODO*/IDEX(Eb2G, movzx), /*0f b7 movzwv TODO*/IDEX(Ew2G, movzx),
  /* 0xb8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xbc */	EMPTY, EMPTY, /*be movsx TODO*/IDEX(Eb2G, movsx), /*bf movsx TODO*/IDEX(Ew2G, movsx),
  /* 0xc0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xc4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xc8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xcc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xec */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */	EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(2byte_esc) {
  uint32_t opcode = instr_fetch(pc, 1) | 0x100;/*opcode_table 2-byte opcode*/
  decinfo.opcode = opcode;
  set_width(opcode_table[opcode].width);
  idex(pc, &opcode_table[opcode]);
}

void isa_exec(vaddr_t *pc) {
  uint32_t opcode = instr_fetch(pc, 1);
  decinfo.opcode = opcode;
  set_width(opcode_table[opcode].width);
  idex(pc, &opcode_table[opcode]);
}
