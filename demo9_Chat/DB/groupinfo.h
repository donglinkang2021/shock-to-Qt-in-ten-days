#ifndef GROUPINFO_H
#define GROUPINFO_H
#include "user.h"

class Groupinfo :public User
{
public:
    //群聊信息
    explicit Groupinfo(QString groupname = "", QString groupprofile = "");
    QString getGroupName() const;
    QString getGroupProfile() const;
private:
    QString groupname;
    QString groupprofile;
};

#endif // GROUPINFO_H
