#include "JCNetworking.h"

#include "SockLib/SockLib.h"

namespace JCN_NAMESPACE
{
	result start_jcn(const jcn_settings& _settings)
	{
		auto _result = (result)slib::start_socklib(_settings.vmajor, _settings.vminor);
		return _result;
	};
	void shutdown_jcn()
	{
		slib::shutdown_socklib();
	};




	void throw_error(Error _error)
	{



	};


};
