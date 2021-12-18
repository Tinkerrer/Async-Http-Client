// Copyright 2021 Tinkerrer
#include <AsyncHttpServer.h>

int main(int argc, char* argv[]) {
  try {
    // Check command line arguments.
    // ./demo <address> <port>
    // command for making request from terminal:
    // curl -X POST http://localhost:5050/v1/api -H "Content-Type:
    // application/json" -d {\"input\":\"hel\"}
    if (argc != 3) {
      std::cerr << "Usage: http_server <address> <port>\n";
      return 1;
    }

    // Initialise the server.
    auto s = std::make_shared<AsyncHttpServer>(argv[1], argv[2]);

    // Run the server until stopped.
    s->run();
  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }
}
