#include "user.h"
#include "database.h"
#include "../config/config.h"
#include "../../data_cache/cache.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <cppkafka/cppkafka.h>

#include <mutex>
#include <sstream>
#include <exception>
#include <algorithm>
#include <future>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void User::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();

            for (auto &hint : database::Database::get_all_hints())
            {
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `User` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`total_id` INT NOT NULL,"
                        << "`first_name` VARCHAR(256) NOT NULL,"
                        << "`last_name` VARCHAR(256) NOT NULL,"
                        << "`login` VARCHAR(256) NOT NULL,"
                        << "`password` VARCHAR(256) NOT NULL,"
                        << "`email` VARCHAR(256) NULL,"
                        << "`birthday` VARCHAR(1024) NULL,"
                        << "PRIMARY KEY (`id`),KEY `tid` (`total_id`), KEY `fn` (`first_name`),KEY `ln` (`last_name`));"
                        << hint,
                        now;

             std::cout << create_stmt.toString() << std::endl;
            }
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

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("total_id", _total_id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("birthday", _birthday);
        root->set("login", _login);
        root->set("password", _password);

        return root;
    }

    User User::fromJSON(const std::string &str)
    {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        user.id() = object->getValue<long>("id");
        user.total_id() = object->getValue<long>("total_id");
        user.first_name() = object->getValue<std::string>("first_name");
        user.last_name() = object->getValue<std::string>("last_name");
        user.email() = object->getValue<std::string>("email");
        user.birthday() = object->getValue<std::string>("birthday");
        user.login() = object->getValue<std::string>("login");
        user.password() = object->getValue<std::string>("password");

        return user;
    }

    std::optional<long> User::auth(std::string &login, std::string &password)
    {
        try
        {
            std::vector<long> result;
            std::vector<std::string> hints = database::Database::get_all_hints();

            std::vector<std::future<std::vector<long>>> futures;
            for (const std::string &hint : hints)
            {
                auto handle = std::async(std::launch::async, [login, password, hint]() mutable -> std::vector<long>
                                        {
                                            std::vector<long> result;

                                            Poco::Data::Session session = database::Database::get().create_session();
                                            Statement select(session);
                                            std::string select_str = "SELECT total_id FROM User where login='";
                                            select_str += login;
                                            select_str += "' and password='";
                                            select_str += password;
                                            select_str += "'";
                                            select_str += hint;
                                            select << select_str;
                                            std::cout << select_str << std::endl;
                                            
                                            select.execute();
                                            Poco::Data::RecordSet record_set(select);

                                            bool more = record_set.moveFirst();
                                            while (more)
                                            {
                                                long a;
                                                a = record_set[0].convert<long>();
                                                result.push_back(a);
                                                more = record_set.moveNext();
                                            }
                                            return result; });

                futures.emplace_back(std::move(handle));
            }

            for (std::future<std::vector<long>> &res : futures)
            {
                std::vector<long> v = res.get();
                std::copy(std::begin(v),
                        std::end(v),
                        std::back_inserter(result));
            }
            if (result.size()) return result[0];
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

    std::optional<User> User::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            User a;
            std::string sharding_hint = database::Database::sharding_hint(id);
            std::string select_str = "SELECT id, total_id, first_name, last_name, email, birthday, login, password FROM User where total_id=? ";
            select_str += sharding_hint;
            std::cout << select_str << std::endl;

            select << select_str,
                into(a._id),
                into(a._total_id),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._birthday),
                into(a._login),
                into(a._password),
                use(id),
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

    std::vector<User> User::search(std::string first_name, std::string last_name)
    {
        std::vector<User> result;
        // get all hints for shards
        std::vector<std::string> hints = database::Database::get_all_hints();

        std::vector<std::future<std::vector<User>>> futures;
        first_name + "%";
        last_name + "%";

        for (const std::string &hint : hints)
        {
            auto handle = std::async(std::launch::async, [first_name, last_name, hint]() mutable -> std::vector<User>
                                     {
                                        std::vector<User> result;

                                        Poco::Data::Session session = database::Database::get().create_session();
                                        Statement select(session);
                                        std::string select_str = "SELECT id, total_id, first_name, last_name, email, birthday, login, password FROM User where first_name='";
                                        select_str += first_name;
                                        select_str += "' and last_name='";
                                        select_str += last_name;
                                        select_str += "'";
                                        select_str += hint;
                                        select << select_str;
                                        std::cout << select_str << std::endl;
                                        
                                        select.execute();
                                        Poco::Data::RecordSet record_set(select);
                                        
                                        bool more = record_set.moveFirst();
                                        while (more)
                                        {
                                            User a;
                                            a._id = record_set[0].convert<long>();
                                            a._total_id = record_set[1].convert<long>();
                                            a._first_name = record_set[2].convert<std::string>();
                                            a._last_name = record_set[3].convert<std::string>();
                                            a._email = record_set[4].convert<std::string>();
                                            a._birthday = record_set[5].convert<std::string>();
                                            a._login = record_set[6].convert<std::string>();
                                            a._password = record_set[7].convert<std::string>();
                                            result.push_back(a);
                                            more = record_set.moveNext();
                                        }
                                        return result; });
            futures.emplace_back(std::move(handle));
        }

        for (std::future<std::vector<User>> &res : futures)
        {
            std::vector<User> v = res.get();
            std::copy(std::begin(v),
                      std::end(v),
                      std::back_inserter(result));
        }

        return result;
    }

    std::vector<User> User::search_by_login(std::string login)
    {
        std::vector<User> result;
        // get all hints for shards
        std::vector<std::string> hints = database::Database::get_all_hints();

        std::vector<std::future<std::vector<User>>> futures;
        login + "%";


        for (const std::string &hint : hints)
        {
            auto handle = std::async(std::launch::async, [login, hint]() mutable -> std::vector<User>
                                     {
                                        std::vector<User> result;

                                        Poco::Data::Session session = database::Database::get().create_session();
                                        Statement select(session);
                                        std::string select_str = "SELECT id, total_id, first_name, last_name, email, birthday, login, password FROM User where login='";
                                        select_str += login;
                                        select_str += "'";
                                        select_str += hint;
                                        select << select_str;
                                        std::cout << select_str << std::endl;
                                        
                                        select.execute();
                                        Poco::Data::RecordSet record_set(select);
                                        
                                        bool more = record_set.moveFirst();
                                        while (more)
                                        {
                                            User a;
                                            a._id = record_set[0].convert<long>();
                                            a._total_id = record_set[1].convert<long>();
                                            a._first_name = record_set[2].convert<std::string>();
                                            a._last_name = record_set[3].convert<std::string>();
                                            a._email = record_set[4].convert<std::string>();
                                            a._birthday = record_set[5].convert<std::string>();
                                            a._login = record_set[6].convert<std::string>();
                                            a._password = record_set[7].convert<std::string>();
                                            result.push_back(a);
                                            more = record_set.moveNext();
                                        }
                                        return result; });
            futures.emplace_back(std::move(handle));
        }

        for (std::future<std::vector<User>> &res : futures)
        {
            std::vector<User> v = res.get();
            std::copy(std::begin(v),
                      std::end(v),
                      std::back_inserter(result));
        }

        return result;
    }
        

    long User::db_length()
    {
        long result = 0;
        std::vector<std::string> hints = database::Database::get_all_hints();
        for (const std::string &hint : hints)
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            long a;
            std::string select_str = "SELECT COUNT(*) FROM User";
            select_str += hint;
            select << select_str,
                into(a),
                range(0, 1);

            select.execute();
            Poco::Data::RecordSet rs(select);

            if (rs.moveFirst()) {
                result += a;
            }
        }
        return result;
    }
    void User::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            long db_len = User::db_length();
            db_len +=1;
            std::string sharding_hint = database::Database::sharding_hint(db_len);

            std::string select_str = "INSERT INTO User (total_id, first_name, last_name, email, birthday, login, password) VALUES(?, ?, ?, ?,?, ?, ?) ";
            select_str += sharding_hint;
            std::cout << "!!!" << select_str << std::endl;

            Poco::Data::Statement insert(session);

            
            insert << select_str,
                use(db_len),
                use(_first_name),
                use(_last_name),
                use(_email),
                use(_birthday),
                use(_login),
                use(_password);

            insert.execute();

            Poco::Data::Statement select(session);
            select_str = "SELECT LAST_INSERT_ID()";
            select_str += sharding_hint;
            select << select_str,
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
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

    std::optional<User> User::read_from_cache_by_id(long id)
    {

        try
        {
            std::string result;
            if (database::Cache::get().get(id, result))
                return fromJSON(result);
            else
                return std::optional<User>();
        }
        catch (std::exception& err)
        {
           // std::cerr << "error:" << err.what() << std::endl;
            return std::optional<User>();
        }
    }

    void User::send_to_queue()
    {
        static cppkafka::Configuration config = {
            {"metadata.broker.list", Config::get().get_queue_host()},
            {"acks", "all"}};
        static cppkafka::Producer producer(config);
        static std::mutex mtx;
        static int message_key{0};
        using Hdr = cppkafka::MessageBuilder::HeaderType;

        std::lock_guard<std::mutex> lock(mtx);
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        bool not_sent = true;

        cppkafka::MessageBuilder builder(Config::get().get_queue_topic());
        std::string mk = std::to_string(++message_key);
        builder.key(mk);                                       // set some key
        builder.header(Hdr{"producer_type", "author writer"}); // set some custom header
        builder.payload(message);                              // set message

        while (not_sent)
        {
            try
            {
                producer.produce(builder);
                not_sent = false;
            }
            catch (...)
            {
            }
        }
    }

     void User::save_to_cache()
    {
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        database::Cache::get().put(_total_id, message);
    }

    const std::string &User::get_login() const
    {
        return _login;
    }

    const std::string &User::get_password() const
    {
        return _password;
    }

    std::string &User::login()
    {
        return _login;
    }

    std::string &User::password()
    {
        return _password;
    }

    long User::get_id() const
    {
        return _id;
    }

    long User::get_total_id() const
    {
        return _total_id;
    }

    const std::string &User::get_first_name() const
    {
        return _first_name;
    }

    const std::string &User::get_last_name() const
    {
        return _last_name;
    }

    const std::string &User::get_email() const
    {
        return _email;
    }
    const std::string &User::get_birthday() const
    {
        return _birthday;
    }
    long &User::id()
    {
        return _id;
    }

    long &User::total_id()
    {
        return _total_id;
    }

    std::string &User::first_name()
    {
        return _first_name;
    }

    std::string &User::last_name()
    {
        return _last_name;
    }

    std::string &User::email()
    {
        return _email;
    }

    std::string &User::birthday()
    {
        return _birthday;
    }
}
