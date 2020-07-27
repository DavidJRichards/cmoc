/* M48T08 Timekeeper ram read and set functions */
/* D. Richards, July 25 2020 */
/* 6809 cmoc compiler */
/* cmoc patched for multicomp simon monitor */

#pragma org 0xC000

#include <cmoc.h>

#define	ENOMEM		12	/* Out of Memory */
#define	EINVAL		22	/* Invalid argument */
#define ENOSPC		28	/* No space left on device */
#define EXIT_SUCCESS 0

#define M48T08_RTC_ADDR     0xDff8

#define M48T08_RTC_SET		0x80
#define M48T08_RTC_READ		0x40

#define bcd2bin(x)	(((x) & 0x0f) + ((x) >> 4) * 10)
#define bin2bcd(x)	((((x) / 10) << 4) + (x) % 10)

struct tm {
	/*
	 * the number of seconds after the minute, normally in the range
	 * 0 to 59, but can be up to 60 to allow for leap seconds
	 */
	int tm_sec;
	/* the number of minutes after the hour, in the range 0 to 59*/
	int tm_min;
	/* the number of hours past midnight, in the range 0 to 23 */
	int tm_hour;
	/* the day of the month, in the range 1 to 31 */
	int tm_mday;
	/* the number of months since January, in the range 0 to 11 */
	int tm_mon;
	/* the number of years since 1900 */
	long tm_year;
	/* the number of days since Sunday, in the range 0 to 6 */
	int tm_wday;
	/* the number of days since January 1, in the range 0 to 365 */
	int tm_yday;
};

typedef unsigned char u8;
/* memory mapped structure of timekeeper registers */
struct m48t08_rtc {
	u8	control;
	u8	sec;
	u8	min;
	u8	hour;
	u8	day;
	u8	date;
	u8	month;
	u8	year;
};

struct m48t08_rtc *timekeeper = M48T08_RTC_ADDR;
struct tm clk;      /* global storage for time read from rtc */
struct tm *tm = &clk;


static int m48t08_read_time(struct m48t08_rtc *dev, struct tm *rtc)
{
	u8 control;
	/*
	 * Only the values that we read from the RTC are set. We leave
	 * tm_wday, tm_yday and tm_isdst untouched. Even though the
	 * RTC has RTC_DAY_OF_WEEK, we ignore it, as it is only updated
	 * by the RTC when initially set to a non-zero value.
	 */
    control = dev->control;
    dev->control = M48T08_RTC_READ | control; 
	rtc->tm_sec = bcd2bin(dev->sec);
	rtc->tm_min = bcd2bin(dev->min);
	rtc->tm_hour = bcd2bin(dev->hour);
	rtc->tm_mday = bcd2bin(dev->date);
	rtc->tm_mon = bcd2bin(dev->month);
	rtc->tm_year = bcd2bin(dev->year);
    dev->control = control;

	/*
	 * Account for differences between how the RTC uses the values
	 * and how they are defined in a struct rtc_time;
	 */
	rtc->tm_year += 70;
	if (rtc->tm_year <= 69)
		rtc->tm_year += 100;

	rtc->tm_mon--;
	return EXIT_SUCCESS;//rtc_valid_tm(tm);
}

#if 1
static int m48t08_set_time(struct m48t08_rtc *dev, struct tm *rtc)
{
	unsigned int yrs;
	u8 control;

	yrs = rtc->tm_year + 1900;

	if (yrs < 1970)
		return -EINVAL;

	yrs -= 1970;
	if (yrs > 255)    /* They are unsigned */
		return -EINVAL;

	if (yrs > 169)
		return -EINVAL;

	if (yrs >= 100)
		yrs -= 100;


    control = dev->control;
    dev->control = M48T08_RTC_SET | control; 

	dev->year = (unsigned char)bin2bcd(yrs);
	dev->month = bin2bcd((unsigned char)rtc->tm_mon + 1);
	dev->date = bin2bcd((unsigned char)rtc->tm_mday);

	dev->hour = bin2bcd((unsigned char)rtc->tm_hour);
	dev->min = bin2bcd((unsigned char)rtc->tm_min);
	dev->sec = bin2bcd((unsigned char)rtc->tm_sec);

    dev->control = control; 
	return EXIT_SUCCESS;
}
#endif

int getval(const char *prompt, int deflt)
{
	int val;
	printf("%s %d ",prompt, deflt);
	val = readword();
	if(val == 0) val = deflt;
	return val;
}
	
int main(void)
{
	int v;
    m48t08_read_time(timekeeper, &clk);
    printf("\nRTC read time %04ld-%02d-%02d %02d/%02d/%02d\n",
		tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
    printf("\n");

	if(getval("Type any value to change clock:",0))
	{
		clk.tm_year = getval("Year",1900+tm->tm_year)-1900;
		clk.tm_mon = getval("Month",tm->tm_mon);
		clk.tm_mday = getval("Mday",tm->tm_mday);

		clk.tm_hour = getval("Hour",tm->tm_hour);
		clk.tm_min = getval("Minute",tm->tm_min);
		clk.tm_sec = 0;//getval("Second",tm->tm_sec);
	 
		m48t08_set_time(timekeeper, &clk);    
		printf("CLock set");
		m48t08_read_time(timekeeper, tm);
		printf("\nRTC read time %04ld-%02d-%02d %02d/%02d/%02d\n",
			tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);
	}
    return EXIT_SUCCESS;
}
