-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

function load_orl_images_matrix()
  -- Get an ordered list of folders s1, s2, s3, ..., s40 (subjects)
  local folders = {}
  for file in lfs.dir("orl_faces") do
    local num = string.match(file, "^s([0-9]+)")
    if num then
      table.insert(folders, { tonumber(num), file })
    end
  end
  table.sort(folders, function (a, b) return a[1] < b[1] end )

  -- Add the list of pictures of each folder/subject
  for i = 1,#folders do
    local images_files = {}
    for file in lfs.dir("orl_faces/"..folders[i][2]) do
      num = string.match(file, "^([0-9]+)\.pgm")
      if num then
	table.insert(images_files, { tonumber(num), file })
      end
    end
    table.sort(images_files, function (a, b) return a[1] < b[1] end )
    table.insert(folders[i], images_files)
  end

  -- Load all images
  local images_matrix = {}
  for i = 1,#folders do
    local images = {}
    for j = 1,#folders[i][3] do
      file = "orl_faces/"..folders[i][2].."/"..folders[i][3][j][2]
      
      print("Loading "..file.."...")
      io.flush()
      
      image = img.Image()
      image:load({ file=file })
      table.insert(images, image)
    end
    table.insert(images_matrix, images)
  end

  return images_matrix
end
