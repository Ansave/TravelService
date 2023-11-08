#include "ride.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>
#include <algorithm>
#include <future>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
namespace database
{
    void Ride::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Ride` (`ride_id` INT NOT NULL AUTO_INCREMENT,"
                        << "`route_id` INT NOT NULL,"
                        << "`creator_id` INT NOT NULL,"
                        << "`ride_time` INT NOT NULL,"
                        << "`ride_passengers` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                        << "PRIMARY KEY (ride_id));",
                        now;

             std::cout << create_stmt.toString() << std::endl;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }


    Poco::JSON::Object::Ptr Ride::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("ride_id", _ride_id);
        root->set("route_id", _route_id);
        root->set("creator_id", _creator_id);
        root->set("ride_time", _ride_time);
        root->set("ride_passengers", _ride_passengers);
        return root;
    }

    Ride Ride::fromJSON(const std::string &str)
    {
        Ride ride;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        ride.ride_id() = object->getValue<long>("ride_id");
        ride.route_id() = object->getValue<long>("route_id");
        ride.creator_id() = object->getValue<long>("creator_id");
        ride.ride_time() = object->getValue<long>("ride_time");
        ride.ride_passengers() = object->getValue<std::string>("ride_passengers");

        return ride;
    }

    std::optional<Ride> Ride::read_ride_by_id(long ride_id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Ride a;
            select << "SELECT ride_id, route_id, creator_id, ride_time, ride_passengers FROM Ride WHERE ride_id=?",
                into(a._ride_id),
                into(a._route_id),
                into(a._creator_id),
                into(a._ride_time),
                into(a._ride_passengers),
                use(ride_id),
                range(0, 1); //  iterate over result set one row at a time
            
            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    void Ride::add_passenger(long ride_id, long user_id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::string passengers;
            select << "SELECT ride_passengers FROM Ride WHERE ride_id=?",
                into(passengers),
                use(ride_id),
                range(0, 1);
            select.execute();
            // std::cout << passengers;
            passengers += std::to_string(user_id) + ";";
            // std::cout << passengers;
            // return;
            Poco::Data::Statement update(session);
            update << "UPDATE Ride SET ride_passengers=? WHERE ride_id=?",
                use(passengers),
                use(ride_id),
                range(0, 1); //  iterate over result set one row at a time
            
            update.execute(); 
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
        }
    }

    void Ride::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Ride (route_id, creator_id, ride_time, ride_passengers) VALUES(?, ?, ?, ?) ",
                use(_route_id),
                use(_creator_id),
                use(_ride_time),
                use(_ride_passengers),

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_ride_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }


            std::cout << "inserted:" << _route_id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }




    long Ride::get_ride_id() const
    {
        return _ride_id;
    }
    long Ride::get_route_id() const
    {
        return _route_id;
    }
    long Ride::get_creator_id() const
    {
        return _creator_id;
    }
    long Ride::get_ride_time() const
    {
        return _ride_time;
    }
    const std::string &Ride::get_ride_passengers() const
    {
        return _ride_passengers;
    }

    long &Ride::ride_id()
    {
        return _ride_id;
    }
    long &Ride::route_id()
    {
        return _route_id;
    }
    long &Ride::creator_id() 
    {
        return _creator_id;
    }
    long &Ride::ride_time()
    {
        return _ride_time;
    }
    std::string &Ride::ride_passengers()
    {
        return _ride_passengers;
    }

}