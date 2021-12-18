// Copyright 2021 Tinkerrer

#ifndef TEMPLATE_ROUTER_H
#define TEMPLATE_ROUTER_H

#include <algorithm>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;    // from <boost/beast.hpp>
namespace http = beast::http;      // from <boost/beast/http.hpp>
namespace net = boost::asio;       // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>

template <class Handler>
class Router {
 public:
  void AddHandler(std::string method, Handler handler) {
    m_handlers.emplace(method, handler);
  }

  http::response<http::string_body> processRoute(
      const std::string& method,
      const http::request<http::string_body>& request) {
    auto handler_it = m_handlers.find(method);
    if (handler_it != m_handlers.end()) {
      return (handler_it->second)(request);
    }
    http::response<http::string_body> res{http::status::bad_request,
                                          request.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.result(http::status::not_found);
    res.body() = "Server can't process this type of request";
    res.prepare_payload();
    return res;
  }

 private:
  std::map<std::string, Handler> m_handlers;
};

#endif  // TEMPLATE_ROUTER_H
