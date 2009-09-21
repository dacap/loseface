========================================
General Scripts
========================================

calculate_from_variance.lua
  Auxiliary function to calculate the number of eigenfaces
  needed to maintain a certain level of variance with a
  given images-matrix.

  An images-matrix is a Lua array of array where:
    images_matrix = { subject1, subject2, ..., subjectN }
    subject1 = { image1, image2, ..., imageM }
    ...

========================================
ORL Faces DB Scripts
========================================

orl_images_matrix.lua
  Creates an images-matrix from the ORL faces database. The
  images must be located in a "orl_faces" folder.

orl_patterns.lua
  Converts the ORL images to points in an eigenspace. These
  points (patterns) can be used as input for a MLP neural net.


========================================
CIDIsinc DB Scripts
========================================

cidisinc_get_faces.lua
  This script crops all the faces from .ppm files in cidisinc_faces/
  directory.  Each face-file is saved with the same original file-name
  but with "-face" suffix and in PNG format

cidisinc_images_matrix.lua
  Creates an images-matrix from selected pictures in
  cidisinc database (located in cidisinc_faces/ directory).

cidisinc_render_images_matrix.lua
  Renders the selected faces cidisinc images-matrix. The result is saved
  in the cidisinc_images_matrix.png file.

cidisinc_render_big_picture.lua
  Renders the whole faces database saving the result in
  cidisinc_big_picture.png file.


========================================
MLP Scripts
========================================

TODO write
