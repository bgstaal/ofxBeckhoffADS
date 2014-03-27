#pragma once
#include "ofMain.h"
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

// TODO: Enable caching of handles

class ofxBeckhoffADS
{
	public:

		ofxBeckhoffADS ()
		{
			long v = AdsGetDllVersion();
			AdsVersion *pDLLVersion = (AdsVersion *)v;
			ofLog() << "ADS Version: " << (int)pDLLVersion->version << endl;
			ofLog() << "ADS Revision: " << (int)pDLLVersion->revision << endl;
			ofLog() << "ADS Build: " << pDLLVersion->build << endl;
		}

		~ofxBeckhoffADS ()
		{
			_isOpen = false;
			AdsPortCloseEx(_port);
			ofLog() << "ADS port closed" << endl;
		}

		void openLocal (unsigned short port)
		{
			_port = AdsPortOpenEx();

			if (!_isError(AdsGetLocalAddressEx(_port, &_addr)))
			{
				printf("%i.%i.%i.%i.%i.%i", _addr.netId.b[0], _addr.netId.b[1], _addr.netId.b[2], _addr.netId.b[3], _addr.netId.b[4], _addr.netId.b[5]);
				_addr.port = port;
				_isOpen = true;
				ofLog() << "connection succesfully opened at port " << _addr.port << endl;
			}
		}

		void openRemote(string address, unsigned short port)
		{
			vector<string> parts = ofSplitString(address, ".");

			if (parts.size() != 6)
			{
				ofLog() << "Invalid ADS address. Must consist of 6 parts" << endl;
				return;
			}

			for (int i = 0; i < 6; i++)
			{
				_addr.netId.b[i] = stoi(parts[i]);
			}

			_addr.port = port;
			_port = AdsPortOpenEx();
			
			printf("%i.%i.%i.%i.%i.%i", _addr.netId.b[0], _addr.netId.b[1], _addr.netId.b[2], _addr.netId.b[3], _addr.netId.b[4], _addr.netId.b[5]);
			
			_isOpen = true;
			ofLog() << "connection succesfully opened at port " << _addr.port << endl;
		}

		template<typename T>
		T readValue(string name, T defaultValue)
		{
			T value = defaultValue;
			unsigned long handle;

			if (_getHandle(name, &handle))
			{
				unsigned long pcbReturn;
				_isError(AdsSyncReadReqEx2(_port, &_addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value, &pcbReturn));
			}

			return value;
		}


		template<typename T>
		vector<T> readArray(string name, unsigned int length)
		{
			vector<T> vec(length);
			unsigned long handle;

			if (_getHandle(name, &handle))
			{
				unsigned long pcbReturn;
				_isError(AdsSyncReadReqEx2(_port, &_addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(T)*length, &vec[0], &pcbReturn));
			}

			return vec;
		}

		template<typename T>
		void writeValue(string name, T value)
		{
			unsigned long handle;

			if (_getHandle(name, &handle))
			{
				_isError(AdsSyncWriteReqEx(_port, &_addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value));
			}
		}

		template<typename T>
		void writeArray(string name, vector<T> values)
		{
			unsigned long handle;

			if (_getHandle(name, &handle))
			{
				_isError(AdsSyncWriteReqEx(_port, &_addr, ADSIGRP_SYM_VALBYHND, handle, values.size() * sizeof(T), &values[0]));
			}
		}

	private:

		AmsAddr _addr;
		long _port;
		bool _isOpen;

		bool _getHandle (string name, unsigned long *handle)
		{
			char *handleName = (char *)name.c_str();
			unsigned long pcbReturn;
			long err = AdsSyncReadWriteReqEx2(_port, &_addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(unsigned long), handle, name.size(), handleName, &pcbReturn);
			
			if (_isError(err)) return false;

			return true;
		}

		bool _isError(long errorCode)
		{
			if (errorCode)
			{
				string msg = _getErrorMsg(errorCode);

				ofLog(OF_LOG_ERROR) << "ADS error (" << errorCode << "): " << msg << endl;
				return true;
			}
			else
			{
				return false;
			}
		}

		string _getErrorMsg (unsigned long err)
		{
			string msg;

			switch(err)
			{
				case 1:
					msg = "Internal error";
					break;
				case 2:
					msg = "No Rtime";
					break;
				case 3:
					msg = "Allocation locked memory error";
					break;
				case 4:
					msg = "Insert mailbox error";
					break;
				case 5:
					msg = "Wrong receive HMSG";
					break;
				case 6:
					msg = "target port not found";
					break;
				case 7:
					msg = "target machine not found";
					break;
				case 8:
					msg = "Unknown command ID";
					break;
				case 9:
					msg = "Bad task ID";
					break;
				case 10:
					msg = "No IO";
					break;
				case 11:
					msg = "Unknown ADS command";
					break;
				case 12:
					msg = "Win 32 error";
					break;
				case 13:
					msg = "Port not connected";
					break;
				case 14:
					msg = "Invalid ADS length";
					break;
				case 15:
					msg = "Invalid ADS Net ID";
					break;
				case 16:
					msg = "Low Installation level";
					break;
				case 17:
					msg = "No debug available";
					break;
				case 18:
					msg = "Port disabled";
					break;
				case 19:
					msg = "Port already connected";
					break;
				case 20:
					msg = "ADS Sync Win32 error";
					break;
				case 21:
					msg = "ADS Sync Timeout";
					break;
				case 22:
					msg = "ADS Sync AMS error";
					break;
				case 23:
					msg = "ADS Sync no index map";
					break;
				case 24:
					msg = "Invalid ADS port";
					break;
				case 25:
					msg = "No memory";
					break;
				case 26:
					msg = "TCP send error";
					break;
				case 27:
					msg = "Host unreachable";
					break;
				case 1280:
					msg = "No locked memory can be allocated";
					break;
				case 1281:
					msg = "The size of the router memory could not be changed";
					break;
				case 1282:
					msg = "The mailbox has reached the maximum number of possible messages. The current sent message was rejected";
					break;
				case 1283:
					msg = "The mailbox has reached the maximum number of possible messages. The sent message will not be displayed in the debug monitor";
					break;
				case 1284:
					msg = "The port type is unknown";
					break;
				case 1285:
					msg = "Router is not initialised";
					break;
				case 1286:
					msg = "RThe desired port number is already assigned";
					break;
				case 1287:
					msg = "Port not registered";
					break;
				case 1288:
					msg = "The maximum number of Ports reached";
					break;
				case 1289:
					msg = "The port is invalid.";
					break;
				case 1290:
					msg = "TwinCAT Router not active";
					break;
				case 1291:
					msg = "ROUTERERR_FRAGMENTBOXFULL";
					break;
				case 1292:
					msg = "ROUTERERR_FRAGMENTTIMEOUT";
					break;
				case 1293:
					msg = "ROUTERERR_TOBEREMOVED";
					break;
				case 1792:
					msg = "error class <device error>";
					break;
				case 1793:
					msg = "Service is not supported by server";
					break;
				case 1794:
					msg = "invalid index group";
					break;
				case 1795:
					msg = "invalid index offset";
					break;
				case 1796:
					msg = "reading/writing not permitted";
					break;
				case 1797:
					msg = "parameter size not correct";
					break;
				case 1798:
					msg = "invalid parameter value(s)";
					break;
				case 1799:
					msg = "device is not in a ready state";
					break;
				case 1800:
					msg = "device is busy";
					break;
				case 1801:
					msg = "invalid context (must be in Windows)";
					break;
				case 1802:
					msg = "out of memory";
					break;
				case 1803:
					msg = "invalid parameter value(s)";
					break;
				case 1804:
					msg = "not found (files, ...)";
					break;
				case 1805:
					msg = "syntax error in command or file";
					break;
				case 1806:
					msg = "objects do not match";
					break;
				case 1807:
					msg = "object already exists";
					break;
				case 1808:
					msg = "symbol not found";
					break;
				case 1809:
					msg = "symbol version invalid";
					break;
				case 1810:
					msg = "server is in invalid state";
					break;
				case 1811:
					msg = "AdsTransMode not supported";
					break;
				case 1812:
					msg = "Notification handle is invalid";
					break;
				case 1813:
					msg = "Notification client not registered";
					break;
				case 1814:
					msg = "no more notification handles";
					break;
				case 1815:
					msg = "size for watch too big";
					break;
				case 1816:
					msg = "device not initialized";
					break;
				case 1817:
					msg = "device has a timeout";
					break;
				case 1818:
					msg = "query interface failed";
					break;
				case 1819:
					msg = "wrong interface required";
					break;
				case 1820:
					msg = "class ID is invalid";
					break;
				case 1821:
					msg = "object ID is invalid";
					break;
				case 1822:
					msg = "request is pending";
					break;
				case 1823:
					msg = "request is aborted";
					break;
				case 1824:
					msg = "signal warning";
					break;
				case 1825:
					msg = "invalid array index";
					break;
				case 1826:
					msg = "symbol not active";
					break;
				case 1827:
					msg = "access denied";
					break;
				case 1856:
					msg = "Error class <client error>";
					break;
				case 1857:
					msg = "invalid parameter at service";
					break;
				case 1858:
					msg = "polling list is empty";
					break;
				case 1859:
					msg = "var connection already in use";
					break;
				case 1860:
					msg = "invoke ID in use";
					break;
				case 1861:
					msg = "timeout elapsed";
					break;
				case 1862:
					msg = "error in win32 subsystem";
					break;
				case 1863:
					msg = "Invalid client timeout value";
					break;
				case 1864:
					msg = "ads-port not opened";
					break;
				case 1872:
					msg = "internal error in ads sync";
					break;
				case 1873:
					msg = "hash table overflow";
					break;
				case 1874:
					msg = "key not found in hash";
					break;
				case 1875:
					msg = "no more symbols in cache";
					break;
				case 1876:
					msg = "invalid response received";
					break;
				case 1877:
					msg = "sync port is locked";
					break;
				case 4096:
					msg = "Internal fatal error in the TwinCAT real-time system";
					break;
				case 4097:
					msg = "Timer value not vaild";
					break;
				case 4098:
					msg = "Task pointer has the invalid value ZERO";
					break;
				case 4099:
					msg = "Task stack pointer has the invalid value ZERO";
					break;
				case 4100:
					msg = "The demand task priority is already assigned";
					break;
				case 4101:
					msg = "No more free TCB (Task Control Block) available. Maximum number of TCBs is 64";
					break;
				case 4102:
					msg = "No more free semaphores available. Maximum number of semaphores is 64";
					break;
				case 4103:
					msg = "No more free queue available. Maximum number of queue is 64";
					break;
				case 4104:
					msg = "TwinCAT reserved.";
					break;
				case 4105:
					msg = "TwinCAT reserved.";
					break;
				case 4106:
					msg = "TwinCAT reserved.";
					break;
				case 4107:
					msg = "TwinCAT reserved.";
					break;
				case 4108:
					msg = "TwinCAT reserved.";
					break;
				case 4109:
					msg = "An external synchronisation interrupt is already applied";
					break;
				case 4110:
					msg = "No external synchronsiation interrupt applied";
					break;
				case 4111:
					msg = "The apply of the external synchronisation interrupt failed";
					break;
				case 4112:
					msg = "Call of a service function in the wrong context";
					break;
				case 4119:
					msg = "Intel VT-x extension is not supported.";
					break;
				case 4120:
					msg = "Intel VT-x extension is not enabled in BIOS.";
					break;
				case 4121:
					msg = "Missing feature in Intel VT-x extension.";
					break;
				case 4122:
					msg = "E nabling Intel VT-x fails.";
					break;
				case 10060:
					msg = "A socket operation was attempted to an unreachable host	Host unreachable";
					break;
				case 10061:
					msg = "A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond.";
					break;
				case 10065:
					msg = "No connection could be made because the target machine actively refused it";
					break;
			}

			return msg;
		}
};