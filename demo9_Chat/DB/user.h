#ifndef USER_H
#define USER_H

#include <QObject>
class User
{
public:
    //用户信息
    explicit User(
            QString username = "",
            QString password = "",
            QString nickname = "",
            QString image = "",
            QString motto = "",
            QString sex = "",
            quint32 age = 0,
            QString birthday = "",
            QString location =""
        );
    //查询接口
    QString getName() const;
    QString getNickname() const;
    QString getPswd() const;
    QString getImage();
    QString getMotto();
    QString getSex();
    quint32 getAge();
    QString getBirth();
    QString getLocation();
    void alterPswd(QString password1);
    void alterNickname(QString nickname1);
    void alterImage(QString image1);
    void alterMotto(QString motto1);
    void alterSex(QString sex1);
    void alterAge(quint32 age1);
    void alterBirth(QString birthday1);
    void alterLocation(QString location1);
private:
    QString username;//用户名
    QString password;//密码
    QString nickname;//昵称
    QString image;//头像路径
    QString motto;//个性签名
    QString sex;//性别
    quint32 age;//年龄
    QString birthday;//生日
    QString location;//地区
};

#endif // USER_H
