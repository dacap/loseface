==================================================
 Referencia del Lenguaje de Scripting de LoseFace
==================================================

:Author: David Capello
:Contact: davidcapello@gmail.com
:Date: Enero 2010

.. contents::

------------------
 Ejecutar Scripts
------------------

Para ejecutar algún script sólo debe arrastrar un archivo ``.lua`` hacia
el ejecutable ``loseface.exe``. También puede ejecutarlo por medio de la
línea de comando::

  C:\Archivos de Programa\LoseFace>loseface.exe mi_script.lua

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

---------------------------------
 Objectos de LoseFace (userdata)
---------------------------------

Eigenfaces
==========

eigenface:add_image
-------------------

eigenface:calculate_eigenfaces
------------------------------

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
