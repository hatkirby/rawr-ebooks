#ifndef TIMELINE_H_FE90F0DC
#define TIMELINE_H_FE90F0DC

#include <functional>
#include <list>
#include <string>
#include <mastodonpp/mastodonpp.hpp>
#include <json.hpp>

class timeline {
public:

  explicit timeline(mastodonpp::API::endpoint_type endpoint);

  std::list<nlohmann::json> poll(mastodonpp::Connection& connection);

private:

  mastodonpp::API::endpoint_type endpoint_;
  bool hasSince_ = false;
  std::string sinceId_;
};

#endif /* end of include guard: TIMELINE_H_FE90F0DC */
