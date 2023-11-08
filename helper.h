// #pragma once
// #include <string>
// #include "Poco/Base64Decoder.h"
// #include <istream>
// #include <ostream>
// #include "Poco/Net/HTTPServer.h"
// #include "Poco/Net/HTTPRequestHandler.h"
// #include "Poco/Net/HTTPRequestHandlerFactory.h"
// #include "Poco/Net/HTTPServerParams.h"
// #include "Poco/Net/HTTPServerRequest.h"
// #include "Poco/Net/HTTPServerResponse.h"
// #include "Poco/Net/HTTPServerParams.h"
// #include "Poco/Net/HTMLForm.h"
// #include "Poco/Net/ServerSocket.h"
// #include "Poco/Timestamp.h"
// #include "Poco/DateTimeFormatter.h"
// #include "Poco/DateTimeFormat.h"
// #include "Poco/Exception.h"
// #include "Poco/ThreadPool.h"
// #include "Poco/Util/ServerApplication.h"
// #include "Poco/Util/Option.h"
// #include "Poco/Util/OptionSet.h"
// #include "Poco/Util/HelpFormatter.h"
// #include <iostream>
// #include <iostream>
// #include <fstream>
// #include "Poco/StreamCopier.h"
// #include "Poco/URI.h"
// #include "Poco/Exception.h"
// #include "Poco/SharedPtr.h"
// #include "Poco/Net/SSLManager.h"
// #include "Poco/Net/KeyConsoleHandler.h"
// #include "Poco/Net/ConsoleCertificateHandler.h"
// #include "Poco/Net/AcceptCertificateHandler.h"
// #include "Poco/Net/HTTPSClientSession.h"
// #include "Poco/Net/HTTPRequest.h"
// #include "Poco/Net/HTTPResponse.h"
// #include <Poco/JSON/Parser.h>
// #include <Poco/Dynamic/Var.h>

// #include <Poco/URIStreamFactory.h>
// #include <Poco/URIStreamOpener.h>
// #include <Poco/Net/HTTPSStreamFactory.h>
// #include <Poco/Net/HTTPStreamFactory.h>
// #include <Poco/Base64Encoder.h>
// #include <Poco/Base64Decoder.h>

// bool get_identity(const std::string identity, std::string &login, std::string &password)
// {
//     std::istringstream istr(identity);
//     std::ostringstream ostr;
//     Poco::Base64Decoder b64in(istr);
//     copy(std::istreambuf_iterator<char>(b64in),
//          std::istreambuf_iterator<char>(),
//          std::ostreambuf_iterator<char>(ostr));
//     std::string decoded = ostr.str();

//     size_t pos = decoded.find(':');
//     login = decoded.substr(0, pos);
//     password = decoded.substr(pos + 1);
//     return true;
// }

// std::optional<std::string> do_get(const std::string &url, const std::string &login, const std::string &password)
//     {
//         std::string string_result;
//         try
//         {
//             std::string token = login + ":" + password;
//             std::ostringstream os;
//             Poco::Base64Encoder b64in(os);
//             b64in << token;
//             b64in.close();
//             std::string identity = "Basic " + os.str();

//             Poco::URI uri(url);
//             Poco::Net::HTTPClientSession s(uri.getHost(), uri.getPort());
//             Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.toString());
//             request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
//             request.setContentType("application/json");
//             request.set("Authorization", identity);
//             request.set("Accept", "application/json");
//             request.setKeepAlive(true);

//             s.sendRequest(request);

//             Poco::Net::HTTPResponse response;
//             std::istream &rs = s.receiveResponse(response);

//             while (rs)
//             {
//                 char c{};
//                 rs.read(&c, 1);
//                 if (rs)
//                     string_result += c;
//             }

//             if (response.getStatus() != 200)
//                 return {};
//         }
//         catch (Poco::Exception &ex)
//         {
//             std::cout << "exception:" << ex.what() << std::endl;
//             return std::optional<std::string>();
//         }

//         return string_result;
//     }

// long TryAuth(HTTPServerRequest &request,
//             HTTPServerResponse &response)
// {
//     std::string scheme;
//     std::string info;
//     std::string login, password;

//     try{
//         request.getCredentials(scheme, info);
//     }
//     catch(...)
//     {
       
//     }
    
//     if (scheme == "Basic")
//     {
//         get_identity(info, login, password);
//         std::cout << "login:" << login << std::endl;
//         std::cout << "password:" << password << std::endl;
//         std::string host = "localhost";
//         std::string url;

//         if(std::getenv("SERVICE_HOST")!=nullptr) host = std::getenv("SERVICE_HOST");
        
//         url = "http://" + host+":8080/auth";

//         try{
//             std::optional<std::string> authStr = do_get(url, login, password);

//             if (authStr.has_value())
//             {
//                 Poco::JSON::Parser parser;
//                 Poco::Dynamic::Var result = parser.parse(authStr.value());
//                 Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

//                 return object->getValue<long>("id");
//             }
//         }
//         catch(...)
//         {
            
//         }
//     }

//     response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
//     response.setChunkedTransferEncoding(true);
//     response.setContentType("application/json");
//     Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
//     root->set("type", "/errors/unauthorized");
//     root->set("title", "Unauthorized");
//     root->set("status", "401");
//     root->set("detail", "Invalid login or password");
//     root->set("instance", "/User");
//     std::ostream &ostr = response.send();
//     Poco::JSON::Stringifier::stringify(root, ostr);

//     return 0;
// }

#pragma once
#include <string>
#include "Poco/Base64Decoder.h"
#include <istream>
#include <ostream>
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
#include <iostream>
#include <fstream>
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <Poco/URIStreamFactory.h>
#include <Poco/URIStreamOpener.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>

bool get_identity(const std::string identity, std::string &login, std::string &password)
{
    std::istringstream istr(identity);
    std::ostringstream ostr;
    Poco::Base64Decoder b64in(istr);
    copy(std::istreambuf_iterator<char>(b64in),
         std::istreambuf_iterator<char>(),
         std::ostreambuf_iterator<char>(ostr));
    std::string decoded = ostr.str();

    size_t pos = decoded.find(':');
    login = decoded.substr(0, pos);
    password = decoded.substr(pos + 1);
    return true;
}

std::optional<std::string> do_get(const std::string &url, const std::string &login, const std::string &password)
    {
        std::string string_result;
        try
        {
            std::string token = login + ":" + password;
            std::ostringstream os;
            Poco::Base64Encoder b64in(os);
            b64in << token;
            b64in.close();
            std::string identity = "Basic " + os.str();

            Poco::URI uri(url);
            Poco::Net::HTTPClientSession s(uri.getHost(), uri.getPort());
            Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.toString());
            request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
            request.setContentType("application/json");
            request.set("Authorization", identity);
            request.set("Accept", "application/json");
            request.setKeepAlive(true);

            s.sendRequest(request);

            Poco::Net::HTTPResponse response;
            std::istream &rs = s.receiveResponse(response);

            while (rs)
            {
                char c{};
                rs.read(&c, 1);
                if (rs)
                    string_result += c;
            }

            if (response.getStatus() != 200)
                return {};
        }
        catch (Poco::Exception &ex)
        {
            std::cout << "exception:" << ex.what() << std::endl;
            return std::optional<std::string>();
        }

        return string_result;
    }

long TryAuth(HTTPServerRequest &request,
            HTTPServerResponse &response)
{
    std::string scheme;
    std::string info;
    std::string login, password;

    try{
        request.getCredentials(scheme, info);
    }
    catch(...)
    {
       
    }
    
    if (scheme == "Basic")
    {
        get_identity(info, login, password);
        std::cout << "login:" << login << std::endl;
        std::cout << "password:" << password << std::endl;
        std::string host = "localhost";
        std::string url;

        if(std::getenv("SERVICE_HOST")!=nullptr) host = std::getenv("SERVICE_HOST");
        
        url = "http://" + host+":8080/auth";

        try{
            std::optional<std::string> authStr = do_get(url, login, password);

            if (authStr.has_value())
            {
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var result = parser.parse(authStr.value());
                Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

                return object->getValue<long>("id");
            }
        }
        catch(...)
        {
            
        }
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/unauthorized");
    root->set("title", "Unauthorized");
    root->set("status", "401");
    root->set("detail", "Invalid login or password");
    root->set("instance", "/User");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);

    return 0;
}