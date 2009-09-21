-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

function get_faces_of_cidisinc_images()
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
      image:load({ file=file })

      print("  Getting the face...")
      io.flush()

      face = image:get_face()
      file = string.sub(file, 1, -5) .. "-face.png"

      print("  Saving "..file.."...")
      io.flush()
      face:save({ file=file })
    end
  end
end

get_faces_of_cidisinc_images()
