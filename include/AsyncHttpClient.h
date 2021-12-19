// Copyright 2021 Tinkerrer

#ifndef TEMPLATE_ASYNCHTTPCLIENT_H
#define TEMPLATE_ASYNCHTTPCLIENT_H

#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <string>

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;       // from <boost/beast.hpp>
namespace http = boost::beast::http;  // from <boost/beast/http.hpp>
namespace net = boost::asio;          // from <boost/asio.hpp>

class AsyncHttpClient : public std::enable_shared_from_this<AsyncHttpClient> {
 public:
  explicit AsyncHttpClient(boost::asio::io_context& io_context);

  void run(const std::string& server, const std::string& port,
           const std::string& path);

 private:
  void handle_resolve(beast::error_code err,
                      const tcp::resolver::results_type& results);

  void handle_connect(beast::error_code err,
                      const tcp::resolver::results_type::endpoint_type&);

  void enter_parameter();

  void handle_write_request(beast::error_code err,
                            std::size_t bytes_transferred);

  void handle_read(beast::error_code err, std::size_t bytes_transferred);

  void handle_stop();

 private:
  tcp::resolver resolver_;

  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;  // (Must persist between reads)

  http::request<http::string_body> request_;
  http::response<http::string_body> response_;

  net::signal_set signals_;
};

#endif  // TEMPLATE_ASYNCHTTPCLIENT_H
