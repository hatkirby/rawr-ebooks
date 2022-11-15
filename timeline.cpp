#include "timeline.h"
#include <sstream>
#include <hkutil/string.h>
#include <iostream>

timeline::timeline(mastodonpp::API::endpoint_type endpoint) : endpoint_(endpoint)
{
}

std::list<nlohmann::json> timeline::poll(mastodonpp::Connection& connection)
{
  std::string maxId;
  std::list<nlohmann::json> result;

  for (int i = 0; i < 5; i++)
  {
    mastodonpp::parametermap arguments;

    if (i > 0)
    {
      arguments["max_id"] = maxId;
    }

    if (hasSince_)
    {
      arguments["since_id"] = sinceId_;
    }

    auto answer{connection.get(endpoint_, arguments)};
    if (!answer)
    {
      if (answer.curl_error_code == 0)
      {
        std::cout << "HTTP status: " << answer.http_status << std::endl;
      }
      else
      {
        std::cout << "libcurl error " << std::to_string(answer.curl_error_code)
             << ": " << answer.error_message << std::endl;
      }
      return {};
    }

    nlohmann::json rjs = nlohmann::json::parse(answer.body);

    if (rjs.empty())
    {
      break;
    }

    for (auto& single : rjs)
    {
      result.push_back(single);
    }

    maxId = result.back()["id"].get<std::string>();
  }

  if (!result.empty())
  {
    sinceId_ = result.front()["id"].get<std::string>();
    hasSince_ = true;
  }

  return result;
}
