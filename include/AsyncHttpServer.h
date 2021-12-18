// Copyright 2021 Tinkerrer

#ifndef TEMPLATE_ASYNCHTTPSERVER_H
#define TEMPLATE_ASYNCHTTPSERVER_H

#include <Connection.h>

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;    // from <boost/beast.hpp>
namespace http = beast::http;      // from <boost/beast/http.hpp>
namespace net = boost::asio;       // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>

class AsyncHttpServer : public std::enable_shared_from_this<AsyncHttpServer> {
 public:
  explicit AsyncHttpServer(const std::string& address, const std::string& port);

  /// Run the server's io_context loop.
  void run();

 private:
  /// Initiate an asynchronous accept operation.
  void start_accept();

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(beast::error_code e, tcp::socket socket);

  /// Handle a request to stop the server.
  void handle_stop();

 private:
  /// The handler for all incoming requests.
  Router<http::response<http::string_body> (*)(
      const http::request<http::string_body>& request)>
      request_router_;

  net::io_context io_context_;
  tcp::acceptor acceptor_;
};

#endif  // TEMPLATE_ASYNCHTTPSERVER_H
