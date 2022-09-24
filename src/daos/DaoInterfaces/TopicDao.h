//
// Created by student on 9/21/22.
//

#ifndef GETANDPOSTSERVERTEMPLATE_SUBTOPICDAO_H
#define GETANDPOSTSERVERTEMPLATE_SUBTOPICDAO_H


#include <ctime>
#include <functional>
#include <vector>
#include "../../domain/Topic.h"

class TopicDao {

public:

    virtual void create(std::string title, int userID, const std::function<void(bool)> &callback) = 0;

    virtual void getCreator(unsigned long id, const std::function<void(User &&)> &callback) = 0;

    virtual void getRecentTopics(int amount, int start, const std::function<void(std::vector<Topic> &&)> &callback) = 0;

    virtual void getById(unsigned long id, const std::function<void(Topic &&)> &callback) = 0;

    virtual void getPostCount(unsigned long id, const std::function<void(int)> &callback) = 0;


};


#endif //GETANDPOSTSERVERTEMPLATE_SUBTOPICDAO_H
