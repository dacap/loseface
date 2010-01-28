-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

-- Get an ordered list of folders s1, s2, s3, ..., s40
folders = {}
for file in lfs.dir("cidisinc_faces") do
  num = string.match(file, "^([0-9]+)$")
  if num then
    table.insert(folders, { tonumber(num), file })
  end
end
table.sort(folders, function (a, b) return a[1] < b[1] end )

-- Add the list of images of each folder
for i = 1,#folders do
  images_file = {}
  for file in lfs.dir("cidisinc_faces/"..folders[i][2]) do
    num = string.match(file, "^(.+)\.png")
    if num then
      table.insert(images_file, { num, file })
    end
  end
  table.sort(images_file, function (a, b) return a[1] < b[1] end )
  table.insert(folders[i], images_file)
end

-- Load all images, get the face and save it
max_w = 0
max_h = 0
for i = 1,#folders do
  print("Loading subject "..i.."...")
  io.flush()

  w = 0
  h = 0
  images = {}
  for j = 1,#folders[i][3] do
    file = "cidisinc_faces/"..folders[i][2].."/"..folders[i][3][j][2]

    print(string.format("%d %s", i, file))
  
    image = img.Image()
    image:load(file)
    table.insert(images, image)

    w = w + image:width()
    h = math.max(h, image:height())
  end
  table.insert(folders[i], images)

  max_w = math.max(max_w, w)
  max_h = max_h + h

  print("  " .. #folders[i][3] .. " images loaded")
  io.flush()
end

big_image = img.Image()
big_image:create(max_w, max_h)
y = 0
for i = 1,#folders do
  x = 0
  h = 0
  for j = 1,#folders[i][4] do
    image = folders[i][4][j]
    big_image:draw(image, x, y)
    x = x + image:width()
    h = math.max(h, image:height())
  end
  y = y + h
end
big_image:save("cidisinc_big_picture.png")
