#include "UserService.h"
#include "../utils/Utils.h"

#include <memory>
#include <iostream>

using std::shared_ptr;
using std::make_shared;

typedef service::UserService cls;

namespace service {

#pragma region Constants

    const size_t SALT_LEN = 16;
    const size_t HASH_LEN = 16;
    const size_t HASH_ITERATIONS = 4096;
    const size_t SESSION_TOKEN_LEN = 256;

    const size_t USERNAME_MIN_LENGTH = 3;
    const size_t PASSWORD_MIN_LENGTH = 8;

    const char *US_USERNAME_ERR_MSG = "Username must be at least 3 characters long";
    const char *US_PASSWORD_ERR_MSG = "Password must be at least 8 characters long";
    const char *US_INTERNAL_ERR_MSG = "There seems to be a problem with the server. Try again later";

#pragma endregion

#pragma region Constructors

    UserService::UserService(UserDao &userDao) : _userDao(userDao) {}

#pragma endregion

#pragma region Static Methods

    std::string cls::hashPassword(const string &password, const ustring &salt) {
        return Utils::hashPassword(password, salt, HASH_ITERATIONS, HASH_LEN);
    }

    ustring cls::createNewSalt() {
        return Utils::createRandomSalt(SALT_LEN);
    }

    std::string cls::createNewSessionToken() {
        ustring sessionTokenU = Utils::createRandomSalt(SESSION_TOKEN_LEN);
        return {Utils::charToHex(sessionTokenU.c_str(), sessionTokenU.length())};
    }

    std::string cls::createAvatarURL(const std::string &username) {
        static const string avatarAPI{
                "https://avatars.dicebear.com/api/identicon/"
        };

        static const string settingsAPI{
                ".svg?b=%2311111199&scale=65&colorLevel=600"
        };

        return avatarAPI + username + settingsAPI;
    }

    bool cls::checkUserNameFormat(const std::string &username) {
        bool result = true;
        result &= username.length() >= USERNAME_MIN_LENGTH;
        return result;
    }

    bool cls::checkPasswordFormat(const std::string &password) {
        bool result = true;
        result &= password.length() >= PASSWORD_MIN_LENGTH;
        return result;
    }

    vector<string> cls::getUserCreateErrorMessages(int errorCode) {
        static const string invalidUsername{US_USERNAME_ERR_MSG};
        static const string invalidPassword{US_PASSWORD_ERR_MSG};
        static const string internalError{US_INTERNAL_ERR_MSG};

        vector<string> result;
        if (errorCode & INVALID_USERNAME) {
            result.push_back(invalidUsername);
        }

        if (errorCode & INVALID_PASSWORD) {
            result.push_back(invalidPassword);
        }

        if (errorCode & INTERNAL_ERROR) {
            result.push_back(internalError);
        }

        return std::move(result);
    }

#pragma endregion

#pragma region Instance Methods

    void cls::createUser(const string &username, const string &password, function<void(int)> callback) {
        int result = SUCCESS;
        if (!checkUserNameFormat(username)) { // invalid username
            result |= INVALID_USERNAME;
        }

        if (!checkPasswordFormat(password)) { // invalid password
            result |= INVALID_PASSWORD;
        }

        if (result != SUCCESS) {
            callback(result);
            return;
        }

        string avatarURL = createAvatarURL(username);
        ustring salt = createNewSalt();
        string hash = hashPassword(password, salt);

        function<void(bool)> createCallback = [callback](bool success) {
            callback(success ? SUCCESS : INTERNAL_ERROR);
        };

        _userDao.createUser(username, hash, salt, avatarURL, createCallback);
    }

    void cls::checkUserNameTaken(const string &username, function<void(bool)> callback) {
        _userDao.isUserNameTaken(username, callback);
    }

    void cls::checkUserPassword(const string &username, const string &password, function<void(bool)> callback) {
        if (username.empty() || password.empty()) {
            callback(false);
            return;
        }

        shared_ptr<string> usernamePtr = make_shared<string>(username);
        shared_ptr<string> passwordPtr = make_shared<string>(password);

        function<void(bool)> userExistsCallback = [this, usernamePtr, passwordPtr, callback](bool userExists) {

            if (!userExists) {
                callback(false);
                return;
            }

            function<void(ustring &&)> saltCallback = [this, usernamePtr, passwordPtr, callback](ustring &&salt) {
                string curHash = hashPassword(*passwordPtr, salt);
                shared_ptr<string> curHashPtr = make_shared<string>(std::move(curHash));

                function<void(string)> hashCallback = [curHashPtr, callback](string &&hash) {

                    std::cout << hash << std::endl;
                    std::cout << *curHashPtr << std::endl;
                    callback((*curHashPtr) == hash);
                };

                _userDao.getPasswordHashByUsername(*usernamePtr, hashCallback);
            };

            _userDao.getSaltByUsername(*usernamePtr, saltCallback);
        };

        _userDao.isUserNameTaken(username, userExistsCallback);
    }

    void cls::checkUserSession(const string &username, const string &sessionToken, function<void(bool)> callback) {
        if (username.empty() || sessionToken.empty()) {
            callback(false);
            return;
        }

        shared_ptr<string> usernamePtr = make_shared<string>(username);
        shared_ptr<string> sessionTokenPtr = make_shared<string>(sessionToken);

        function<void(bool)> userExistsCallback = [this, usernamePtr, sessionTokenPtr, callback](bool userExists) {
            if (!userExists)
            {
                callback(false);
                return;
            }

            function<void(string &&)> sessionTokenCallback = [sessionTokenPtr, callback](string &&sessionToken) {
                callback(!sessionToken.empty() && *sessionTokenPtr == sessionToken);
            };

            _userDao.getSessionTokenByUsername(*usernamePtr, sessionTokenCallback);
        };

        _userDao.isUserNameTaken(username, userExistsCallback);
    }

    void cls::createNewUserSession(const string &username, function<void(bool, string &&)> callback) {
        if (username.empty()) {
            callback(false, "");
            return;
        }

        shared_ptr<string> usernamePtr = make_shared<string>(username);

        function<void(bool)> userExistsCallback = [this, usernamePtr, callback](bool userExists) {
            if (!userExists)
            {
                callback(false, "");
                return;
            }

            shared_ptr<string> newSessionTokenPtr = make_shared<string>(createNewSessionToken());
            _userDao.setSessionTokenByUsername(*usernamePtr, (*newSessionTokenPtr),
                                               [callback, newSessionTokenPtr](bool b) {
                                                   callback(b, std::move(*newSessionTokenPtr));
                                               });
        };

        _userDao.isUserNameTaken(username, userExistsCallback);
    }

#pragma endregion

} // service