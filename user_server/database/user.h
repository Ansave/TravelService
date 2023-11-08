#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class User{
        private:
            long _id;
            long _total_id;
            std::string _first_name;
            std::string _last_name;
            std::string _email;
            std::string _birthday;
            std::string _login;
            std::string _password;

        public:

            static User fromJSON(const std::string & str);

            long             get_id() const;
            long             get_total_id() const;
            const std::string &get_first_name() const;
            const std::string &get_last_name() const;
            const std::string &get_email() const;
            const std::string &get_birthday() const;
            const std::string &get_login() const;
            const std::string &get_password() const;

            long&        id();
            long&        total_id();
            std::string &first_name();
            std::string &last_name();
            std::string &email();
            std::string &birthday();
            std::string &login();
            std::string &password();

            static void init();
            static std::optional<User> read_by_id(long id);
            static std::optional<long> auth(std::string &login, std::string &password);
            static std::vector<User> search(std::string first_name,std::string last_name);
            static std::vector<User> search_by_login(std::string login);
            void save_to_mysql();
            long db_length();
            
            void send_to_queue();
            void save_to_cache();
            static std::optional<User> read_from_cache_by_id(long id);

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif
