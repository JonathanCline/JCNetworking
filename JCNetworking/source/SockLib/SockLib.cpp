#include "SockLib.h"

#include <SAELib_Singleton.h>

namespace JCN_NAMESPACE::slib
{

#ifdef JCN_WINSOCK

	namespace
	{
		struct WinsockData
		{
			bool started = false;
			WSAData wsa{};
		};
		struct WinsockDataTag {};

		WinsockData& get_winsock_data()
		{
			return sae::get_singleton<WinsockData, WinsockDataTag>();
		};

		bool start_socklib_os(int _vmajor, int _vminor)
		{
			auto& _wdata = sae::get_singleton<WinsockData, WinsockDataTag>();
			if (!_wdata.started)
			{
				auto _vword = MAKEWORD(_vmajor, _vminor);
				auto& _wsaData = _wdata.wsa;
				auto _result = WSAStartup(_vword, &_wsaData);
				_wdata.started = (_result != SOCKET_ERROR);
			};
			return _wdata.started;
		};
		void shutdown_socklib_os()
		{
			auto& _wdata = sae::get_singleton<WinsockData, WinsockDataTag>();
			if (_wdata.started)
			{
				auto _result = WSACleanup();
				_wdata.started = false;
			};
		};
	};

#endif


	bool start_socklib(int _vmajor, int _vminor)
	{
		return start_socklib_os(_vmajor, _vminor);
	};


	void shutdown_socklib()
	{
		shutdown_socklib_os();
	};


};
