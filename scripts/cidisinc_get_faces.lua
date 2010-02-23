-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
--
-- Description:
--   This script crops all the faces from .ppm files in cidisinc_faces/
--   directory.  Each face-file is saved with the same original file-name
--   but with "-face" suffix and in PNG format
--
-- Usage:
--   You can use this script directly running the following command:
--
--     loseface cidisinc_get_faces.lua
--

-- Get an ordered list of folders (001, 002, etc.)
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
    num = string.match(file, "^([0-9_]+)\.ppm$")
    if num then
table.insert(images_file, { num, file })
    end
  end
  table.sort(images_file, function (a, b) return a[1] < b[1] end )
  table.insert(folders[i], images_file)
end

-- Load each image, crop the face and save the result
for i = 1,#folders do
  for j = 1,#folders[i][3] do
    file = "cidisinc_faces/"..folders[i][2].."/"..folders[i][3][j][2]

    print("Loading "..file.."...")
    io.flush()
    
    image = img.Image()
    image:load(file)

    print("  Getting the face...")
    io.flush()

    face = image:get_face()
    file = string.sub(file, 1, -5) .. "-face.png"

    print("  Saving "..file.."...")
    io.flush()
    face:save(file)
  end
end
