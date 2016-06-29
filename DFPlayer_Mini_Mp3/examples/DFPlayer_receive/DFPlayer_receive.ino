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
 *	name:		DFPlayer_receive test code
 *	Author:		ciotto <ciotto@users.noreply.github.com>
 *	Date:		2016-06-29
 *	Description:	Example of use of mp3 library for DFPlayer mini board
 *
 *	this code read number of tracks in the SD card folders 01 - 02 and
 *	random play one track
 * 
 *  The circuit:
 *    * DFR0299 mp3 player attached to pins 19, 18 (serial), 6 (buzy) and power
 *    * speaker attached to DFR0299 pin 6 and 8
 *    
 *  This code is based on CaprerAVR project <https://github.com/ciotto/CaprerAVR>
 */

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

// costants
const int busyPin = 6;              // Arduino pin wired to DFR0299 16 pin
const int volume = 20;              // the volume level
const int foldersCount = 2;              // the volume level

// variables
int tracks[foldersCount] = {0, 0};              // tracks in microSD

void setup() {
  // Init serial
  Serial1.begin (9600);
  Serial.begin (9600);
  while (!Serial);

  // Set Serial for DFPlayer-mini mp3 module
  mp3_set_serial(Serial1);
  // Set logging Serial
  mp3_set_debug_serial(Serial);
  // Set volume (value 0~30)
  mp3_set_volume(volume);
  // Set device to microSD
  mp3_set_device(2);
  
  for (int i=0; i < foldersCount; i++) {
    // Query the total number of microSD card files
    mp3_get_folder_sum(i + 1);
    tracks[i] = mp3_wait_folder_sum();

    Serial.print("Find ");
    Serial.print(tracks[i]);
    Serial.print(" tracks in folder 0");
    Serial.print(i);
    Serial.println(".");
  }
}

void loop() {
  // Read buzy state
  int busyState = digitalRead(busyPin);

  // check if the DFR0299 is not buzy.
  if (busyState == 1) {
      // Play random file in random folder
      int folder = random(1, foldersCount + 1);
      int track = random(1, tracks[folder - 1] + 1);
      
      mp3_play_file_in_folder(folder, track);
      
      Serial.print("Play track ");
      Serial.print(track);
      Serial.print(" in folder ");
      Serial.println(folder);
  }

  delay(500);
}

