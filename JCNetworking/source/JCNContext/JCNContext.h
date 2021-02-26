#pragma once

#include "JCNetworking.h"
#include "JCNetworkingBk.h"

#include "Socket/SockSet.h"
#include "SockLib/SockLib.h"



#include <atomic>
#include <queue>
#include <vector>

namespace JCN_NAMESPACE
{
	struct JCNContext
	{
	public:
		void lock();
		void unlock();
		
		jcn_settings settings{};

		ErrorCallback error_callback = nullptr;
		std::queue<Error> error_queue{};

	private:
		mutable std::atomic_flag rflag_ = ATOMIC_FLAG_INIT;

	};

	JCNContext& global_context();

	JCNContext* get_current_context();
	void make_context_current(JCNContext* _context);

	JCNContext& get_best_context();

};

