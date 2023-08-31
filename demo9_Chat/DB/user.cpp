#include "user.h"

//列表初始化
User::User(
    QString username,
    QString password,
    QString nickname,
    QString image,
    QString motto,
    QString sex,
    quint32 age,
    QString birthday,
    QString location
) : username(username),password(password),nickname(nickname),image(image),motto(motto),sex(sex),age(age),birthday(birthday),location(location)
{}

//查询函数实现

QString User::getName() const
{
    return username;
}

QString User::getNickname() const
{
    return nickname;
}

QString User::getPswd() const
{
    return password;
}

quint32 User::getAge()
{
    return age;
}

QString User::getBirth()
{
    return birthday;
}

QString User::getImage()
{
    return image;
}

QString User::getLocation()
{
    return location;
}

QString User::getMotto()
{
    return motto;
}

QString User::getSex()
{
    return sex;
}

void User::alterPswd(QString password1)
{
    password = password1;
}

void User::alterNickname(QString nickname1)
{
    nickname = nickname1;
}

void User::alterImage(QString image1)
{
    image = image1;
}
void User::alterMotto(QString motto1)
{
    motto = motto1;
}
void User::alterSex(QString sex1)
{
    sex = sex1;
}
void User::alterAge(quint32 age1)
{
    age = age1;
}
void User::alterBirth(QString birthday1)
{
    birthday = birthday1;
}
void User::alterLocation(QString location1)
{
    location = location1;
}


