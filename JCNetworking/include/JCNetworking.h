#pragma once
#ifndef JC_NETWORKING_H
#define JC_NETWORKING_H

#include "JCNetworkingConfig.h"

#include <string_view>

namespace JCN_NAMESPACE
{
	enum result : bool
	{
		rError = false,
		rGood = true
	};

	struct jcn_settings
	{
		int vmajor = 0;
		int vminor = 0;
	};

	result start_jcn(const jcn_settings& _settings);
	void shutdown_jcn();
	


	struct Error
	{
		int code;
		std::string_view function;
	};

	//using error_callback = void(*)(Error);
	//void set_error_callback(error_callback _cb);


	
	void throw_error(Error _error);



};

#endif