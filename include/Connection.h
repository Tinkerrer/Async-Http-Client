// Copyright 2021 Tinkerrer

#ifndef TEMPLATE_CONNECTION_H
#define TEMPLATE_CONNECTION_H

#include <Router.h>

#include <algorithm>
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

class Connection : public std::enable_shared_from_this<Connection> {
 public:
  /// Construct a connection with the given io_context.
  explicit Connection(
      Router<http::response<http::string_body> (*)(
          const http::request<http::string_body> &request)> &router,
      tcp::socket &&socket);

  /// Start the first asynchronous operation for the connection.
  void start();

 private:
  void do_read();

  /// Handle completion of a read operation.
  void handle_read(beast::error_code e, std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(bool close, beast::error_code e,
                    std::size_t bytes_transferred);

  void do_close();

 private:
  // The handler used to process the incoming request.
  Router<http::response<http::string_body> (*)(
      const http::request<http::string_body> &request)> &request_router_;

  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;

  http::request<http::string_body> request_;
  std::shared_ptr<void> res_;
};

#endif  // TEMPLATE_CONNECTION_H
