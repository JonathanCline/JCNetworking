#include "SockLib.h"

#include "Error/Error.h"

#include <SAELib_Singleton.h>

#include <cerrno>

namespace JCN_NAMESPACE::slib
{
#ifdef JCN_WINSOCK
	namespace
	{
		struct WinsockData
		{
			int startcount = 0;
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
			if (_wdata.startcount == 0)
			{
				auto _vword = MAKEWORD(_vmajor, _vminor);
				auto& _wsaData = _wdata.wsa;
				auto _result = WSAStartup(_vword, &_wsaData);
				if (!_result)
				{
					_wdata.startcount = -1;
				};
			};
			++_wdata.startcount;
			return (_wdata.startcount != 0);
		};
		void shutdown_socklib_os()
		{
			auto& _wdata = sae::get_singleton<WinsockData, WinsockDataTag>();
			if (_wdata.startcount != 0)
			{
				--_wdata.startcount;
			};
			if (_wdata.startcount == 0)
			{
				auto _result = WSACleanup();
			};
		};
	};
#else
	namespace
	{
		bool start_socklib_os(int _vmajor, int _vminor)
		{
			// no need for unix sockets
			return true;
		};
		void shutdown_socklib_os()
		{
			// no need for unix sockets
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


#ifdef JCN_WINSOCK
	namespace
	{
		int get_last_error_os()
		{
			return WSAGetLastError();
		};
	};
#elif defined(JCN_UNIX)
	namespace
	{
		int get_last_error_os()
		{
			return errno;
		};
	};
#endif

	int get_last_error()
	{
		return get_last_error_os();
	};






#ifdef JCN_WINSOCK
	namespace
	{
		bool bad_result_os()
		{
			return SOCKET_ERROR;
		};
	};
#elif defined(JCN_UNIX)
	namespace
	{
		bool bad_result_os()
		{
			return -1;
		};
	};
#endif

	int bad_result()
	{
		return bad_result_os();
	};



	bool is_result_good(int _result)
	{
		return (_result != bad_result());
	};


#ifdef JCN_WINSOCK
	namespace
	{
		socket_t invalid_socket_os()
		{
			return INVALID_SOCKET;
		};
	};
#elif defined(JCN_UNIX)
	namespace
	{
		socket_t invalid_socket_os()
		{
			return -1;
		};
	};
#endif

	socket_t invalid_socket()
	{ 
		return invalid_socket_os();
	};

	namespace
	{
		Error make_error(const std::string_view& _location)
		{
			const auto _ecode = get_last_error();
			return Error{ _ecode, _location };
		};

		void throw_error(const std::string_view& _location)
		{
			throw_error(make_error(_location));
		};
		void throw_error(const std::string_view& _location, JCNContext* _context)
		{
			throw_error(_context, make_error(_location));
		};
	};



	bool check_result(JCNContext* _context, int _result, const std::string_view& _location)
	{
		const auto _good = is_result_good(_result);
		if (!_good) [[unlikely]]
		{
			throw_error(_location, _context);
		};
		return _good;
	};
	bool check_result(JCNContext* _context, int _result)
	{
		return check_result(_context, _result, "");
	};

	bool check_result(int _result, const std::string_view& _location)
	{
		const auto _good = is_result_good(_result);
		if (!_good) [[unlikely]]
		{
			throw_error(_location);
		};
		return _good;
	};
	bool check_result(int _result)
	{
		return check_result(_result, "");
	};





};
