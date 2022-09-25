#ifndef RESPONSESERVICETEST_USERSERVICE_H
#define RESPONSESERVICETEST_USERSERVICE_H

#include <functional>
#include <string>

#include "daos/DaoInterfaces/UserDao.h"

using std::string;
using std::function;

typedef std::basic_string<unsigned char> ustring;

namespace service
{
    class UserService
    {
    private:
        static ustring createRandomSalt(size_t length);
        UserDao& _userDao;
    public:
        UserService() = delete;
        UserService(const UserService&) = delete;
        UserService(UserService&&) = delete;
        UserService& operator=(const UserService&) = delete;
        UserService& operator=(UserService&&) = delete;

        UserService(UserDao& userDao);

        void createUser(const string& username, const string& password, function<void(bool)> callback);
        void checkUserPassword(const string& username, const string& password, function<void(bool)> callback);
        void checkUserSession(const string& username, const string& sessionCookie, function<void(bool)> callback);
        void createNewUserSession(const string& username, function<void(string&&)> callback);
    };
} // service

#endif //RESPONSESERVICETEST_USERSERVICE_H