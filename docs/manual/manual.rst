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

Se recomienda revisar la sección `2 <http://www.lua.org/manual/5.1/es/manual.html#2>`_
y `5 <http://www.lua.org/manual/5.1/es/manual.html#5>`_ de dicho manual para
conocer la sintaxis y funciones utilitarias del lenguaje (e.j. para manejar archivos
de texto, tablas, funciones matemáticas, fechas, etc.).

----------------------
 Objectos de LoseFace
----------------------

Para realizar experimentos LoseFace le ofrece una serie de objetos (*userdata*)
que puede utilizar en sus scripts Lua. A continuación se da una referencia
de los objetos disponibles.

Eigenfaces
==========

Representa un conjunto de eigenfaces, útil para proyectar imágenes en
al eigenspace.

El objeto ``Eigenfaces`` se encuentra en el namespace ``img``.
Para crear un nuevo objeto ``Eigenfaces``::

  local eig = img.Eigenfaces()

Luego puede utilizar la variable ``eig`` como una instancia de
la clase ``Eigenfaces`` accediendo a los distintos métodos. Ejemplo::

  eig:save("eigenfaces.dat")

eigenfaces:add_image
--------------------

::

  eigenfaces:add_image(image1, image2, ...)

Parámetros:

- *image1*, *image2*, etc.: Imágenes a ser agregadas para el posterior
  cálculo de eigenfaces.

Ejemplo::

  -- Cargamos una serie de imágenes
  local img1 = img.Image()
  local img2 = img.Image()

  img1:load("photo1.png")
  img2:load("photo2.png")

  -- Creamos el objeto Eigenfaces y le agregamos las imágenes
  local eig = img.Eigenfaces()
  eig:add_image(img1, img2)

eigenfaces:calculate_eigenfaces
-------------------------------

::

  number = eigenfaces:calculate_eigenfaces({ components=number })
  number = eigenfaces:calculate_eigenfaces({ variance=number })

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

eigenfaces:eigenvalues_count
----------------------------

::

  number = eigenfaces:eigenvalues_count()

Devuelve la cantidad de eigenvalores disponibles. Este valor se puede obtener
una vez llamada la función eigenfaces:calculate_eigenfaces.

Valor de retorno:

- La cantidad de eigenvalores calculados.

eigenfaces:project_in_eigenspace
--------------------------------

::

  outputs = eigenfaces:project_in_eigenspace(images)

Proyecta cada imagen especificada en el eigenspace.

Parámetros:

- *images*: Un arreglo de imágenes a proyectar en el eigenspace.

Valor de retorno:

- *outputs*: Un arreglo donde cada elemento corresponde a un vector
  en el eigenspace.

Ejemplo::

  local outputs = eig:project_in_eigenspace({ img1, img2 })
  local img1_proyectada = outputs[0]
  local img2_proyectada = outputs[1]

eigenfaces:reserve
------------------

::

  eigenfaces:reserve(size)

Reserva con anterioridad una matriz del tamaño suficiente para
una cierta cantidad de imágenes que se agregarán con la
función `eigenfaces:add_image`_. Se recomienda utilizar
esta función para mejorar el rendimiento de `eigenfaces:add_image`_.

Parámetros:

- *size*: Cantidad de imágenes que se agregarán luego con `eigenfaces:add_image`_.

Ejemplo::

  local eig = img.Eigenfaces()
  eig:reserve(10)
  for i = 1,10 do
    local img = img.Image()
    img:load("photo"..i..".png")
    eig:add_image(img)
  end

eigenfaces:save
---------------

::

  eigenfaces:save(filename)

Guarda la información de eigenfaces en el archivo especificado.

Parámetros:

- *filename*: Nombre del archivo donde dejar la información de eigenfaces.

Ejemplo::

  eig:save("patterns.txt")

Image
=====

Representa una imagen (vacía, creada en memoria, o leída desde un archivo).

El objeto ``Image`` se encuentra en el namespace ``img``. Para crear
una nueva imagen (vacía)::

  local imagen = img.Image()

Una vez que tiene una instancia de ``Image`` (en este caso la variable ``imagen``),
puede llamar algunos de sus métodos.

image:create
------------

::

  image:create(width, height)

Cambia el tamaño de la imagen al ancho y alto especificado. El
contenido de la imagen ``image`` es descartado y una nueva imagen
con fondo negro es creada.

Parámetros:

- *width*: Ancho de la imagen.

- *height*: Alto de la imagen.

Ejemplo::

  local a = img.Image()
  a:create(640, 480)

image:draw
----------

::

  image:draw(sprite, x, y)

Dibuja la imagen *sprite* en la posición especificada (*x*, *y*).
El sprite es dibujado sobre la instancia *image*.

Parámetros:

- *sprite*: Imagen a dibujar sobre la instancia *image*.

- *x*: Posición x (columna) donde debemos comenzar a dibujar la imagen *sprite*.

- *y*: Posición y (fila) donde debemos comenzar a dibujar la imagen *sprite*.

Ejemplo::

  local spr = img.Image()
  spr:create(32, 32)
  
  local image4x4 = img.Image()
  image4x4:create(64, 64)

  image4x4:draw(spr,  0,  0)
  image4x4:draw(spr, 32,  0)
  image4x4:draw(spr,  0, 32)
  image4x4:draw(spr, 32, 32)

image:get_face
--------------

::

  img_face = image:get_face()

TODO documentar

image:height
------------

::

  height = image:height()

Devuelve el alto de la imagen en pixeles (un número entero).

image:load
----------

::

  image:load(filename)

Guarda la imagen en el archivo especificado.

image:save
----------

::

  image:save(filename)

Carga una imagen desde un archivo.

image:width
-----------

::

  width = image:width()

Devuelve el ancho de la imagen en pixeles (un número entero).

Normalizer
==========

Objeto para normalizar patrones de entrenamiento.

El objeto ``Normalizer`` se encuentra en el namespace ``ann``.

    local normalizer = ann.Normalizer(set)

Parámetros:

- *set*: El conjunto de patrones de entrenamiento (un PatternSet_). Estos patrones
  son utilizados para calcular los valores máximos y mínimos de los valores de
  entrada. Luego puede normalizarse cualquier otro patrón utilizando la función
  `normalizer:normalize`_.

normalizer:normalize
--------------------

::

  normalizer:normalize(set1, set2, ...)

Normaliza las entradas de cada uno de los PatternSet_ indicados en los
parámetros.

Ejemplo::

    local train_set = ann.PatternSet()
    local test_set = ann.PatternSet()

    local n = ann.Normalizer(train_set)
    n:normalize(train_set, test_set)

PatternSet
==========

Representa un conjunto de patrones (ya sean de entrenamiento o prueba)
que pueden ser utilizados en un Mlp_.

Formas de crear un PatternSet::

   set = ann.PatternSet()

Crea un conjunto de patrones vacío.

También podemos cargar patrones desde un archivo de texto::

   set = ann.PatternSet({ inputs=number, outputs=number, file=string })

Cada línea del archivo (*file*) debe contener tantos números
(enteros o decimales) como se especifiquen en la cantidad de
entradas (*inputs*) y salidas (*outputs*).

Ejemplo::

   local set = ann.PatternSet({ inputs=2, outputs=1, file="xor.txt" })

Conteniendo el archivo ``xor.txt`` las siguientes líneas::

   0 0 0
   0 1 1
   1 0 1
   1 1 0

patternset:add_pattern
----------------------

::

  patternset:add_pattern(inputs, outputs)

Parámetros:

- *inputs*: Vector de entradas. Esto es una tabla de Lua con números enteros.

- *outputs*: Vector de salidas.

Ejemplo::

  -- Patrones para una compuerta OR
  local ps = ann.PatternSet()
  ps:add_pattern({ 0, 0 }, { 0 })
  ps:add_pattern({ 0, 1 }, { 1 })
  ps:add_pattern({ 1, 0 }, { 1 })
  ps:add_pattern({ 1, 1 }, { 1 })

patternset:clone
----------------

::

  other_patternset = patternset:clone()

Devuelve una copia de todos los patrones del conjunto. Las copias
(y los patrones que contienen) son independientes entre sí, lo que
significa que modificando cualquier de los dos PatternSet_ (tanto
el original como el clon) no influirá en los patrones del otro.

patternset:merge
----------------

::

  patternset:merge(set1, set2, set3, ...)

Agrega al conjunto de patrones todos los patrones especificados
en los argumentos. Cada *set1*, *set2*, etc. es un PatternSet_.

Los patrones originales no son modificados, y las copias son
completamente independientes a las originales.

patternset:save
---------------

::

  patternset:save(filename)

Guarda todo el conjunto de patrones en el archivo especificado.

Parámetros:

- *filename*: Nombre del archivo donde guardar los patrones.

patternset:set_output
---------------------

::

  patternset:set_output(output_vector)

Cambia el vector de salida de todos los patrones de entrenamiento en
el conjunto.

Parámetros:

- *output_vector*: Vector de salida.

Ejemplo:

  local ps1 = ann.PatternSet()
  ps1:add_pattern({ 0 }, { 0, 0 })
  ps1:add_pattern({ 1 }, { 1, 0 })

  local ps2 = ps1:clone()
  ps2:set_output({ 1, 1 })

patternset:shuffle
------------------

::

  patternset:shuffle()

Reordena aleatoriamente todos los patrones del conjunto. Aunque
decimos que un PatternSet_ es "un conjunto", en realidad es una lista
ordenada de patrones. El orden de la lista es el mismo orden con el
cual los patrones son utilizados, por ejemplo al ser presentados en el
entrenamiento de una red neuronal artificial.

patternset:split
----------------

::

  patternset:split_by_percentage({ percentage1, percentage2... })
  patternset:split_by_output({ output1, output2, output3... })

Divide el conjunto de patrones en subconjuntos según el criterio
especificado.

Ejemplo::
  local subsets1 = all_patterns:split_by_percentage({ 20, 60, 20 })
  local subsets2 = all_patterns:split_by_output({ 1, 2, 3 })

Mlp
===

Representa una red neuronal artificial de tipo perceptrón multicapa.
El modelo tiene tres capas (neuronas de entradas, capa oculta, y capa de salida).
Cada neurona de la capa oculta y la de salida tiene un parámetro de *bias*.

Al crear un nuevo modelo red MLP debe especificar la cantidad de neuronas en cada capa::

  local mlp = ann.Mlp({ inputs=number, hiddens=number, outputs=number })

Donde *inputs* es la cantidad de entradas del modelo, *hiddens* la cantidad
de neuronas en la capa oculta, y *outputs* es la cantidad de salidas.

Ejemplo::

  local logic_gate = ann.Mlp({ inputs=2, hiddens=2, outputs=1 })

mlp:clone
---------

::

  copy = mlp:clone()

Devuelve una copia del modelo MLP.

mlp:init
--------

::

  mlp:init({ min=number, max=number })

Inicializa los pesos y los bias de la red MLP aleatoriamente en el rango
especificado entre *min* y *max*.

Ejemplo::

  mlp:init({ min=-1.0, max=1.0 })

mlp:load
--------

::

  mlp:load(filename)

Carga el modelo MLP desde el archivo *filename* especificado.

mlp:mse
-------

::

  mlp:mse(set)

Calcula el MSE de un conjunto de patrones dados.

Parámetros:

- *set*: Un conjunto de patrones PatternSet_ para ser probados en el
  MLP y calcular su MSE correspondiente.

mlp:recall
----------

::

  local outputs = mlp:recall(set)

Ejecuta la red neuronal con las entradas de cada patrón del conjunto
especificado. Devuelve un vector con cada salida de la red.

Parámetros:

- *set*: Un PatternSet_ que contiene los patrones a ser probados en la red.

mlp:save
--------

::

  mlp:save(filename)

Guarda el modelo MLP en el archivo *filename* especificado.

mlp:train
---------

::

  mlp:train({ set=PatternSet,
              epochs=number,
              learning_rate=number,
              momentum=number,
              shuffle=number,
              goal=ann.LAST | ann.BESTMSE,
              goal_mse=number,
              early_stopping={ set=PatternSet, iterations=number } }

Entrena la red neuronal por un número de épocas especificado.

Parámetros:

- *set*: Conjunto de patrones de entrenamiento (un PatternSet_).

- *epochs*: Cantidad de épocas a iterar. En cada época, a la red neuronal
  se le presentan todos los patrones indicados en *set*.

- *goal_mse*: Indica un nivel de MSE al que queramos llegar en el entrenamiento.
  El MSE es calculado en base a los mismos patrones de entrenamiento (argumento *set*).

- *learning_rate*: Tasa de aprendizaje a utilizar en cada iteración.

- *momentum*: Valor de momentum. Al ajustar los pesos del modelo MLP (pesos
  en las conexiones entre neuronas y bias), cada "delta" es multiplicado
  por el momentum.

- *shuffle*: Indica cada cuántas épocas queremos "mezclar" (cambiar el orden de)
  todos los patrones. Si es igual a cero, entonces los patrones no se mezclan
  y son mostrados en el mismo orden en todas las épocas.

- *goal*: Indica con qué red nos quedamos luego del entrenamiento:

  - ann.LAST: La red obtenida en la última época.

  - ann.BESTMSE: La red con menor MSE (calculado a partir del *set*, es decir,
    los mismos patrones de entrenamiento).

- *early_stopping*: Indica alguna configuración extra para frenar el entrenamiento
  antes de lo esperado:

  - *set*: Un conjunto de patrones de validación, para ver si el MSE de este
    conjunto empeora época tras época.

  - *iterations*: Cantidad de "malas épocas" que deben pasar para frenar
    el entrenamiento por completo (por omisión este valor es igual a 5).
    Por "mala época" entendemos que el MSE, del conjunto de patrones
    especificados para el *early_stopping*, empeoró con respecto a la
    anterior época.

Existen tres formas de utilizar esta función de entrenamiento:

- Sin especificar *epochs* y *goal_mse*, se entrenará a la red sólo una época.

- Especificando *epochs*, con lo cual se entrenará una cantidad fija de épocas.

- Especificando *goal_mse*, con lo cual se entrenará hasta llegar al nivel de MSE dado.
  Debe tener en cuenta que si el entrenamiento no converge, el programa puede quedar
  en un bucle infinito. Se recomienda utilizar la siguiente modalidad de uso.
  
- Especificando *epochs* y *goal_mse*, con lo cual se intentará llegar al
  valor de MSE indicado, en un máximo de épocas dado.

MlpArray
========

Representa un arreglo de redes MLP.

El objeto ``MlpArray`` se encuentra en el namespace ``ann``. Ejemplo
para crear un nuevo arreglo de redes MLP::
 
  local mlps = { mlp1, mlp2, mlp3 }
  local arreglo = ann.MlpArray(mlps)

Siendo *mlp1*, *mlp2* y *mlp3* tres objetos Mlp_.

mlparray:load
-------------

::

  mlparray:load(filename)

Carga el arreglo de MLPs desde el archivo *filename* especificado.

mlparray:recall
---------------

::

  local outputs = mlparray:recall(set)

Ejecuta el arreglo de redes con las entradas de cada patrón del conjunto
especificado. Devuelve un vector con cada salida del arreglo.

Parámetros:

- *set*: Un PatternSet_ que contiene los patrones a ser probados en el arreglo.

mlparray:save
-------------

::

  mlparray:save(filename)

Guarda el arreglo de MLPs en el archivo *filename* especificado.
