#include "rtl/rtl.h"
void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
	//printf("\033[32m todo in raise_intr\n");
	s0 = cpu.idtr.Base + 8 * NO;
	s1 = 0x0000ffff & vaddr_read(s0, 4);
	s0 += 4;
	s1 |= 0xffff0000 & vaddr_read(s0, 4);
	cpu.pc = s1;
}

bool isa_query_intr(void) {
  return false;
}
