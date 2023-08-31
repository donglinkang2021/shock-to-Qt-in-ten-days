#include "groupinfo.h"
Groupinfo::Groupinfo(
    QString groupname,
    QString groupprofile
) : groupname(groupname), groupprofile(groupprofile)
{}
QString Groupinfo::getGroupName() const
{
    return groupname;
}

QString Groupinfo::getGroupProfile() const
{
    return groupprofile;
}
