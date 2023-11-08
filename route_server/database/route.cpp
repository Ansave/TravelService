#include "route.h"
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

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Route::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Route` (`route_id` INT NOT NULL AUTO_INCREMENT,"
                        << "`creator_id` INT NOT NULL,"
                        << "`route_title` VARCHAR(256) NOT NULL,"
                        << "`route_description` VARCHAR(256) NOT NULL,"
                        << "`route_start` text NOT NULL ,"
                        << "`route_end` text NOT NULL ,"
                        << "PRIMARY KEY (`route_id`));",
                now;

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

    Poco::JSON::Object::Ptr Route::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("route_id", _route_id);
        root->set("creator_id", _creator_id);
        root->set("route_title", _route_title);
        root->set("route_description", _route_description);
        root->set("route_start", _route_start);
        root->set("route_end", _route_end);

        return root;
    }

    Route Route::fromJSON(const std::string &str)
    {
        Route route;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        route.route_id() = object->getValue<long>("route_id");
        route.creator_id() = object->getValue<long>("creator_id");
        route.route_title() = object->getValue<std::string>("route_title");
        route.route_description() = object->getValue<std::string>("route_description");
        route.route_start() = object->getValue<std::string>("route_start");
        route.route_end() = object->getValue<std::string>("route_end");

        return route;
    }

    std::vector<Route> Route::read_all_routes_by_user_id(long user_id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Route> result;
            Route a;
            select << "SELECT route_id, creator_id, route_title, route_description, route_start, route_end FROM Route WHERE creator_id=? ",
                into(a._route_id),
                into(a._creator_id),
                into(a._route_title),
                into(a._route_description),
                into(a._route_start),
                into(a._route_end),
                use(user_id),
                range(0, 1); //  iterate over result set one row at a time
            
            while (!select.done())
            {
                if (select.execute()) {
                    result.push_back(a);
                }
            }
            return result;
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

    void Route::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Route (creator_id, route_title, route_description, route_start, route_end) VALUES(?, ?, ?, ?, ?) ",
                use(_creator_id),
                use(_route_title),
                use(_route_description),
                use(_route_start),
                use(_route_end),

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_route_id),
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
    long Route::get_route_id() const
    {
        return _route_id;
    }
        long Route::get_creator_id() const
    {
        return _creator_id;
    }
    const std::string &Route::get_route_title() const
    {
        return _route_title;
    }
    const std::string &Route::get_route_description() const
    {
        return _route_description;
    }
    const std::string &Route::get_route_start() const
    {
        return _route_start;
    }
    const std::string &Route::get_route_end() const
    {
        return _route_end;
    }

    long &Route::route_id()
    {
        return _route_id;
    }
    long &Route::creator_id()
    {
        return _creator_id;
    }
    std::string &Route::route_title()
    {
        return _route_title;
    }
    std::string &Route::route_description()
    {
        return _route_description;
    }
    std::string &Route::route_start()
    {
        return _route_start;
    }
    std::string &Route::route_end()
    {
        return _route_end;
    }
}