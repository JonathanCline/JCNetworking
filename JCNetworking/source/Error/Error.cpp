#include "Error.h"

#include "JCNContext/JCNContext.h"

namespace JCN_NAMESPACE
{
	
	namespace
	{
		auto& context_error_callback(JCNContext* _context)
		{
			return _context->error_callback;
		};
		auto& context_error_queue(JCNContext* _context)
		{
			return _context->error_queue;
		};
	};



	void throw_error(JCNContext* _context, Error _error)
	{
		auto& _callback = _context->error_callback;
		if (_callback)
		{
			std::invoke(_callback, _context, _error);
		}
		else
		{
			_context->error_queue.push(std::move(_error));
		};
	};
	void throw_error(Error _error) 
	{ 
		throw_error(&get_best_context(), _error);
	};

	void set_error_callback(JCNContext* _context, ErrorCallback _cb)
	{
		auto& _contextCB = context_error_callback(_context);
		_contextCB = _cb;
	};
	void set_error_callback(ErrorCallback _cb)
	{
		set_error_callback(&global_context(), _cb);
	};

	Error get_last_error(JCNContext* _context)
	{
		Error _out{};
		auto& _queue = context_error_queue(_context);

		if (!_queue.empty()) 
		{
			_out = _queue.front();
			_queue.pop();
		};

		return _out;
	};
	Error get_last_error()
	{
		return get_last_error(&global_context());
	};

}