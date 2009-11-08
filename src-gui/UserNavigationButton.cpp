// Lose Face - An open source face recognition project
// Copyright (C) 2008-2009 David Capello
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the authors nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "UserNavigationButton.h"
#include <QtGui>
		       
UserNavigationButton::UserNavigationButton(QWidget* parent,
					   const dto::User& user, QImage photo)
  : QWidget(parent)
  , m_user(user)
  , m_photo(photo)
  , m_forceHot(false)
{
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
    bar.addAction(tr("Remove"));
    bar.exec(mapToGlobal(event->pos()));

    m_forceHot = underMouse();	// TODO
    update(rect());
  }  
}
