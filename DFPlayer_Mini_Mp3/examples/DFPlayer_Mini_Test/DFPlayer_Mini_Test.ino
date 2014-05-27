/*******************************************************************************
 * DFPlayer_Mini_Mp3, This library provides a quite complete function for      * 
 * DFPlayer mini mp3 module.                                                   *
 * www.github.com/dfrobot/DFPlayer_Mini_Mp3 (github as default source provider)*
 *  DFRobot-A great source for opensource hardware and robot.                  *
 *                                                                             *
 * This file is part of the DFplayer_Mini_Mp3 library.                         *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is free software: you can redistribute it and/or          *
 * modify it under the terms of the GNU Lesser General Public License as       *
 * published by the Free Software Foundation, either version 3 of              *
 * the License, or any later version.                                          *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public            *
 * License along with DFPlayer_Mini_Mp3. If not, see                           *
 * <http://www.gnu.org/licenses/>.                                             *
 *									       *
 ******************************************************************************/

/*
 *	Copyright:	DFRobot
 *	name:		DFPlayer_Mini_Mp3 test code
 *	Author:		lisper <lisper.li@dfrobot.com>
 *	Date:		2014-05-22
 *	Description:	mp3 library for DFPlayer mini board
 */
 
 
#include <SoftwareSerial.h>
#include "DFRobot_utility.h"
#include "DFPlayer_Mini_Mp3.h"

#define BUFSIZE 20
#define CMDNUM 8

uint8_t buf[BUFSIZE];
char *cmdbuf[CMDNUM];

//
void setup () {
        Serial1.begin (9600);
        Serial.begin (9600);
        while (!Serial);

        mp3_set_serial (&Serial1);
        mp3_set_volume (15);
        mp3_get_tf_sum ();
        print_info ();
}

void print_info () {
        Serial.println ("you send:");
        print_hex (send_buf, 10);
        delay (100);
        int recv_leng = read_serial_with_timeout (Serial1, recv_buf, 10, 3);
        if (recv_leng) {
                Serial.println ("you get:");
                print_hex (recv_buf, 10);
        }
}

//
void loop () {        
        int leng;
        leng = read_serial_with_timeout (Serial1, buf, BUFSIZE, 3);
        if (leng) {
                Serial.print ("=>");
                print_hex (buf, 10);
        }
        leng = read_serial_with_timeout (Serial, buf, BUFSIZE, 3);
        if (leng) {
                buf[leng] = '\0';
                Serial.println ((char*)buf);
                int cmdleng = split(cmdbuf, (char*)buf, 8);
                if (cmdleng == 1) {
                        if (strcmp (cmdbuf[0], "next") == 0) {
                                mp3_next ();
                                print_info ();
                        } 
                        else if (strcmp (cmdbuf[0], "prev") == 0) {
                                mp3_prev ();
                                print_info ();
                        } 
                        else if (strcmp (cmdbuf[0], "stop") == 0) {
                                mp3_stop ();
                                print_info ();
                        } 
                        else if (strcmp (cmdbuf[0], "next") == 0) {
                                mp3_next ();
                                print_info ();
                        } 
                        else if (strcmp (cmdbuf[0], "pause") == 0) {
                                mp3_pause ();
                                print_info ();
                        } 
                        else if (strcmp (cmdbuf[0], "state") == 0) {
                                mp3_get_state ();
                                print_info ();
                        } 
                        else if (strcmp (cmdbuf[0], "sum") == 0) {
                                mp3_get_tf_sum ();
                                print_info ();
                        } 
                        else if (strcmp (cmdbuf[0], "current") == 0) {
                                mp3_get_tf_current ();
                                print_info ();
                        }
                        else if (strcmp (cmdbuf[0], "volume") == 0) {
                                mp3_get_volume ();
                                print_info ();
                        }
                } 
                else if (cmdleng == 2) {
                        if (strcmp (cmdbuf[0], "play") == 0) {
                                mp3_play (atoi (cmdbuf[1]));
                                print_info ();
                        }
                        else if (strcmp (cmdbuf[0], "volume") == 0) {
                                mp3_set_volume (atoi (cmdbuf[1]));
                                print_info ();
                        }
                        else if (strcmp (cmdbuf[0], "reply") == 0) {
                                if (strcmp (cmdbuf[1], "true") == 0)
                                        mp3_set_reply (true);
                                else if (strcmp (cmdbuf[1], "false") == 0)
                                        mp3_set_reply (false);
                                print_info ();
                        }
                        else if (strcmp (cmdbuf[0], "mp3") == 0) {
                                mp3_play_mp3 (atoi (cmdbuf[1]));
                                print_info ();
                        }
                        else if (strcmp (cmdbuf[0], "eq") == 0) {
                                mp3_set_EQ (atoi (cmdbuf[1]));
                                print_info ();
                        }
                        else if (strcmp (cmdbuf[0], "single") == 0) {
                                if (strcmp (cmdbuf[1], "true") == 0)
                                        mp3_single_loop (true);
                                else if (strcmp (cmdbuf[1], "false") == 0)
                                        mp3_single_loop (false);
                                print_info ();
                        }
                }

///////////////////////////////////////////////////////////
//                if (cmdleng == 1) {
//                        switch (cmdbuf[0][0]) {
//                        case 't':
//                                mp3_stop ();
//                                break;
//                        case 'n':
//                                mp3_next ();
//                                break;
//                        case 'p':
//                                mp3_prev ();
//                                break;
//                        case 's':
//                                mp3_sleep ();
//                                break;
//                        case 'r':
//                                mp3_reset ();
//                                break;
//                        }
//                }
//                else if (cmdleng == 2) {
//                        switch (cmdbuf[0][0]) {
//                        case 'p':
//                                mp3_play (atoi (cmdbuf[1]));
//                                break;
//                        case 'v':
//                                mp3_set_volume (atoi (cmdbuf[1]));
//                                break;
//                        case 'm':
//                                mp3_play_mp3 (atoi (cmdbuf[1]));
//                                break;
//
//                        }
//                }
//////////////////////////////////////////////////

        }
}





