-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
-- 
-- Description:
--   Creates an images-matrix from the CIDISINC faces database. The
--   images must be located in the "cidisinc_faces" folder.


-- load_cidisinc_images_matrix:
--   Returns an images-matrix reading a selected group of faces
--   of the CIDISINC database.
--
function load_cidisinc_images_matrix()
  local selected_images =
    { { "cidisinc_faces/001/080829_172631-face.png",
	"cidisinc_faces/001/080829_172638-face.png",
	"cidisinc_faces/001/080829_172655-face.png",
	"cidisinc_faces/001/080829_172707-face.png",
	"cidisinc_faces/001/080829_172722-face.png",
	"cidisinc_faces/001/080829_172853-face.png",
	"cidisinc_faces/001/080916_141205-face.png",
	"cidisinc_faces/001/080916_141227-face.png" },
      { "cidisinc_faces/002/080829_173208-face.png",
	"cidisinc_faces/002/080829_173218-face.png",
	"cidisinc_faces/002/080829_173225-face.png",
	"cidisinc_faces/002/080829_173238-face.png",
	"cidisinc_faces/002/080829_173243-face.png",
	"cidisinc_faces/002/080829_173251-face.png",
	"cidisinc_faces/002/080829_173303-face.png",
	"cidisinc_faces/002/080829_173318-face.png" },
      { "cidisinc_faces/003/080901_111445-face.png",
	"cidisinc_faces/003/080901_111452-face.png",
	"cidisinc_faces/003/080901_111505-face.png",
	"cidisinc_faces/003/080902_150317-face.png",
	"cidisinc_faces/003/080902_150322-face.png",
	"cidisinc_faces/003/080904_160435-face.png",
	"cidisinc_faces/003/080904_160444-face.png",
	"cidisinc_faces/003/080905_154013-face.png" },
      { "cidisinc_faces/004/080901_111621-face.png",
	"cidisinc_faces/004/080905_153546-face.png",
	"cidisinc_faces/004/080905_153550-face.png",
	"cidisinc_faces/004/080909_114906-face.png",
	"cidisinc_faces/004/080909_114916-face.png",
	"cidisinc_faces/004/080909_114926-face.png",
	"cidisinc_faces/004/080916_141950-face.png",
	"cidisinc_faces/004/080929_102103-face.png" },
      { "cidisinc_faces/005/080901_111406-face.png",
	"cidisinc_faces/005/081002_184033-face.png",
	"cidisinc_faces/005/081002_184034-face.png",
	"cidisinc_faces/005/081002_184039-face.png",
	"cidisinc_faces/005/081002_184044-face.png",
	"cidisinc_faces/005/081002_184050-face.png",
	"cidisinc_faces/005/081002_184106-face.png",
	"cidisinc_faces/005/081002_184107-face.png" },
      { "cidisinc_faces/006/080901_123038-face.png",
	"cidisinc_faces/006/080901_123053-face.png",
	"cidisinc_faces/006/080901_123107-face.png",
	"cidisinc_faces/006/080916_141749-face.png",
	"cidisinc_faces/006/080916_141754-face.png",
	"cidisinc_faces/006/080916_141757-face.png",
	"cidisinc_faces/006/081002_140046-face.png",
	"cidisinc_faces/006/081002_140059-face.png" },
      { "cidisinc_faces/012/080829_170034-face.png",
	"cidisinc_faces/012/080829_170124-face.png",
	"cidisinc_faces/012/080829_170130-face.png",
	"cidisinc_faces/012/080829_170136-face.png",
	"cidisinc_faces/012/080829_170148-face.png",
	"cidisinc_faces/012/080829_170204-face.png",
	"cidisinc_faces/012/080829_170219-face.png",
	"cidisinc_faces/012/080829_170226-face.png" } }

  -- Load all images
  local images_matrix = {}
  for i = 1,#selected_images do
    local images = {}
    for j = 1,#selected_images[i] do
      file = selected_images[i][j]

      print("Loading "..file.."...")
      io.flush()
      
      image = img.Image()
      image:load(file)
      table.insert(images, image)
    end
    table.insert(images_matrix, images)
  end

  return images_matrix
end
