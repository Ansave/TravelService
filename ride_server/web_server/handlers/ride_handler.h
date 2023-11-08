#ifndef RIDERHANDLER_H
#define RIDEHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../../ride_server/database/ride.h"
#include "../../../helper.h"

class RideHandler : public HTTPRequestHandler
{
public:
    RideHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        try
        {
            
            if (hasSubstr(request.getURI(), "/add_ride") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) &&
                form.has("route_id")&&
                form.has("creator_id")&&
                form.has("ride_time"))
            {
                database::Ride ride;
                ride.route_id() = atol(form.get("route_id").c_str());
                ride.creator_id() = atol(form.get("creator_id").c_str());
                ride.ride_time() = atol(form.get("ride_time").c_str());
                ride.ride_passengers() = "";
                ride.save_to_mysql();
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                ostr << ride.get_ride_id();
                return;
                
            }
            else if (hasSubstr(request.getURI(), "/add_passenger") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST&&
                form.has("ride_id")&&
                form.has("user_id"))
            {
                long ride_id = atol(form.get("ride_id").c_str());
                long user_id = atol(form.get("user_id").c_str());
                database::Ride ride;
                ride.add_passenger(ride_id, user_id);
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                ostr << ride.get_ride_id();
                return;
            } else if (hasSubstr(request.getURI(), "/read_ride_by_id") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET&&
                        form.has("ride_id")) 
            {
                long ride_id = atol(form.get("ride_id").c_str());
                std::optional<database::Ride> result = database::Ride::read_ride_by_id(ride_id);
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(result->toJSON(), ostr);

                return;
            }
        }
        catch (...) {   
        }
        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "request not found");
        root->set("instance", "/ride");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }


private:
    std::string _format;
};
#endif