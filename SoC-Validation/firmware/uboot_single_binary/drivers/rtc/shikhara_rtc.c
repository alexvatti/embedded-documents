#include <common.h>
#include <command.h>
#include <rtc.h>
#include <asm/arch-anuboard/shikhara_rtc.h>



static int pl031_initted = 0;

/* Enable RTC Start in Control register*/
void rtc_init(void)
{
	writel(RTC_CR_START, SHIKHARA_RTC_BASE+RTC_CR);

	pl031_initted = 1;
}

/*
 * Reset the RTC. We set the date back to 1970-01-01.
 */
void rtc_reset(void)
{
	writel(0x00, SHIKHARA_RTC_BASE+RTC_LR);
	if(!pl031_initted)
		rtc_init();
}

/*
 * Set the RTC
*/
int rtc_set(struct rtc_time *tmp)
{
	unsigned long tim;

	if(!pl031_initted)
		rtc_init();

	if (tmp == NULL) {
		puts("Error setting the date/time\n");
		return -1;
	}

	/* Calculate number of seconds this incoming time represents */
	tim = mktime(tmp->tm_year, tmp->tm_mon, tmp->tm_mday,
	                tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	writel(tim, SHIKHARA_RTC_BASE+RTC_LR);

	return 0;
}

/*
 * Get the current time from the RTC
 */
int rtc_get(struct rtc_time *tmp)
{
	ulong tim;

	if(!pl031_initted)
		rtc_init();

	if (tmp == NULL) {
		puts("Error getting the date/time\n");
		return -1;
	}

	tim = readl(SHIKHARA_RTC_BASE+RTC_DR);

	to_tm (tim, tmp);

	debug ( "Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	return 0;
}



