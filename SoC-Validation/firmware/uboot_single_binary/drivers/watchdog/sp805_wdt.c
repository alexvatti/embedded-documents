/*
 * Watchdog driver for the SP805
 *
 */

#include <common.h>
#include <watchdog.h>
#include <asm/io.h>
#include <sp805_wdt.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_wdt.h>
#include <asm/arch/shikhara_irq.h>

#define CONFIG_SP805_HW_TIMEOUT  2
/*
 * Set the watchdog time interval.
 * Counter is 32 bit.
 */
int sp805_wdt_settimeout(unsigned int timeout)
{
	unsigned int reg;

	struct sp805_wdt *wd = (struct sp805_wdt *)SHIKHARA_WDT_BASE;

	printf("Activating WDT..\n");
		
	reg = SP805_WDLOAD(timeout * SP805_TIMEOUT_FACTOR);

	writel(reg, &wd->wdogload);

	return 0;
}

void sp805_wdt_reset(void)
{
	struct sp805_wdt *wd = (struct sp805_wdt *)SHIKHARA_WDT_BASE;

	/* clear control register */
	writel(0, &wd->wdogcontrol);

	/* Enable WDT */
	writel((SP805_WDCR_RST | SP805_WDCR_INTR), &wd->wdogcontrol);
}

void sp805_wdt_disable(void)
{
	struct sp805_wdt *wd = (struct sp805_wdt *)SHIKHARA_WDT_BASE;

	printf("Deactivating WDT..\n");

	/*
	 * It was defined with CONFIG_WATCHDOG_NOWAYOUT in Linux
	 *
	 * Shut off the timer.
	 * Lock it in if it's a module and we defined ...NOWAYOUT
	 */
	writel(0, &wd->wdogcontrol);
}

#if defined(CONFIG_HW_WATCHDOG)
void hw_watchdog_reset(void)
{
	sp805_wdt_reset();
}

void hw_watchdog_init(void)
{
	/* set timer in ms */
	sp805_wdt_settimeout(CONFIG_SP805_HW_TIMEOUT * 1000);
}
#endif
