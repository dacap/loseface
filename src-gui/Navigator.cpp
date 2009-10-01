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
#include "NewFuncs.h"
#include <Vaca/Vaca.h>

Navigator::Navigator(Widget* parent)
  : Widget(parent, 
	   Widget::Styles::Default | 
	   Widget::Styles::ClientEdge)
  , m_imageList(System::getSmallImageList())
  , m_timer(250)
{
  setBgColor(Color::White);

  m_timer.Tick.connect(&Navigator::onTimerTick, this);
  m_timer.start();

  setDoubleBuffered(true);
}

void Navigator::addImagesToProcess(const String& file)
{
  m_filesToProcess.push_back(file);
  invalidate(true);
}

void Navigator::onPaint(PaintEvent& ev)
{
  Graphics& g(ev.getGraphics());
  Rect rc(getClientBounds());
  Brush bgBrush(getBgColor());
  Color fgColor(getFgColor());
  Pen fgPen(fgColor);

  Point pt(8, 8);
  g.setFont(getFont());

  // No images
  if (m_filesToProcess.empty()) {
    g.drawString(L"No hay imágenes en la base de datos", fgColor, pt);
    return;
  }

  // There are users in DB
  // if () {
    // Draw a separator
    //g.drawLine(fgPen, );
  // }

  // Draw files to process
  int icon_w = m_imageList.getImageSize().w;
  for (std::vector<String>::iterator
	 it=m_filesToProcess.begin(); it!=m_filesToProcess.end(); ++it) {
    String path = *it;

    int imageIndex = System::getFileImageIndex(path, true);
    g.drawImageList(m_imageList, imageIndex, pt, ILD_NORMAL);
    g.drawString(Vaca::file_name(path), fgColor, pt.x + 4 + icon_w + 4, pt.y);

    Size sz = g.measureString(*it);
    pt.y += sz.h + 4;
  }
}

void Navigator::onTimerTick()
{
  if (!m_filesToProcess.empty()) {
    // Process the next file
    std::vector<String>::iterator it = m_filesToProcess.begin();
    String file = *it;
    m_filesToProcess.erase(it);

    if (System2::isDirectory(file)) {
      FindFiles ff(file + L"\\*.*");
      while (ff.next()) {
	m_filesToProcess.push_back(file + L"\\" + ff.getFileName());
      }
    }

    invalidate(true);
  }
}

