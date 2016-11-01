// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef DTO_USER_H
#define DTO_USER_H

#include <QSharedPointer>
#include <QString>

namespace dto {

  class User
  {
    int m_id;
    QString m_userName;
    QString m_firstName;
    QString m_lastName;
    QString m_email;
    QString m_ssn;

  public:
    int getId() const { return m_id; }
    QString getUserName() const { return m_userName; }
    QString getFirstName() const { return m_firstName; }
    QString getLastName() const { return m_lastName; }
    QString getEmail() const { return m_email; }
    QString getSsn() const { return m_ssn; }

    void setId(int id) { m_id = id; }
    void setUserName(const QString& userName) { m_userName = userName; }
    void setFirstName(const QString& firstName) { m_firstName = firstName; }
    void setLastName(const QString& lastName) { m_lastName = lastName; }
    void setEmail(const QString& email) { m_email = email; }
    void setSsn(const QString& ssn) { m_ssn = ssn; }
  };

  typedef QSharedPointer<dto::User> UserPtr;

}

#endif
