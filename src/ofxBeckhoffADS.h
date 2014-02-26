#pragma once
#include "ofMain.h"
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

class ofxBeckhoffADS
{
	public:

		ofxBeckhoffADS ()
		{
			long nTemp = AdsGetDllVersion();
			AdsVersion *pDLLVersion = (AdsVersion *)&nTemp;
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
		T readValue(string name)
		{
			T value;
			unsigned long handle;

			if (_getHandle(name, &handle))
			{
				unsigned long pcbReturn;
				AdsSyncReadReqEx2(_port, &_addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value, &pcbReturn);
			}

			return value;
		}

		template<typename T>
		void writeValue(string name, T value)
		{
			unsigned long handle;

			if (_getHandle(name, &handle))
			{
				AdsSyncWriteReqEx(_port, &_addr, ADSIGRP_SYM_VALBYHND, handle, sizeof(value), &value);
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
				ofLog(OF_LOG_ERROR) << errorCode << endl;
				return true;
			}
			else
			{
				return false;
			}
		}

};