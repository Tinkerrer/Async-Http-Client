// Copyright 2021 Tinkerrer

#include "AsyncHttpClient.h"

AsyncHttpClient::AsyncHttpClient(boost::asio::io_context& io_context)
    : resolver_(net::make_strand(io_context)),
      stream_(net::make_strand(io_context)),
      signals_(io_context, SIGINT, SIGTERM) {}

void AsyncHttpClient::run(const std::string& server, const std::string& port,
                          const std::string& path) {
  // SIGINT or SEGTERM closes the socket
  signals_.async_wait(boost::bind(&AsyncHttpClient::handle_stop, this));

  request_.version(11);
  request_.method(http::verb::post);
  request_.target(path);
  request_.set(http::field::host, server);
  request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  request_.set(http::field::content_type, "application/json");

  resolver_.async_resolve(
      server, port,
      beast::bind_front_handler(&AsyncHttpClient::handle_resolve,
                                shared_from_this()));
}

void AsyncHttpClient::handle_resolve(
    beast::error_code err, const tcp::resolver::results_type& results) {
  if (!err) {
    stream_.async_connect(
        results, beast::bind_front_handler(&AsyncHttpClient::handle_connect,
                                           shared_from_this()));
  } else {
    std::cout << "Error: " << err.message() << "\n";
  }
}

void AsyncHttpClient::handle_connect(
    beast::error_code err, const tcp::resolver::results_type::endpoint_type&) {
  if (!err) {
    enter_parameter();
  } else {
    std::cout << "Error: " << err.message() << "\n";
  }
}

void AsyncHttpClient::enter_parameter() {
  // entering json request_body a.k. parameter
  std::cout << "\n\nEnter parameter in json\nExample: {\n"
               "  \"input\": \"hel\"\n"
               "}\n";
  std::getline(std::cin, request_.body());
  request_.prepare_payload();

  http::async_write(
      stream_, request_,
      beast::bind_front_handler(&AsyncHttpClient::handle_write_request,
                                shared_from_this()));
}

void AsyncHttpClient::handle_write_request(
    beast::error_code err, [[maybe_unused]] std::size_t bytes_transferred) {
  if (!err) {
    response_.body() = "";
    http::async_read(stream_, buffer_, response_,
                     beast::bind_front_handler(&AsyncHttpClient::handle_read,
                                               shared_from_this()));
  } else {
    std::cout << "Error: " << err.message() << "\n";
  }
}

void AsyncHttpClient::handle_read(beast::error_code err,
                                  std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (!err) {
    // Write the message to standard out
    std::cout
        << "\n===========================================================\n"
        << response_
        << "\n===========================================================\n";
    enter_parameter();
  } else {
    std::cout << "Error: " << err << "\n";
  }
}

void AsyncHttpClient::handle_stop() {
  // Gracefully close the socket
  stream_.socket().shutdown(tcp::socket::shutdown_both);
  std::cout << "\nsocket shutdown\n";
  exit(0);
}
