// Copyright 2021 Tinkerrer

#include "Connection.h"

#include <boost/beast/http/write.hpp>
#include <boost/bind/bind.hpp>
#include <vector>

Connection::Connection(
    Router<http::response<http::string_body> (*)(
        const http::request<http::string_body>& request)>& router,
    tcp::socket&& socket)
    : request_router_(router), stream_(std::move(socket)) {}

void Connection::start() {
  net::dispatch(
      stream_.get_executor(),
      beast::bind_front_handler(&Connection::do_read, shared_from_this()));
}

void Connection::do_read() {
  request_ = {};

  // Read a request
  http::async_read(
      stream_, buffer_, request_,
      beast::bind_front_handler(&Connection::handle_read, shared_from_this()));
}

void Connection::handle_read(beast::error_code e,
                             [[maybe_unused]] std::size_t bytes_transferred) {
  if (e == http::error::end_of_stream) return do_close();

  if (!e) {
    http::response<http::string_body> res = request_router_.processRoute(
        request_.method_string().to_string(), request_);

    auto sp = std::make_shared<http::message<false, http::string_body>>(
        std::move(res));
    res_ = sp;

    http::async_write(
        stream_, *sp,
        beast::bind_front_handler(&Connection::handle_write, shared_from_this(),
                                  sp->need_eof()));
  }
}

void Connection::handle_write(bool close, beast::error_code e,
                              [[maybe_unused]] std::size_t bytes_transferred) {
  if (close) {
    // This means we should close the connection, usually because
    // the response indicated the "Connection: close" semantic.
    return do_close();
  }

  if (!e) {
    do_read();
  }
}

void Connection::do_close() {
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}
