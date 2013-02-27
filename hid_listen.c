/* HID Listen, http://www.pjrc.com/teensy/hid_listen.html
 * Listens (and prints) all communication received from a USB HID device,
 * which is useful for view debug messages from the Teensy USB Board.
 * Copyright 2008, PJRC.COM, LLC
 *
 * You may redistribute this program and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rawhid.h"

#include <ncurses.h>

static void delay_ms(unsigned int msec);

#define CTRL(x) ((x) & 0x3f)

int main(void)
{
	char buf[64], *in, *out;
	rawhid_t *hid;
	int num, count, notdone=1;

	int ch;

	initscr();                      /* Start curses mode            */
	raw();                          /* Line buffering disabled      */
	keypad(stdscr, TRUE);           /* We get F1, F2 etc..          */
	noecho();                       /* Don't echo() while we do getch */
	nodelay(stdscr, TRUE);
	scrollok(stdscr, TRUE);
	printw("[Waiting for device, CTRL-C to exit...]");
	refresh();
	while (notdone) {
		//hid = rawhid_open_only1(0, 0, 0xFF31, 0x0074);
		//hid = rawhid_open_only1(0, 0, 0x03eb, 0x2fe4);
		hid = rawhid_open_only1(0x03eb, 0x2ffb, 0, 0);
		if (hid == NULL) {
			ch = getch();
			if(ch == CTRL('C')) {
				notdone=0;
				break;
			}
			//printw(".");
			refresh();
			delay_ms(25);
			continue;
		}
		printw("[Got it]\n");
		refresh();
		while (1) {
		        ch = getch();
			if (ch != ERR) {
				if(ch == CTRL('C')) {
					notdone=0;
					break;
				} else {
					unsigned char tmp[8] = {0,0,0,0,0,0,0,0};
					//printw("[%02x]\n", ch);
					tmp[0] = ch;
					rawhid_write(hid, tmp, sizeof(tmp), 100);
				}

        		}
			num = rawhid_read(hid, buf, sizeof(buf), 100);
			if(rawhid_status(hid) != 0) {
				break;
			} else {
				if (num < 0) break;
				if (num == 0) continue;
				in = out = buf;
				for (count=0; count<num; count++) {
					if (*in) {
						*out++ = *in;
						if(*in != '\r') {
							printw("%c",*in);
						}
					}
					in++;
				}
				count = out - buf;
				//printf("read %d bytes, %d actual\n", num, count);
				if (count) {
					//num = fwrite(buf, 1, count, stdout);
					refresh(); 
				}
			}
		}
		rawhid_close(hid);
		printw("\n[Device disconnected]\n[Waiting for new device]");
	}
	printw("[Ending program]\n");
	endwin();                       /* End curses mode                */
	return 0;
}






#if (defined(WIN32) || defined(WINDOWS) || defined(__WINDOWS__)) 
#include <windows.h>
static void delay_ms(unsigned int msec)
{
	Sleep(msec);
}
#else
#include <unistd.h>
static void delay_ms(unsigned int msec)
{
	usleep(msec * 1000);
}
#endif
