#ifndef RIDE_H
#define RIDE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Ride{
        private:
            long _ride_id;
            long _route_id;
            long _creator_id;
            long _ride_time;
            std::string _ride_passengers;

        public:
            static Ride fromJSON(const std::string & str);

            long               get_ride_id() const;
            long               get_route_id() const;
            long               get_creator_id() const;
            long               get_ride_time() const;
            const std::string  &get_ride_passengers() const;

            long&        ride_id();
            long&        route_id();
            long&        creator_id();
            long&        ride_time();
            std::string &ride_passengers();

            static void init();
            void add_passenger(long ride_id, long user_id);
            static std::optional<Ride> read_ride_by_id(long ride_id);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif