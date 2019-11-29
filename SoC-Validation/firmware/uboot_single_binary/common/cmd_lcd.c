#include <common.h>
#include <command.h>
#include<lcd.h>
#include <amba_clcd.h>
extern void fill_screen(unsigned color);
extern void lcd_draw_rectangle(int x,int y,int width,int height, unsigned int color);

static void do_fill(void);

static void do_fill(void)
{
	printf("User has given a command to fill whole LCD screen with a color\n");
	char choice, ch;
   
print : printf("Enter your color choice index:\n");
        printf("W. White\n");
        printf("B. Blue\n");
        printf("M. Magenta\n");
	printf("Y. Yellow\n");
	printf("G. Green\n");
	printf("R. Red\n");
        choice = getc();
	printf("choice %c\n", choice);      
	switch(choice){
		case 'W':
		case 'w':	
			fill_screen(CONSOLE_COLOR_WHITE);
			break;
		case 'B':
		case 'b':
			 fill_screen(CONSOLE_COLOR_BLUE);
			 break;
		case 'M':
		case 'm':
			 fill_screen(CONSOLE_COLOR_MAGENTA);
			 break;
		case 'Y':
		case 'y':
		 	 fill_screen(CONSOLE_COLOR_YELLOW);
			 break;
		case 'G':
		case 'g': 
 			 fill_screen(CONSOLE_COLOR_GREEN);
			 break;
		case 'R':
		case 'r':
			 fill_screen(CONSOLE_COLOR_RED);
			 break;
		default:
			printf("You entered invalid Choice\n Do you want to exit Y/N ?\n");
			ch= getc();
			if(ch=='y' || 'Y')
			   break;
			else 
			goto print;
		}		
}

static void do_draw_ract(int height, int width)
{

	int x,y;
#ifdef CONFIG_PL111
	x=panel_info1->vl_col/2;
        y=panel_info1->vl_row/2;
#endif
#ifdef CONFIG_DSI
        x=panel_info2->vl_col/2;
        y=panel_info2->vl_row/2;
#endif
#ifdef CONFIG_HDMI
        x=panel_info3->vl_col/2;
        y=panel_info3->vl_row/2;
#endif		
	lcd_draw_rectangle(x,y,width,height,CONSOLE_COLOR_BLACK);
}

#if 0
void do_lcd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	unsigned long height, width;
        if (strcmp("fill",  argv[1]) == 0 )
	{
		do_fill();
	}
	else if(strcmp("rect", argv[1]) == 0)
	{
		height=simple_strtoul(argv[2], NULL, 10);
		width= simple_strtoul(argv[3], NULL, 10);
		printf("Entered height: %lu and width: %lu\n", height, width);
		do_draw_ract(height, width);
	}else
		printf("Enter a valid command\n");
}

U_BOOT_CMD(
        lcd,     4,      1,      do_lcd,
	"CLCD test command like fill with a color and drawing rectangle",
        "lcd fill - Paint the Screen with one color"
	"lcd rect <height> <width>- Draw rectangle of given height and width"
);

int do_lcd_dump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	struct clcd_config *config;
        struct clcd_registers *regs;
	
        config = (struct  clcd_config *)(panel_info1->priv);
        regs = config->address;

	printf("Address: %x\t Value of tim0 REG: 0x%x\n",&regs->tim0,readl(&regs->tim0));
	printf("Address: %x\t Value of tim1 REG: 0x%x\n",&regs->tim1,readl(&regs->tim1));
	printf("Address: %x\t Value of tim2 REG: 0x%x\n",&regs->tim2,readl(&regs->tim2));
	printf("Address: %x\t Value of tim3 REG: 0x%x\n",&regs->tim3,readl(&regs->tim3));
	printf("Address: %x\t Value of ubas REG: 0x%x\n",&regs->ubas,readl(&regs->ubas));
	printf("Address: %x\t Value of lbas REG: 0x%x\n",&regs->lbas,readl(&regs->lbas));
	printf("Address: %x\t Value of cntl REG: 0x%x\n",&regs->cntl,readl(&regs->cntl));
	printf("Address: %x\t Value of stat REG: 0x%x\n",&regs->stat,readl(&regs->stat));
	printf("Address: %x\t Value of intr REG: 0x%x\n",&regs->intr,readl(&regs->intr));
	printf("Address: %x\t Value of ucur REG: 0x%x\n",&regs->ucur,readl(&regs->ucur));
	printf("Address: %x\t Value of lcur REG: 0x%x\n",&regs->lcur,readl(&regs->lcur));	
	printf("Address: %x\t Value of palette REG: 0x%x\n",&regs->palette,readl(regs->palette));

	printf("Address: %x\t Value of crsrimg REG: 0x%x\n",&regs->crsrimg,readl(regs->crsrimg));
	printf("Address: %x\t Value of crsrctrl REG: 0x%x\n",&regs->crsrctrl,readl(&regs->crsrctrl));
	printf("Address: %x\t Value of crsrcnfg REG: 0x%x\n",&regs->crsrcnfg,readl(&regs->crsrcnfg));
	printf("Address: %x\t Value of crsrplte0 REG: 0x%x\n",&regs->crsrplte0,readl(&regs->crsrplte0));
	printf("Address: %x\t Value of crsrplte1 REG: 0x%x\n",&regs->crsrplte1,readl(&regs->crsrplte1));
	printf("Address: %x\t Value of crsrclip REG: 0x%x\n",&regs->crsrclip,readl(&regs->crsrclip));
}

U_BOOT_CMD(
        lcd_dump,     1,      1,      do_lcd_dump,
        "To dump the LCD(PL-111) register set",
        " "
        " "
);
#endif
