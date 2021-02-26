#pragma once

#include "JCNetworkingBK.h"
#include "JCNetworkingSockLib.h"

namespace JCN_NAMESPACE::bk
{
	namespace impl
	{
		struct SocketUnwrapper
		{
			static SocketAdapter::socket_type unwrap(const socket& _s);
			static socket wrap(const SocketAdapter::socket_type& _s);

			static SocketAdapter::socket_type unwrap_ref(const socketref& _s);
			static socketref wrap_ref(const SocketAdapter::socket_type& _s);

		};
	};

	static auto unwrap_socket(const socket& _s)
	{
		return impl::SocketUnwrapper::unwrap(_s);
	};
	static auto wrap_socket(const slib::socket_t& _s)
	{
		return impl::SocketUnwrapper::wrap(_s);
	};

	static auto unwrap_socket_ref(const socketref& _s)
	{
		return impl::SocketUnwrapper::unwrap_ref(_s);
	};
	static auto wrap_socket_ref(const slib::socket_t& _s)
	{
		return impl::SocketUnwrapper::wrap_ref(_s);
	};

};
