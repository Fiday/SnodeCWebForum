#Create Database  Forum;

CREATE OR REPLACE TABLE User
(
    id            INT UNSIGNED AUTO_INCREMENT,
    username      VARCHAR(30)  not null unique,
    passwordHash  VARCHAR(32)  not null,
    salt          VARCHAR(32)  not null,
    avatarURL     VARCHAR(150) not null,
    sessionToken  VARCHAR(512) not null default '',
    creationDate  datetime not null default NOW(),
    PRIMARY KEY (id)
);

CREATE OR REPLACE TABLE Topic
(
    id           INT UNSIGNED AUTO_INCREMENT,
    creatorID    INT UNSIGNED,
    title        VARCHAR(100) not null,
    description  VARCHAR(500) not null,
    creationDate datetime default NOW(),
    PRIMARY KEY (id),
    FOREIGN KEY (creatorID) REFERENCES User (ID)
);

CREATE OR REPLACE TABLE Post
(
    id           INT UNSIGNED AUTO_INCREMENT,
    topicID      INT UNSIGNED,
    creatorID    INT UNSIGNED,
    title        VARCHAR(100) not null,
    content      VARCHAR(500) not null,
    creationDate datetime default NOW(),
    PRIMARY KEY (id),
    FOREIGN KEY (creatorID) REFERENCES User (ID),
    FOREIGN KEY (topicID) REFERENCES Topic (ID)
);

CREATE OR REPLACE TABLE Comment
(
    id           INT UNSIGNED AUTO_INCREMENT,
    postID       INT UNSIGNED,
    creatorID     INT UNSIGNED,
    content      VARCHAR(500) not null,
    creationDate datetime default NOW(),
    PRIMARY KEY (id),
    FOREIGN KEY (postID) REFERENCES Post (ID),
    FOREIGN KEY (creatorID) REFERENCES User (ID)
);





