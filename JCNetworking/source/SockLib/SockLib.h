#pragma once

#include "JCNetworkingSockLib.h"

#include <string_view>


namespace JCN_NAMESPACE
{
	struct JCNContext;
};

namespace JCN_NAMESPACE::slib
{
	bool start_socklib(int _vmajor, int _vminor);
	void shutdown_socklib();

	int bad_result();

	int get_last_error();
	bool is_result_good(int _result);

	bool check_result(JCNContext* _context, int _result, const std::string_view& _location);
	bool check_result(JCNContext* _context, int _result);

	bool check_result(int _result, const std::string_view& _location);
	bool check_result(int _result);

	socket_t invalid_socket();

};

