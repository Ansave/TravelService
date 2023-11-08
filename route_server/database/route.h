#ifndef ROUTE_H
#define ROUTE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Route{
        private:
            long _route_id;
            long _creator_id;
            std::string _route_title;
            std::string _route_description;
            std::string _route_start;
            std::string _route_end;
        public:
            static Route fromJSON(const std::string & str);
            long get_route_id() const;
            long get_creator_id() const;
            
            const std::string& get_route_title() const;
            const std::string& get_route_description() const;
            const std::string& get_route_start() const;
            const std::string& get_route_end() const;

            long& route_id();
            long& creator_id();
            std::string& route_title();
            std::string& route_description();
            std::string& route_start();
            std::string& route_end();

            static void init();
            static std::vector<Route> read_all_routes_by_user_id(long user_id);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif