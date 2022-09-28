#include "UserService.h"

#include <memory>

#include "../utils/Utils.h"

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

#pragma endregion

#pragma region Constructors

    UserService::UserService(UserDao& userDao) : _userDao(userDao) {}

#pragma endregion

#pragma region Static Methods

    std::string cls::hashPassword(const string& password, const ustring& salt) {
        return Utils::hashPassword(password, salt, HASH_ITERATIONS, HASH_LEN);
    }

    ustring cls::createNewSalt() {
        return Utils::createRandomSalt(SALT_LEN);
    }

    std::string cls::createNewSessionToken() {
        ustring sessionTokenU = Utils::createRandomSalt(SESSION_TOKEN_LEN);
        return {reinterpret_cast<const char*>(sessionTokenU.c_str()), SESSION_TOKEN_LEN};
    }

    std::string cls::createAvatarURL(const std::string& username) {
        static const string avatarAPI{
                "https://avatars.dicebear.com/api/identicon/"
        };

        static const string settingsAPI{
                ".svg?b=%2311111199&scale=65&colorLevel=600"
        };

        return avatarAPI + username + settingsAPI;
    }

    bool cls::checkUserNameFormat(const std::string& username) {
        bool result = true;
        result &= username.length() >= USERNAME_MIN_LENGTH;
        return result;
    }

    bool cls::checkPasswordFormat(const std::string& password) {
        bool result = true;
        result &= password.length() >= USERNAME_MIN_LENGTH;
        return result;
    }

#pragma endregion

#pragma region Instance Methods

    void cls::createUser(const string& username, const string& password, function<void(int)> callback) {
        int result = UserCreationResult::SUCCESS;
        if(!checkUserNameFormat(username)) { // invalid username
            result |= UserCreationResult::INVALID_USERNAME;
        }

        if (!checkPasswordFormat(password)) { // invalid password
            result |= UserCreationResult::INVALID_PASSWORD;
        }

        if (result != UserCreationResult::SUCCESS) {
            callback(result);
            return;
        }

        string avatarURL = createAvatarURL(username);
        ustring salt = createNewSalt();
        string hash = hashPassword(password, salt);

        function<void(bool)> createCallback = [callback](bool success) {
            callback(success ? UserCreationResult::SUCCESS : UserCreationResult::INTERNAL_ERROR);
        };

        _userDao.createUser(username, hash, salt, avatarURL, createCallback);
    }

    void cls::checkUserNameTaken(const string& username, function<void(bool)> callback) {
        _userDao.isUserNameTaken(username, callback);
    }

    void cls::checkUserPassword(const string& username, const string& password, function<void(bool)> callback) {
        shared_ptr<string> usernamePtr = make_shared<string>(username);
        shared_ptr<string> passwordPtr = make_shared<string>(password);

        function<void(ustring&&)> saltCallback = [this, usernamePtr, passwordPtr, callback](ustring&& salt) {
            string curHash = hashPassword(*passwordPtr, salt);
            shared_ptr<string> curHashPtr = make_shared<string>(std::move(curHash));

            function<void(string)> hashCallback = [curHashPtr, callback](string&& hash) {
                callback((*curHashPtr) == hash);
            };

            _userDao.getPasswordHashByUsername(*usernamePtr, hashCallback);
        };

        _userDao.getSaltByUsername(username, saltCallback);
    }

    void cls::checkUserSession(const string& username, const string& sessionToken, function<void(bool)> callback) {
        shared_ptr<string> sessionTokenPtr = make_shared<string>(sessionToken);

        function<void(string&&)> sessionTokenCallback = [sessionTokenPtr, callback](string&& sessionToken) {
            callback(*sessionTokenPtr == sessionToken);
        };

        _userDao.getSessionTokenByUsername(username, sessionTokenCallback);
    }

    void cls::createNewUserSession(const string& username, function<void(bool)> callback) {
        shared_ptr<string> usernamePtr = make_shared<string>(username);
        string newSessionToken = createNewSessionToken();
        _userDao.setSessionTokenByUsername(username, newSessionToken, callback);
    }

#pragma endregion

} // service