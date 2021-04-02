// Meatloaf - A Commodore 64/128 multi-device emulator
// https://github.com/idolpx/meatloaf
// Copyright(C) 2020 James Johnston
//
// Meatloaf is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Meatloaf is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Meatloaf. If not, see <http://www.gnu.org/licenses/>.

#ifndef IECDEVICE_H
#define IECDEVICE_H

#if defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#if defined(USE_SPIFFS)
#include <SPIFFS.h>
#elif defined(USE_LITTLEFS)
#include <LittleFS.h>
#endif

#include "global_defines.h"
#include "MemoryInfo.h"
#include "iec.h"
#include "cbmdefines.h"
#include "Petscii.h"
#include "device_db.h"
#include "helpers.h"

// enum  fsType {
// 	IS_FAIL = 0xFF, 	// IFail: SD card or fat not ok
// 	IS_NATIVE = 0,		// Regular file system file state
// 	IS_D64 = 1,
// 	IS_T64 = 2,
// 	IS_M2I = 3,
// 	IS_PRG = 4,
// };

enum OpenState
{
	O_NOTHING,		// Nothing to send / File not found error
	O_INFO,			// User issued a reload sd card
	O_FILE,			// A program file is opened
	O_DIR,			// A listing is requested
	O_FILE_ERR,		// Incorrect file format opened
	O_SAVE_REPLACE, // Save-with-replace is requested
	O_DEVICE_INFO,
	O_DEVICE_STATUS
};

// The base pointer of basic.
#define C64_BASIC_START 0x0801

class iecDevice
{
public:
	iecDevice(iecBus &iec, FS *fileSystem);
	virtual ~iecDevice() {}

	bool begin();

	// The handler returns the current IEC state, see the iec.hpp for possible states.
	void loop(void);

	// Keeping the system date and time as set on a specific moment. The millis() will then keep the elapsed time since
	// moment the time was set.
	void setDateTime(word year, byte month, byte day, byte hour, byte minute, byte second);

	// retrieve the date and time as strings. Current time will be updated according to the elapsed millis before formatting.
	// String will be of format "yyyymmdd hhmmss", if timeOnly is true only the time part will be returned as
	// "hhmmss", this fits the TIME$ variable of cbm basic 2.0 and later.
	char *dateTimeString(char *dest, bool timeOnly);

private:
	void reset(void);

	void sendStatus(void);
	void sendDeviceInfo(void);
	void sendDeviceStatus(void);

	void sendListing(void);
	void sendListingHTTP(void);
	uint16_t sendHeader(uint16_t &basicPtr);
	uint16_t sendLine(uint16_t &basicPtr, uint16_t blocks, char *text);
	uint16_t sendLine(uint16_t &basicPtr, uint16_t blocks, const char *format, ...);
	uint16_t sendFooter(uint16_t &basicPtr);
	void sendFile(void);
	void sendFileHTTP(void);

	void saveFile(void);

	// handler helpers.
	void handleATNCmdCodeOpen(iecBus::ATNCmd &cmd);
	void handleATNCmdCodeDataListen(void);
	void handleATNCmdCodeDataTalk(int chan);
	void handleATNCmdClose(void);

	void handleDeviceCommand(iecBus::ATNCmd &cmd);
	void handleMeatLoafCommand(iecBus::ATNCmd &cmd);

	// our iec low level driver:
	iecBus &m_iec;

	// This var is set after an open command and determines what to send next
	byte m_openState; // see OpenState
	byte m_queuedError;

	// atn command buffer struct
	iecBus::ATNCmd &m_atn_cmd;

	FS *m_fileSystem;
	StaticJsonDocument<256> m_jsonHTTP;
	String m_lineBuffer;
	//DynamicJsonDocument m_jsonHTTPBuffer;

	DeviceDB m_device;
	String m_filename;
	String m_filetype;
};

#elif defined(ESP32)

#include <WiFi.h>
#include <HTTPClient.h>

#if defined(USE_SPIFFS)
#include <SPIFFS.h>
#elif defined(USE_LITTLEFS)
#include <LittleFS.h>
#endif

#include "global_defines.h"
#include "MemoryInfo.h"
#include "iec.h"
#include "cbmdefines.h"
#include "Petscii.h"
#include "device_db.h"
#include "helpers.h"

enum OpenState {
	O_NOTHING,			// Nothing to send / File not found error
	O_INFO,				// User issued a reload sd card
	O_FILE,				// A program file is opened
	O_DIR,				// A listing is requested
	O_FILE_ERR,			// Incorrect file format opened
	O_SAVE_REPLACE,		// Save-with-replace is requested
	O_DEVICE_INFO,
	O_DEVICE_STATUS
};

// The base pointer of basic.
#define C64_BASIC_START 0x0801

#define IMAGE_TYPES "D64|D71|D80|D81|D82|D8B|G64|X64|Z64|TAP|T64|TCRT|CRT|D1M|D2M|D4M|DHD|HDD|DNP|DFI|M2I|NIB"
#define FILE_TYPES "C64|PRG|P00|SEQ|S00|USR|U00|REL|R00"

class iecDevice
{
public:
	iecDevice(iecBus &iec, FS *fileSystem);
	virtual ~iecDevice() {}

	bool begin();

	// The handler services the IEC bus
	void loop(void);

private:
	void reset(void);

	void sendStatus(void);
	void sendDeviceInfo(void);
	void sendDeviceStatus(void);

	void sendListing(void);
	void sendListingHTTP(void);
	uint16_t sendHeader(uint16_t &basicPtr);
	uint16_t sendLine(uint16_t &basicPtr, uint16_t blocks, char* text);
	uint16_t sendLine(uint16_t &basicPtr, uint16_t blocks, const char* format, ...);
	uint16_t sendFooter(uint16_t &basicPtr);
	void sendFile(void);
	void sendFileHTTP(void);

	void saveFile(void);

	// handler helpers.
	void handleATNCmdCodeOpen(iecBus::ATNCmd &cmd);
	void handleATNCmdCodeDataListen(void);
	void handleATNCmdCodeDataTalk(int chan);
	void handleATNCmdClose(void);

	void handleDeviceCommand(iecBus::ATNCmd &cmd);
	void handleMeatLoafCommand(iecBus::ATNCmd &cmd);

	// our iec low level driver:
	iecBus& m_iec;

	// This var is set after an open command and determines what to send next
	int m_openState;			// see OpenState
	int m_queuedError;

	// atn command buffer struct
	iecBus::ATNCmd& m_atn_cmd;

	FS *m_fileSystem;
	StaticJsonDocument<256> m_jsonHTTP;
	String m_lineBuffer;
	//DynamicJsonDocument m_jsonHTTPBuffer;

	DeviceDB m_device;
	String m_filename;
	String m_filetype;
};

#endif
#endif
