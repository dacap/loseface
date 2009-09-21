-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

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
big_image:create({ width=max_w, height=max_h })
y = 0
for i = 1,#images_matrix do
  x = 0
  h = 0
  for j = 1,#images_matrix[i] do
    image = images_matrix[i][j]
    big_image:draw({ sprite=image, x=x, y=y })
    x = x + image:width()
    h = math.max(h, image:height())
  end
  y = y + h
end
big_image:save({ file="cidisinc_images_matrix.png" })
