#include "JCNetworking.h"

#include "SockLib/SockLib.h"
#include "Error/Error.h"
#include "JCNContext/JCNContext.h"

namespace JCN_NAMESPACE
{
	JCNContext* start_jcn(const jcn_settings& _settings)
	{
		JCNContext* _out = nullptr;
		auto _result = slib::start_socklib(_settings.vmajor, _settings.vminor);
		slib::check_result(_out, _result);

		_out = new JCNContext{};
		_out->settings = _settings;

		return _out;
	};
	void shutdown_jcn(JCNContext*& _context)
	{
		delete _context;
		_context = nullptr;
		slib::shutdown_socklib();
	};

};

namespace JCN_NAMESPACE
{
	std::string host_name()
	{
		std::string _out{};
		_out.resize(64);
		auto _result = slib::gethostname(_out.data(), (int)_out.size());
		slib::check_result(_result);
		return _out;
	};
};




