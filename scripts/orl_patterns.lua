-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

-- Get an ordered list of folders s1, s2, s3, ..., s40
folders = {}
for file in lfs.dir("orl_faces") do
  num = string.match(file, "^s([0-9]+)")
  if num then
    table.insert(folders, { tonumber(num), file })
  end
end
table.sort(folders, function (a, b) return a[1] < b[1] end )

-- Add the list of images of each folder
for i = 1,#folders do
  images_file = {}
  for file in lfs.dir("orl_faces/"..folders[i][2]) do
    num = string.match(file, "^([0-9]+)\.pgm")
    if num then
      table.insert(images_file, { tonumber(num), file })
    end
  end
  table.sort(images_file, function (a, b) return a[1] < b[1] end )
    
  table.insert(folders[i], images_file)
end

-- Load all images
for i = 1,#folders do
  images = {}
  for j = 1,#folders[i][3] do
    file = "orl_faces/"..folders[i][2].."/"..folders[i][3][j][2]

    print("Loading "..file.."...")
    io.flush()
    
    image = img.Image()
    image:load({ file=file })
    table.insert(images, image)
  end
  table.insert(folders[i], images)
end

function create_patterns(inputs, partitions, outputfile_prefix)
  -- Add images to calculate eigenfaces
  print("Adding images to calculate eigenfaces...")
  io.flush()

  -- Preparing partition
  do
    local str_partitions = ""
    for i = 1,#partitions do
      if i > 1 then
	str_partitions = str_partitions .. ", "
      end
      str_partitions = str_partitions .. partitions[i]
    end
    print("Preparing partition { "..str_partitions.." }...")
  end

  local images_for_training = {}
  local images_for_testing = {}
  local subject_for_training = {}
  local subject_for_testing = {}

  local eig = img.Eigenfaces()
  for i = 1,#folders do
    local ibeg = 0
    local iend
    for j = 1,#partitions do
      iend = ibeg + #folders[i][4] * partitions[j]/100
      if iend > #folders[i][4] then iend = #folders[i][4] end
      if (j % 2) == 1 then
	for k = ibeg+1,iend do
	  print(string.format("  Subject#%02d Image#%02d for Training", i, k))
	  eig:add_image({ folders[i][4][k] })
	  table.insert(images_for_training, folders[i][4][k])
	  table.insert(subject_for_training, i)
	end
      else
	for k = ibeg+1,iend do
	  print(string.format("  Subject#%02d Image#%02d for Testing", i, k))
	  table.insert(images_for_testing, folders[i][4][k])
	  table.insert(subject_for_testing, i)
	end
      end
      ibeg = iend
    end
  end

  -- Calculate eigenfaces
  print("Calculating eigenfaces...")
  io.flush()

  local percentage = string.match(inputs, "^([0-9]+)%%$")
  if percentage ~= nil then
    inputs = eig:calculate_eigenfaces({ variance=(tonumber(percentage) / 100) })
    print("Components = "..inputs.." for "..percentage.."% of variance")
  else
    print("Components = "..inputs)
  end
  io.flush()
  eig:calculate_eigenfaces({ components=inputs })
  
  -- Create the PatternSet with all subjects
  print("Creating patterns...")
  io.flush()

  -- Create training and testing sets
  local training_set = ann.PatternSet()
  local testing_set = ann.PatternSet()

  local eigenpoints = eig:project_in_eigenspace(images_for_training)
  for i = 1,#eigenpoints do
    training_set:add_pattern({ input=eigenpoints[i], output={ subject_for_training[i] } })
  end

  local eigenpoints = eig:project_in_eigenspace(images_for_testing)
  for i = 1,#eigenpoints do
    testing_set:add_pattern({ input=eigenpoints[i], output={ subject_for_testing[i] } })
  end

  -- Save the pattern set
  print("Saving training patterns in '"..outputfile_prefix.."_training.txt'...")
  io.flush()
  training_set:save(outputfile_prefix.."_training.txt")

  print("Saving testing patterns in '"..outputfile_prefix.."_testing.txt'...")
  io.flush()
  testing_set:save(outputfile_prefix.."_testing.txt")

  print("Done")
end

-- Creates patterns using different partitions as training-images,
-- this is necessary to do cross-validation later
function create_patterns2(inputs)
  create_patterns(inputs, {80,20, 0}, "orl_patterns/"..inputs.."_cross1")
  create_patterns(inputs, {60,20,20}, "orl_patterns/"..inputs.."_cross2")
  create_patterns(inputs, {40,20,40}, "orl_patterns/"..inputs.."_cross3")
  create_patterns(inputs, {20,20,60}, "orl_patterns/"..inputs.."_cross4")
  create_patterns(inputs, { 0,20,80}, "orl_patterns/"..inputs.."_cross5")
end

create_patterns2(25)
create_patterns2(50)
create_patterns2(75)
