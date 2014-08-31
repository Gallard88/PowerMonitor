/**
 * \file
 *
 * \brief User board initialization template
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#define WDT_PERIOD                        3000

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	uint32_t wdt_mode, timeout_value;
	
	PIOA->PIO_PER = PIO_PA8;	// Enable Register
	PIOA->PIO_OER = PIO_PA8;
	PIOA->PIO_CODR = PIO_PA8;

	PIOA->PIO_PER = PIO_PA9;	// Enable Register
	PIOA->PIO_OER = PIO_PA9;
	PIOA->PIO_CODR = PIO_PA9;
	
	PIOA->PIO_PER = PIO_PA10;	// Enable Register
	PIOA->PIO_OER = PIO_PA10;
	PIOA->PIO_CODR = PIO_PA10;
	

	timeout_value = wdt_get_timeout_value(WDT_PERIOD * 1000, BOARD_FREQ_SLCK_XTAL);
	/* Configure WDT to trigger an interrupt (or reset). */
	wdt_mode = WDT_MR_WDRPROC |	/* WDT fault resets processor only. */
	    WDT_MR_WDRSTEN  |
//		WDT_MR_WDDBGHLT |	/* WDT stops in debug state. */
		WDT_MR_WDIDLEHLT;	/* WDT stops in idle state. */
		
	/* Initialize WDT with the given parameters. */
	wdt_init(WDT, wdt_mode, timeout_value, timeout_value);

	pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA12A_MISO | PIO_PA13A_MOSI | PIO_PA14A_SPCK );
	pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA5A_RXD0 | PIO_PA6A_TXD0 );
}
