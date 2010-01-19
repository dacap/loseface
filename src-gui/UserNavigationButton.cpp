// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#include "UserNavigationButton.h"
#include "dao/User.h"
#include "LoseFaceApp.h"
#include <QtGui>
		       
UserNavigationButton::UserNavigationButton(QWidget* parent,
					   const dto::User& user, QImage photo)
  : QWidget(parent)
  , m_user(user)
  , m_photo(photo)
  , m_forceHot(false)
{
  createActions();

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
}

QSize UserNavigationButton::sizeHint() const
{
  return QSize(128, 128);
}

void UserNavigationButton::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  QPen pen(QColor(0, 128, 255));
  painter.setPen(pen);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QRect rc(0, 0, width()-1, height()-1);

  if (underMouse() || m_forceHot) {
    QBrush brush(QColor(100, 240, 255));
    painter.setBrush(brush);
  }

  painter.drawRoundedRect(rc, 16, 16);

  if (m_photo.isNull()) {
    painter.setFont(QFont("Tahoma", 14));
    painter.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, tr("No photo"));
  }
  else {
    QSize sz(m_photo.size());
    sz.scale(rc.size()*0.9, Qt::KeepAspectRatio);
    painter.drawImage(QRect(rc.center()-QPoint(sz.width()/2, sz.height()/2), sz), m_photo);
  }
}

void UserNavigationButton::enterEvent(QEvent* event)
{
  QWidget::enterEvent(event);
  update(rect());
}

void UserNavigationButton::leaveEvent(QEvent* event)
{
  QWidget::leaveEvent(event);
  update(rect());
}

void UserNavigationButton::mousePressEvent(QMouseEvent* event)
{
  QWidget::mousePressEvent(event);

  if (event->button() == Qt::RightButton) {
    m_forceHot = true;

    QMenu bar(this);
    bar.addAction(tr("Photos"));
    bar.addSeparator();
    bar.addAction(m_deleteUser);
    bar.exec(mapToGlobal(event->pos()));

    m_forceHot = underMouse();	// TODO
    update(rect());
  }
}

void UserNavigationButton::deleteUser()
{
  emit userDeleted(this);
}

void UserNavigationButton::createActions()
{
  m_deleteUser = new QAction(tr("&Delete"), this);
  connect(m_deleteUser, SIGNAL(triggered()), this, SLOT(deleteUser()));
}
