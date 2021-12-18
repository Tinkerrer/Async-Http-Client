// Copyright 2021 Tinkerrer
#include "AsyncHttpServer.h"

#include <boost/bind/bind.hpp>
#include <thread>
#include <vector>

// Handlers======================================================================
using json = nlohmann::json;

http::response<http::string_body> HandlerGetPost(
    const http::request<http::string_body>& request) {
  //создается ответ, устанавливаются первичные параматры, изначально параметры
  //устанавливаются на bad_request, однако если приложение выполнится, то статус
  //ответа изменится
  http::response<http::string_body> response;
  response.result(http::status::not_found);
  response.body() = "Target was not found";
  response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  response.set(http::field::content_type, "application/json");
  response.version(request.version());

  //обработка тела запроса
  if (request.target() == "/v1/api/suggest") {
    json user_input_value;
    try {
      user_input_value = json::parse(request.body());
    } catch (json::exception&) {
      response.result(http::status::bad_request);
      response.body() = "Invalid request body";
      response.prepare_payload();
      return response;
    }

    //чтение файла в json
    std::string path = std::filesystem::current_path().parent_path().string() +
                       "/suggest_json/suggestions.json";
    std::ifstream file{path};
    if (!file) {
      response.result(http::status::service_unavailable);
      response.body() = "Server didn't find json_file";
      response.prepare_payload();
      return response;
    }
    json suggestions = json::parse(file);

    //удаляем неподходящие записи, добавляя их в другой json
    json processed_json = json::array();
    for (auto iter = suggestions.begin(); iter < suggestions.end(); ++iter) {
      if (iter->at("id") == user_input_value.at("input")) {
        processed_json.push_back(
            {{"text", iter->at("name")}, {"cost", iter->at("cost")}});
      }
    }

    //сортируем записи по cost
    std::sort(processed_json.begin(), processed_json.end(),
              [](const auto& left, const auto& right) {
                return left.at("cost") < right.at("cost");
              });

    //формируем ответ
    response.result(http::status::ok);
    response.body() = processed_json.dump(1);
    response.prepare_payload();
  }
  return response;
}

// Server========================================================================

AsyncHttpServer::AsyncHttpServer(const std::string& address,
                                 const std::string& port)
    : acceptor_(net::make_strand(io_context_)) {
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_context_);
  boost::asio::ip::tcp::endpoint endpoint =
      *resolver.resolve(address, port).begin();

  // Adding handlers for processing different types of requests
  request_router_.AddHandler("POST", HandlerGetPost);

  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(net::socket_base::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
}

void AsyncHttpServer::run() {
  start_accept();

  io_context_.run();
}

void AsyncHttpServer::start_accept() {
  acceptor_.async_accept(
      net::make_strand(io_context_),
      beast::bind_front_handler(&AsyncHttpServer::handle_accept,
                                shared_from_this()));
}

void AsyncHttpServer::handle_accept(beast::error_code e, tcp::socket socket) {
  if (!e) {
    std::cout << "accept!";

    auto c = std::make_shared<Connection>(request_router_, std::move(socket));
    c->start();
  }

  start_accept();
}

void AsyncHttpServer::handle_stop() { io_context_.stop(); }
