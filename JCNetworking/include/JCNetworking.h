#pragma once
#ifndef JC_NETWORKING_H
#define JC_NETWORKING_H

#include "JCNetworkingConfig.h"

#include <string>
#include <string_view>

namespace JCN_NAMESPACE
{

	struct jcn_settings
	{
		int vmajor = 0;
		int vminor = 0;
	};

	struct JCNContext;

	JCNContext* start_jcn(const jcn_settings& _settings);
	void shutdown_jcn(JCNContext*& _context);
	
	JCNContext* get_current_context();
	void make_context_current(JCNContext* _context);




	struct Error
	{
		constexpr explicit operator bool() const noexcept { return !this->code; };
		int code;
		std::string_view function;
	};

	using ErrorCallback = bool(*)(JCNContext*, Error);



	void set_error_callback(JCNContext* _context, ErrorCallback _cb);
	void set_error_callback(ErrorCallback _cb);

	Error get_last_error(JCNContext* _context);
	Error get_last_error();
	
	void throw_error(JCNContext* _context, Error _error);
	void throw_error(Error _error);

};

namespace JCN_NAMESPACE
{

	std::string host_name();





	
	




};

#endif