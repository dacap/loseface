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

#include "Navigator.h"
#include "MainFrame.h"
#include "LoseFaceApp.h"
#include "dao/User.h"
#include "dao/Picture.h"

#include "NewFuncs.h"
#include <Vaca/Vaca.h>

using namespace Vaca;

class NavUser : public Widget
{
  dto::User m_user;
  Image m_image;

public:
  NavUser(const dto::User& user, const Image& image, Widget* parent)
    : Widget(parent)
    , m_user(user)
    , m_image(image)
  {
    setBgColor(Color::White);
  }

  const dto::User* getUser() const
  {
    return &m_user;
  }

protected:

  virtual void onPreferredSize(PreferredSizeEvent& ev)
  {
    ev.setPreferredSize(128, 128);
  }

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g(ev.getGraphics());
    String userName(convert_to<String>(m_user.getName()));
    Rect rc(getClientBounds());
    Size textBox = g.measureString(userName);
    Size ellipse(8, 8);
    Brush brush1(hasMouse() ? Color(0, 200, 230): Color(0, 230, 255));
    Pen pen1(hasMouse() ? Color(0, 100, 255): Color(0, 200, 255));

    g.fillRoundRect(brush1, rc, ellipse);
    g.drawRoundRect(pen1, rc, ellipse);

    // Draw the first picture of the user
    if (m_image.isValid()) {
      g.drawImage(m_image,
    		  rc.x+rc.w/2 - m_image.getWidth()/2,
    		  rc.y+rc.h/2 - m_image.getHeight()/2);
    }

    // Draw name
    g.drawString(userName,
		 Color(0, 0, 0),
		 Point(rc.x+rc.w/2 - textBox.w/2,
		       rc.y+rc.h - 4 - textBox.h));
  }

  virtual void onMouseEnter(MouseEvent& ev)
  {
    Widget::onMouseEnter(ev);
    invalidate(false);
  }

  virtual void onMouseLeave(MouseEvent& ev)
  {
    Widget::onMouseLeave(ev);
    invalidate(false);
  }

  virtual void onMouseMove(MouseEvent& ev)
  {
    Widget::onMouseMove(ev);
  }
};

Navigator::Navigator(Widget* parent)
  : ScrollableWidget(parent)
  , m_imageList(System::getSmallImageList())
  , m_timer(250)
{
  setBgColor(Color::White);

  m_timer.Tick.connect(&Navigator::onTimerTick, this);
  m_timer.start();

  setDoubleBuffered(true);
}

Navigator::~Navigator()
{
  clearNavWidgets();
}

void Navigator::refresh()
{
  clearNavWidgets();

  // There are users in DB
  dao::General* generalDao = get_general_dao();
  if (generalDao) {
    dao::User userDao(generalDao);
    dao::Picture pictureDao(generalDao);
    dao::UserIteratorPtr userIter = userDao.getIterator();
    dto::User user;

    while (userIter->next(user)) {
      Image image;
      dao::PictureIteratorPtr pictureIter = pictureDao.getIterator(user.getId());
      dto::Picture picture;
      if (pictureIter->next(picture)) {
      	image = pictureDao.loadImage(picture.getId());
      }

      NavUser* widget = new NavUser(user, image, this);
      // the widget is saved in the list of children of "this" widget
    }
  }

  // // Draw files to process
  // int icon_w = m_imageList.getImageSize().w;
  // for (std::vector<String>::iterator
  // 	 it=m_filesToProcess.begin(); it!=m_filesToProcess.end(); ++it) {
  //   String path = *it;

  //   int imageIndex = System::getFileImageIndex(path, true);
  //   g.drawImageList(m_imageList, imageIndex, pt, ILD_NORMAL);
  //   g.drawString(Vaca::file_name(path), fgColor, pt.x + 4 + icon_w + 4, pt.y);

  //   Size sz = g.measureString(*it);
  //   pt.y += sz.h + 4;
  // }

  invalidate(true);
}

void Navigator::addImagesToProcess(const String& file)
{
  m_filesToProcess.push_back(file);
  refresh();
}

void Navigator::onPaint(PaintEvent& ev)
{
  // do nothing
}

void Navigator::onLayout(LayoutEvent& ev)
{
  ScrollableWidget::onLayout(ev);

  Rect rc(getClientBounds());

  ScreenGraphics g;		// Used to measure strings
  g.setFont(getFont());

  Point pt = Point(8, 8) - getScrollPoint();
  pt -= getScrollPoint();

  g.setFont(getFont());

  // There are users in DB
  WidgetList children = getChildren();
  WidgetList::iterator it = children.begin();
  WidgetList::iterator end = children.end();
  Size userBoxSize(128, 128);
  Size max(0, 0);

  for (; it != end; ++it) {
    NavUser* widget = (NavUser*)*it;
    const dto::User* user = widget->getUser();
    Rect userBox(pt, userBoxSize);

    widget->setBounds(userBox);
    max.w = max_value(max.w, userBox.x + userBox.w + 8);
    max.h = max_value(max.h, userBox.y + userBox.h + 8);

    pt.x += userBoxSize.w + 8;
    if (pt.x + userBoxSize.w > rc.w) {
      pt.x = 8 - getScrollPoint().x;
      pt.y += userBoxSize.h + 8;
    }
  }

  setFullSize(max);
}

void Navigator::onScroll(ScrollEvent& ev)
{
  // WidgetList children = getChildren();
  // WidgetList::iterator it = children.begin();
  // WidgetList::iterator end = children.end();
  // std::vector<Rect> rcs;

  // for (; it != end; ++it) {
  //   Widget* widget = (Widget*)*it;

  //   Rect rc = widget->getBounds();
  //   rc.offset(getScrollPoint());

  //   rcs.push_back(rc);
  // }

  ScrollableWidget::onScroll(ev);

  layout();

  // for (int c=0; it != end; ++it, ++c) {
  //   Widget* widget = (Widget*)*it;

  //   rcs[c].offset(-getScrollPoint());
  //   widget->setBounds(rcs[c]);
  // }

}

void Navigator::onTimerTick()
{
  // if (!m_filesToProcess.empty()) {
  //   // Process the next file
  //   std::vector<String>::iterator it = m_filesToProcess.begin();
  //   String file = *it;
  //   m_filesToProcess.erase(it);

  //   if (file_extension(file) == L"png") { // TODO add to_lower and to_upper functions
  //   }

  //   // if (FileSystem::isDirectory(file)) {
  //   //   FindFiles ff(file + L"\\*.*");
  //   //   while (ff.next()) {
  //   // 	m_filesToProcess.push_back(file + L"\\" + ff.getFileName());
  //   //   }
  //   // }

  //   invalidate(true);
  // }
}

void Navigator::clearNavWidgets()
{
  WidgetList children = getChildren();

  for (WidgetList::iterator
	 it = children.begin(); it != children.end(); ++it) {
    delete *it;
  }
}

