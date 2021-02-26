#pragma once
#ifndef JC_NETWORKING_H
#define JC_NETWORKING_H

#include "JCNetworkingConfig.h"
#include "JCNetworkingBk.h"

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

	using bk::protocol;
	using bk::port_t;





	class RemoteConnection
	{
	public:
		const auto& sendbuf() const noexcept { return this->send_buff_; };
		void set_sendbuf(const bk::packetbuff& _buf) { this->send_buff_ = _buf; };

		const auto& recvbuf() const noexcept { return this->recv_buff_; };
		void set_recvbuf(const bk::packetbuff& _buf) { this->recv_buff_ = _buf; };

		const auto& socket() const noexcept { return this->sock_; };

		bool good() const noexcept { return this->socket().good(); };
		explicit operator bool() const noexcept { return this->good(); };

		explicit RemoteConnection(bk::socket _sock, bk::packetbuff _sendbuff, bk::packetbuff _recvbuff) : 
			sock_{ std::move(_sock) }, send_buff_{ _sendbuff }, recv_buff_{ _recvbuff }
		{};

		RemoteConnection(const std::string_view& _address, const std::string_view& _pService, protocol::tcp_t) : 
			sock_{ bk::connect( _address.data(), _pService.data(), protocol::tcp) }
		{};
		RemoteConnection(const std::string_view& _address, const std::string_view& _pService, protocol::udp_t) :
			sock_{ bk::connect(_address.data(), _pService.data(), protocol::udp) }
		{};

	private:
		bk::socket sock_{};
		bk::packetbuff send_buff_{};
		bk::packetbuff recv_buff_{};

	};


};

#endif