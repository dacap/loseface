==================================================
 Referencia del Lenguaje de Scripting de LoseFace
==================================================

:Author: David Capello
:Contact: davidcapello@gmail.com
:Date: Enero 2010

.. contents::

--------------
 Introducción
--------------

¿Qué es loseface.exe? Un programa de línea de comando para realizar
experimentos de reconocimiento facial utilizando eigenfaces y redes
neuronales artificiales (perceptrón multicapa específicamente).

LoseFace funciona con scripts, que son pequeños archivos de texto
indican qué operaciones realizar. Así usted tiene la posibilidad de
programar en un lenguaje específico (Lua) cualquier experimento que
desee llevar a cabo.

------------------
 Ejecutar Scripts
------------------

Para ejecutar algún script sólo debe arrastrar un archivo ``.lua`` hacia
el ejecutable ``loseface.exe``. También puede ejecutarlo por medio de la
línea de comando::

  C:\Archivos de Programa\LoseFace>loseface.exe mi_script.lua

A veces es útil ejecutar el script desde la línea de comando en caso
que éste imprima algo en la pantalla. Otra opción es crear un archivo
``.bat`` para redireccionar la salida a un archivo de texto, así ejecutando
el archivo ``.bat`` puede dejar el experimento ejecutándose en segundo
plano y al terminar ver la salida en un archivo que usted desee.
Ejemplo de un archivo ``ejemplo.bat``::

  rem ejemplo.lua recibe un argumento de entrada
  loseface.exe ejemplo.lua 1 > ejemplo-1.txt
  loseface.exe ejemplo.lua 2 > ejemplo-2.txt
  loseface.exe ejemplo.lua 3 > ejemplo-3.txt

-------------------------------
 Referencia del Lenguaje (Lua)
-------------------------------

LoseFace utiliza el lenguaje Lua para procesar scripts. Podrá
encontrar la referencia completa del lenguaje Lua en la siguiente
dirección:

  http://www.lua.org/manual/5.1/es/

Se recomienda revisar la sección 2 y 5 de dicho manual para conocer la
sintaxis y funciones utilitarias del lenguaje (e.j. para manejar archivos
de texto, tablas, funciones matemáticas, fechas, etc.).

En las siguientes secciones de este capítulo se hace un breve resumen
del lenguaje (basado en el manual original).

Comentarios
===========

Toda línea precedida por -- se considera un comentario, por lo tanto
el texto será descartado al ejecutar el script. Ejemplo::

   -- Aquí asignamos a la variable "a" el valor 5
   a = 5

Tipos
=====

Lua es *dinámicamente tipado*, lo que significa que cada variable puede
cambiar de tipo en *tiempo de ejecución*. Ejemplo::

  a = 2             -- la variable "a" es un número
  a = "hola"        -- ahora "a" es una cadena
  a = true          -- ahora "a" tiene el valor de verdad true

En Lua una variable puede ser de los siguientes tipos:
  - **nil**: Indica que la variable no tiene un valor (ejemplo: aún no fue inicializada).
  - *boolean*: Una variable puede tener el valor de verdad **true** o **false**.
    En una condición tanto **false** como **nil** hacen que la condición sea falsa.
    Cualquier otro caso es verdadera.
  - *number*: Todos los números son de punto flotante de doble precisión (a=2 o a=2.0 es lo mismo).
  - *string*: Las cadenas de caracteres son una sucesión de caracteres de 8 bits.
  - *function*: Una variable puede contener una función (útil para programación funcional).
  - *userdata*: Este tipo es un tipo de dato definido por el usuario, en este caso, el usuario
    de Lua es LoseFace. Cada objeto de LoseFace es un *userdata* específico que contiene diferentes
    funciones para accederlo.
  - *table*: Un arreglo asociativo o simplemente una lista de elementos.

Variables
=========

Expresiones
===========

Estructuras de Control
======================

if
--

for
---

while
-----

Funciones
=========

Objetos
=======

Espacio de Nombres (namespaces)
===============================

----------------------
 Objectos de LoseFace
----------------------

Para realizar experimentos debe conocer una serie de objetos (*userdata*)
que puede utilizar en sus scripts Lua. A continuación se da una referencia
de todos los objetos disponibles ().

Eigenfaces
==========

El objeto ``Eigenfaces`` se encuentra en el namespace ``img``.
Para crear un nuevo objeto ``Eigenfaces``::

  local eig = img.Eigenfaces()

Luego puede utilizar la variable ``eig`` como un objeto para acceder
a los distintos métodos.

eigenfaces:add_image
--------------------

::

  eigenfaces:add_image(images)

Parámetros:

- *images*: Un arreglo de imágenes a ser agregados para el posterior
  cálculo de eigenfaces.

Ejemplo::

  -- Cargamos desde el disco una serie de imágenes
  local img1 = img.Image()
  local img2 = img.Image()

  img1:load({ file="photo1.png" })
  img2:load({ file="photo2.png" })

  -- Creamos un arreglo de imágenes
  local images = { img1, img2 }

  -- Creamos el objeto Eigenfaces y le agregamos las imágenes
  local eig = img.Eigenfaces()
  eig:add_image(images)

eigenface:calculate_eigenfaces
------------------------------

::

  eigenfaces:calculate_eigenfaces({ components=number })
  eigenfaces:calculate_eigenfaces({ variance=number })

Calcula las eigenfaces para luego proder proyectar cualquier imagen al
eigenspace.

Parámetros:

- *components*: Cantidad de componentes por eigenface. De todos los
  eigenvalores/eigenvectores posibles a utilizar, sólo utilizaremos
  esta cantidad especificada.

- *variance*: En cambio de especificar la cantidad de componentes
  específica, con este parámetros podemos indicar qué nivel de varianza
  de información queremos abarcar. Así, se utilizarán tantos
  eigenvalores/eigenvectores como varianza se necesite.

Valor de retorno:

- La cantidad de componentes de eigenfaces utilizados. Este valor
  tiene sentido cuando la función se utiliza con el parámetro *variance*.

Ejemplo: Calcular las eigenfaces para un nivel de varianza de 80%::

  local num_eigenfaces = eig:calculate_eigenfaces({ variance=0.8 })

eigenface:eigenvalues_count
---------------------------

eigenface:project_in_eigenspace
-------------------------------

eigenface:reserve
-----------------

eigenface:save
--------------

Image
=====

image:create
------------

image:draw
----------

image:get
---------

image:height
------------

image:load
----------

image:save
----------

image:width
-----------

Normalizer
==========

normalizer:normalize
--------------------

PatternSet
==========

patternset:add_pattern
----------------------

patternset:clone
----------------

patternset:merge
----------------

patternset:save
---------------

patternset:set_output
---------------------

patternset:shuffle
------------------

patternset:split
----------------

Mlp
===

mlp:clone
---------

mlp:init
--------

mlp:load
--------

mlp:mse
-------

mlp:recall
----------

mlp:save
--------

mlp:test
--------

mlp:train
---------

MlpArray
========

mlparray:load
-------------

mlparray:recall
---------------

mlparray:save
-------------
