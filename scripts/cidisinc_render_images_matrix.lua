-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
--
-- Description:
--   Renders the selected faces CIDISINC images-matrix. The result is saved
--   in "cidisinc_images_matrix.png" file.
--
-- Usage:
--   You can use this script directly running the following command:
--
--     loseface cidisinc_render_images_matrix.lua
--

dofile("cidisinc_images_matrix.lua")

images_matrix = load_cidisinc_images_matrix()

-- Calculate the required image-size to draw the whole images_matrix
max_w = 0
max_h = 0
for i = 1,#images_matrix do
  w = 0
  h = 0
  for j = 1,#images_matrix[i] do
    image = images_matrix[i][j]
    w = w + image:width()
    h = math.max(h, image:height())
  end
  max_w = math.max(max_w, w)
  max_h = max_h + h
end

-- create a big image with the required size and draw the whole
-- images_matrix on it
big_image = img.Image()
big_image:create(max_w, max_h)
y = 0
for i = 1,#images_matrix do
  x = 0
  h = 0
  for j = 1,#images_matrix[i] do
    image = images_matrix[i][j]
    big_image:draw(image, x, y)
    x = x + image:width()
    h = math.max(h, image:height())
  end
  y = y + h
end
big_image:save("cidisinc_images_matrix.png")
