/**
 *	Integrated Interrupt Controller for RaspberryPi.
 *	@author	James Walmsley <james@fullfat-fs.co.uk>
 *	Refactored by Léonard Bise
 **/
#include "bcm2835_irq.h"
#include "bcm2835_intc.h"

static INTERRUPT_VECTOR g_VectorTable[BCM2835_INTC_TOTAL_IRQ];

typedef struct {
	uint32_t IRQBasic;
	uint32_t Pending1;
	uint32_t Pending2;
	uint32_t FIQCtrl;
	uint32_t Enable1;
	uint32_t Enable2;
	uint32_t EnableBasic;
	uint32_t Disable1;
	uint32_t Disable2;
	uint32_t DisableBasic;
} BCM2835_INTC_REGS;

static volatile BCM2835_INTC_REGS * const pRegs = (BCM2835_INTC_REGS *) (BCM2835_BASE_INTC);

// Remember which interrupts have been enabled:
static unsigned long enabled[3];

static void handleRange (uint32_t pending, const uint32_t base)
{
	while (pending)
	{
		// Get index of first set bit:
		uint32_t bit = 31 - __builtin_clz(pending);

		// Map to IRQ number:
		uint32_t irq = base + bit;

		// Call interrupt handler, if enabled:
		if (g_VectorTable[irq].pfnHandler)
			g_VectorTable[irq].pfnHandler(irq, g_VectorTable[irq].pParam);

		// Clear bit in bitfield:
		pending &= ~(1UL << bit);
	}
}

/**
 *	This is the global IRQ handler on this platform!
 *	It is based on the assembler code found in the Broadcom datasheet.
 *
 **/
void irqHandler (void)
{
	register uint32_t ulMaskedStatus = pRegs->IRQBasic;

	// Bit 8 in IRQBasic indicates interrupts in Pending1 (interrupts 31-0):
	if (ulMaskedStatus & (1UL << 8))
		handleRange(pRegs->Pending1 & enabled[0], 0);

	// Bit 9 in IRQBasic indicates interrupts in Pending2 (interrupts 63-32):
	if (ulMaskedStatus & (1UL << 9))
		handleRange(pRegs->Pending2 & enabled[1], 32);

	// Bits 7 through 0 in IRQBasic represent interrupts 64-71:
	if (ulMaskedStatus & 0xFF)
		handleRange(ulMaskedStatus & 0xFF & enabled[2], 64);
}

void bcm2835_irq_unblock (void)
{
	asm volatile ("cpsie i" ::: "memory");
}

void bcm2835_irq_block (void)
{
	asm volatile ("cpsid i" ::: "memory");
}

void bcm2835_irq_register (const uint32_t irq, FN_INTERRUPT_HANDLER pfnHandler, void *pParam)
{
	if (irq < BCM2835_INTC_TOTAL_IRQ) {
		bcm2835_irq_block();
		g_VectorTable[irq].pfnHandler = pfnHandler;
		g_VectorTable[irq].pParam     = pParam;
		bcm2835_irq_unblock();
	}
}

void bcm2835_irq_enable (const uint32_t irq)
{
	uint32_t mask = 1UL << (irq % 32);

	if (irq <= 31) {
		pRegs->Enable1 = mask;
		enabled[0] |= mask;
	}
	else if (irq <= 63) {
		pRegs->Enable2 = mask;
		enabled[1] |= mask;
	}
	else if (irq < BCM2835_INTC_TOTAL_IRQ) {
		pRegs->EnableBasic = mask;
		enabled[2] |= mask;
	}
}

void bcm2835_irq_disable (const uint32_t irq)
{
	uint32_t mask = 1UL << (irq % 32);

	if (irq <= 31) {
		pRegs->Disable1 = mask;
		enabled[0] &= ~mask;
	}
	else if (irq <= 63) {
		pRegs->Disable2 = mask;
		enabled[1] &= ~mask;
	}
	else if (irq < BCM2835_INTC_TOTAL_IRQ) {
		pRegs->DisableBasic = mask;
		enabled[2] &= ~mask;
	}
}
