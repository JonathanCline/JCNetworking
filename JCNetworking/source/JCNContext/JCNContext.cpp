#include "JCNContext.h"

#include "SockLib/BKExposed.h"

#include <SAELib_Singleton.h>

namespace JCN_NAMESPACE
{

	void JCNContext::lock()
	{
		while (this->rflag_.test_and_set()) { /* consider adding a wait */ };
	};
	void JCNContext::unlock()
	{
		JCN_ASSERT(this->rflag_.test());
		this->rflag_.clear();
	};

	struct JCNGlobalContextTag {};
	JCNContext& global_context()
	{
		return sae::get_singleton<JCNContext, JCNGlobalContextTag>();
	};



	struct JCNCurrentContextTag {};
	JCNContext*& current_context()
	{
		return sae::get_singleton_thread_local<JCNContext*, JCNCurrentContextTag>();
	};

	JCNContext* get_current_context()
	{
		return current_context();
	};
	void make_context_current(JCNContext* _context)
	{
		current_context() = _context;
	};

	JCNContext& get_best_context()
	{
		auto _out = get_current_context();
		if (!_out)
		{
			_out = &global_context();
		};
		JCN_ASSERT(_out != nullptr);
		return *_out;
	};



};
