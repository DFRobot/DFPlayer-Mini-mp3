/*******************************************************************************
 * Copyright (C) 2014 DFRobot						       *
 *									       *
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
 ******************************************************************************/

/*
 *	name:		DFPlayer_Mini_Mp3
 *	version:	1.0
 *	Author:		lisper <lisper.li@dfrobot.com>
 *	Date:		2014-05-22
 *	Description:	mp3 library for DFPlayer mini board
 *			note: mp3 file must put into mp3 folder in your tf card
 */


#include <Arduino.h>
#include <SoftwareSerial.h>
//#include  "DFPlayer_Mini_Mp3.h"

extern uint8_t send_buf[10];
extern uint8_t recv_buf[10];

unsigned long _last_call = 0;

static void(*send_func)() = NULL;
static boolean(*recv_func)() = NULL;
static HardwareSerial * _hardware_serial = NULL;
static SoftwareSerial * _software_serial = NULL;
static HardwareSerial * _debug_serial = NULL;
static boolean is_reply = false;

//
void mp3_set_reply (boolean state) {
	is_reply = state;
	send_buf[4] = is_reply;
}

//
static void fill_uint16_bigend (uint8_t *thebuf, uint16_t data) {
	*thebuf =	(uint8_t)(data>>8);
	*(thebuf+1) =	(uint8_t)data;
}


//calc checksum (1~6 byte)
uint16_t mp3_get_checksum (uint8_t *thebuf) {
	uint16_t sum = 0;
	for (int i=1; i<7; i++) {
		sum += thebuf[i];
	}
	return -sum;
}

//fill checksum to send_buf (7~8 byte)
void mp3_fill_checksum () {
	uint16_t checksum = mp3_get_checksum (send_buf);
	fill_uint16_bigend (send_buf+7, checksum);
}

//
void h_send_func () {
	for (int i=0; i<10; i++) {
		_hardware_serial->write (send_buf[i]);
	}
}

//
void s_send_func () {
	for (int i=0; i<10; i++) {
		_software_serial->write (send_buf[i]);
	}
}

// Reset buffer
void reset_recv_buf () {
	for (int i=0; i<10; i++) {
		recv_buf[i] = 0;
	}
}

// Read data from hardware serial
boolean h_recv_func () {
	reset_recv_buf();

	for (int i=0; i<10; i++) {
		int b = _hardware_serial->read ();
		if (b == -1) {
			return false;
		}
		recv_buf[i] = b;
	}
			
	return true;
}

// Read data from software serial
boolean s_recv_func () {
	delay(5);
	
	reset_recv_buf();

	if (_software_serial->available () < 10) {
		return false;
	}
	for (int i=0; i<10; i++) {
		int b = _software_serial->read ();
		if (b == -1) {
			return false;
		}
		recv_buf[i] = b;
	}
			
	return true;
}

//
//void mp3_set_serial (HardwareSerial *theSerial) {
void mp3_set_serial (HardwareSerial &theSerial) {
	_hardware_serial = &theSerial;
	send_func = h_send_func;
	recv_func = h_recv_func;
}

//
void mp3_set_serial (SoftwareSerial &theSerial) {
	_software_serial = &theSerial;
	send_func = s_send_func;
	recv_func = s_recv_func;
}

// Set the serial port used for logging
void mp3_set_debug_serial (HardwareSerial &theSerial) {
	_debug_serial = &theSerial;
}

//
void mp3_send_cmd (uint8_t cmd, uint16_t high_arg, uint16_t low_arg) {
	unsigned long d = abs(millis() - _last_call);
	if (d < 50) {
		delay(50 - d);
	}

	send_buf[3] = cmd;

	send_buf[5] = high_arg;
	send_buf[6] = low_arg;

	mp3_fill_checksum ();
	send_func ();

	_last_call = millis();
}

//
void mp3_send_cmd (uint8_t cmd, uint16_t low_arg) {
	mp3_send_cmd(cmd, 0, low_arg);
}

//
void mp3_send_cmd (uint8_t cmd) {
	mp3_send_cmd(cmd, 0, 0);
}

// Send byte1 (0-15) as first byte of high and byte234 (0 - 4095) splitted in high byte 2 and low
void mp3_send_cmd (uint8_t cmd, uint8_t byte1, uint32_t byte234) {
	uint16_t low = byte234 & 0x00FF;
	uint16_t high = (byte1 << 4) | (byte234 >> 8);

	mp3_send_cmd (cmd, high, low);
}

// Wait and receive replay for specific command
uint8_t* mp3_recv_cmd (uint8_t wait) {
	uint8_t static result[2] = {0, 0};
	uint8_t received = 0;

	do {
	    boolean read = recv_func();

	    if (read) {
	    	received = recv_buf[3];

	    	if (received == 0x40) {
	    		// Error responce
	    		(* _debug_serial).print("Error responce with code ");
	    		(* _debug_serial).println(recv_buf[6]);
	    	}else if (wait != 0 && wait == received) {
	            result[0] = recv_buf[5];
	            result[1] = recv_buf[6];
	    	}
	    } else {
	    	delay(50);
	    }
	} while (wait != 0 && wait != received);

    return result;
}

// Receive replay
uint8_t* mp3_recv_cmd () {
	return mp3_recv_cmd(0);
}

// Wait and receive replay as int for specific command
int mp3_recv_int_cmd (int wait) {
	int res = 0;
	uint8_t* result = mp3_recv_cmd(wait);

    res = (unsigned char)result[0];
    res = res * 0xFF + (unsigned char)result[1];

    return res;
}

// Receive replay as int
int mp3_recv_int_cmd () {
	return mp3_recv_int_cmd(0);
}


//
void mp3_play_physical (uint16_t num) {
	mp3_send_cmd (0x03, num);
}

//
void mp3_play_physical () {
	mp3_send_cmd (0x03);
}

//
void mp3_next () {
	mp3_send_cmd (0x01);
}

//
void mp3_prev () {
	mp3_send_cmd (0x02);
}

//0x06 set volume 0-30
void mp3_set_volume (uint16_t volume) {
	mp3_send_cmd (0x06, volume);
}

//0x07 set EQ0/1/2/3/4/5    Normal/Pop/Rock/Jazz/Classic/Bass
void mp3_set_EQ (uint16_t eq) {
	mp3_send_cmd (0x07, eq);
}

//0x09 set device 1/2/3/4/5 U/SD/AUX/SLEEP/FLASH
void mp3_set_device (uint16_t device) {
	mp3_send_cmd (0x09, device);
}

//
void mp3_sleep () {
	mp3_send_cmd (0x0a);
}

//
void mp3_reset () {
	mp3_send_cmd (0x0c);
}

//
void mp3_play () {
	mp3_send_cmd (0x0d);
}

//
void mp3_pause () {
	mp3_send_cmd (0x0e);
}

//
void mp3_stop () {
	mp3_send_cmd (0x16);
}

//play mp3 file in mp3 folder in your tf card
void mp3_play (uint16_t num) {
	mp3_send_cmd (0x12, num);
}

//
void mp3_get_state () {
	mp3_send_cmd (0x42);
}
// Wait for mp3_get_state reply
int mp3_wait_state () {
	return mp3_recv_int_cmd(0x42);
}

//
void mp3_get_volume () {
	mp3_send_cmd (0x43);
}
// Wait for mp3_get_volume reply
int mp3_wait_volume () {
	return mp3_recv_int_cmd(0x43);
}

//
void mp3_get_u_sum () {
	mp3_send_cmd (0x47);
}
// Wait for mp3_get_u_sum reply
int mp3_wait_u_sum () {
	return mp3_recv_int_cmd(0x47);
}

//
void mp3_get_tf_sum () {
	mp3_send_cmd (0x48);
}
// Wait for mp3_get_tf_sum reply
int mp3_wait_tf_sum () {
	return mp3_recv_int_cmd(0x48);
}

//
void mp3_get_flash_sum () {
	mp3_send_cmd (0x49);
}
// Wait for mp3_get_flash_sum reply
int mp3_wait_flash_sum () {
	return mp3_recv_int_cmd(0x49);
}

//
void mp3_get_tf_current () {
	mp3_send_cmd (0x4c);
}
// Wait for mp3_get_tf_current reply
int mp3_wait_tf_current () {
	return mp3_recv_int_cmd(0x4c);
}

//
void mp3_get_u_current () {
	mp3_send_cmd (0x4b);
}
// Wait for mp3_get_u_current reply
int mp3_wait_u_current() {
	return mp3_recv_int_cmd(0x4b);
}

//
void mp3_get_flash_current () {
	mp3_send_cmd (0x4d);
}
// Wait for mp3_get_flash_current reply
int mp3_wait_flash_current () {
	return mp3_recv_int_cmd(0x4d);
}

//
void mp3_single_loop (boolean state) {
	mp3_send_cmd (0x19, !state);
}

//add 
void mp3_single_play (uint16_t num) {
	mp3_play (num);
	delay (10);
	mp3_single_loop (true); 
	//mp3_send_cmd (0x19, !state);
}

//
void mp3_DAC (boolean state) {
	mp3_send_cmd (0x1a, !state);
}

//
void mp3_random_play () {
	mp3_send_cmd (0x18);
}

// Query total file numbers of a folder
void mp3_get_folder_sum (uint16_t folder) {
	mp3_send_cmd (0x4E, folder);
}
// Wait for mp3_get_folder_sum reply
int mp3_wait_folder_sum () {
	return mp3_recv_int_cmd(0x4E);
}

// Play mp3 file in selected folder
void mp3_play_file_in_folder (uint8_t folder, uint32_t num) {
	mp3_send_cmd (0x14, folder, num);
}

