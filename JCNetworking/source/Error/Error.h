#pragma once

#include "JCNetworking.h"


#include <string_view>

namespace JCN_NAMESPACE
{
	void throw_error(JCNContext* _context, Error _error);
	void throw_error(Error _error);

	void set_error_callback(JCNContext* _context, ErrorCallback _cb);
	void set_error_callback(ErrorCallback _cb);

	Error get_last_error(JCNContext* _context);
	Error get_last_error();

};
