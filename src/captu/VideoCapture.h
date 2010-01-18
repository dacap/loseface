// Copyright (C) 2008-2010 David Capello. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE.txt file.

#ifndef LOSEFACE_CAPTU_VIDEOCAPTURE_H
#define LOSEFACE_CAPTU_VIDEOCAPTURE_H

#include "CImg.h"

#include <vector>
#include <string>

namespace captu {

  class VideoControls;

  /// Interface para obtener imágenes desde un dispositivo de captura
  /// de video (ej. una webcam). Esta interface es implementada
  /// dependiendo de las capacidades de cada sistema operativo.
  /// 
  class VideoCapture
  {
  public:
    virtual ~VideoCapture() { };

    /// Inicia el driver de captura. Esto significa chequear que el
    /// dispositivo esté conectado, que pueda ser utilizado para
    /// capturar imágenes, y reservar el recurso para uso propio de la
    /// aplicación.
    /// 
    virtual bool initDriver() = 0;

    /// Libera el dispositivo así otros programas pueden usarlo.
    /// 
    virtual void exitDriver() = 0;

    /// Devuelve verdadero si el sistema operativo ya ofrece una
    /// ventana para mostrar la previsualización de video.
    /// 
    /// Esto es específico de Win32, ya que provee de una ventana
    /// (HWND) que sirve para comunicarse con el dispositivo de captura
    /// de video.
    /// 
    /// @return Verdadero si la plataforma ofrece de forma nativa la
    /// ventana de previsualización.
    /// 
    virtual bool isPreviewWindowAvailable() = 0;

    virtual bool createPreviewWindow(void* parentWindowHandle) = 0;
    virtual void destroyPreviewWindow() = 0;
    virtual void startPreviewInWindow() = 0;
    virtual void stopPreviewInWindow() = 0;

    virtual void grabFrame() = 0;
    virtual bool getImage(cimg_library::CImg<unsigned char>& img) = 0;

    /// Devuelve verdadero si el sistema operativo soporta de forma
    /// nativa el diálogo de configuración de formato de video (tamaño
    /// y formato de imagen).
    /// 
    /// Si no lo soporta de forma nativa, se deberá utilizar el método
    /// #getVideoControls para obtener la interfaz que permite
    /// manipular los parámetros de forma manual.
    /// 
    /// @ref showFormatDialog
    /// 
    virtual bool hasFormatDialog() = 0;

    /// Devuelve verdadero si el sistema operativo soporta de forma
    /// nativa el diálogo de configuración de parámetros de captura
    /// (brillo, contraste, etc.).
    /// 
    /// Si no lo soporta de forma nativa, se deberá utilizar el método
    /// #getVideoControls para obtener la interfaz que permite
    /// manipular los parámetros de forma manual.
    /// 
    /// @see showParamsDialog
    /// 
    virtual bool hasParamsDialog() = 0;

    /// Muestra el diálogo para configurar el formato de captura (si el
    /// sistema operativo lo soporta de forma nativa).
    /// 
    /// @ref hasFormatDialog
    /// 
    virtual void showFormatDialog() = 0;

    /// Muestra el diálogo para configurar los parámetros de captura
    /// (si el sistema operativo lo soporta de forma nativa).
    /// 
    /// @see hasParamsDialog
    /// 
    virtual void showParamsDialog() = 0;

    /// Devuelve una interfaz para manipular los parámetros de captura
    /// de manualmente.
    /// 
    /// @return NULL si el sistema operativo ya soporta la modificación
    /// de parámetros de captura de forma nativa.
    /// 
    virtual VideoControls* getVideoControls() = 0;

    /// Devuelve una implementación de VideoCapture. Debe estar
    /// definido una vez para cada plataforma, devolviendo un puntero a
    /// la implementación correspondiente a la plataforma.
    /// 
    static VideoCapture* create();
  };

  /// Clase para modificar el formato y los parámetros de captura de
  /// video.
  /// 
  /// Debe ser implementada por las plataformas que no soporten de
  /// forma nativa las ventanas de configuración.
  /// 
  /// @see VideoCapture#getVideoControls
  /// 
  class VideoControls
  {
  public:
    virtual ~VideoControls() { }

    virtual void getAvailableFormats(std::vector<std::string>& lst) = 0;
    virtual void getAvailableResolutions(std::vector<std::string>& lst) = 0;

    virtual void setFormat(const std::string& fmt) = 0;
    virtual void setResolution(const std::string& res) = 0;

    virtual unsigned char getBrightness() = 0;
    virtual unsigned char getContrast() = 0;
    virtual unsigned char getColors() = 0;

    virtual void setBrightness(unsigned char value) = 0;
    virtual void setContrast(unsigned char value) = 0;
    virtual void setColors(unsigned char value) = 0;

    virtual bool getFlipRB() = 0;
    virtual void setFlipRB(bool flip) = 0;
  };

} // namespace captu

#endif // LOSEFACE_CAPTU_VIDEOCAPTURE_H
