#pragma once

#include "JCNetworkingSockLib.h"

namespace JCN_NAMESPACE::slib
{
	bool start_socklib(int _vmajor, int _vminor);
	void shutdown_socklib();
};

