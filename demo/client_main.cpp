// Copyright 2021 Tinkerrer

#include <AsyncHttpClient.h>

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 4)
    {
      std::cout << "Usage: async_client <server> <port> <path>\n";
      //./demo localhost 5050 "/v1/api/suggest"
      return 1;
    }

    net::io_context io_context;

    std::make_shared<AsyncHttpClient>(io_context)->run(argv[1], argv[2], argv[3]);

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cout << "Exception: " << e.what() << "\n";
  }

  return 0;
}
