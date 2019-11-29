/*
*cmd_flyer.c 
*'show flyer' command to show a Test Image of resolution 640x480 to the LCD
*/

#include <common.h>
#include <command.h>

extern void show_flyer(void);

void do_show(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]){
	 if (strcmp("flyer",  argv[1]) == 0){
		show_flyer();
	 }else
                printf("Enter a valid command\n");

}

U_BOOT_CMD(
        show,    2,      1,      do_show,
        "Show flyer Image",
	"flyer     - Paint the Screen with the flyer image"
);



