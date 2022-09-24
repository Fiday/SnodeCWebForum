#ifndef RESPONSESERVICETEST_HTMLRESPONSECREATIONSERVICE_H
#define RESPONSESERVICETEST_HTMLRESPONSECREATIONSERVICE_H

#include <functional>
#include <vector>
#include <string>
#include "../daos/DaoInterfaces/CommentDao.h"
#include "../daos/DaoInterfaces/PostDao.h"
#include "../daos/DaoInterfaces/TopicDao.h"
#include "../daos/DaoInterfaces/UserDao.h"

using std::string;
using std::vector;
typedef std::function<void(string)> resCallback;

namespace service
{
    class HTMLResponseCreationService
    {
    private:
        static const char* DEFAULT_TITLE;
        static const char* EMPTY_USER_NAME;

        static string createHead(const string& title = DEFAULT_TITLE);
        static string createHeader(const string& userName);
        static string createFooter();
        static string createAndFillResponse(const string& content, const string& userName, const string& title,
                                            bool addHeader = true);

        CommentDao& _commentDao;
        PostDao& _postDao;
        TopicDao& _topicDao;
        UserDao& _userDao;

    public:
        HTMLResponseCreationService() = delete;
        HTMLResponseCreationService(const HTMLResponseCreationService&) = delete;
        HTMLResponseCreationService(HTMLResponseCreationService&&) = delete;
        HTMLResponseCreationService& operator=(const HTMLResponseCreationService&) = delete;
        HTMLResponseCreationService& operator=(HTMLResponseCreationService&&) = delete;

        HTMLResponseCreationService(CommentDao&, PostDao&, TopicDao&, UserDao&);

        static string createHomeResponse(const vector<Topic>& topics, const vector<int>& postCounts,
                                         const vector<Post>& latestPostsIncCreator, const string& userName);

        static string createNotFoundResponse(const string& userName = EMPTY_USER_NAME);

        static string createTopicOverviewResponse(const Topic& topic, const vector<Post>& postsIncCreator,
                                                  const vector<int>& postCommentCounts,
                                                  const string& userName = EMPTY_USER_NAME);

        static string createPostOverviewResponse(const Post& postIncCreatorAndTopic,
                                                 const vector<Comment>& commentsIncCreator,

                                                 const string& userName = EMPTY_USER_NAME);
        static string createLoginPageResponse();

        static string createRegisterAccountResponse();

        void createHomeResponseFromDao(const string& userName, const resCallback&);
        void createTopicOverviewResponseFromDao(unsigned long topicId, const string& userName, const resCallback&);
        void createPostOverviewResponseFromDao(unsigned long postId, const string& userName, const resCallback&);
    };

}

#endif //RESPONSESERVICETEST_HTMLRESPONSECREATIONSERVICE_H