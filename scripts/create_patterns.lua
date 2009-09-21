-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

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

  -- Create the eigenfaces class
  local eig = img.Eigenfaces()

  -- Reserve space for all images
  local num_of_images = 0
  for i = 1,#images_matrix do 
    for j = 1,#images_matrix[i] do
      num_of_images = num_of_images + 1
    end
  end
  if num_of_images > 0 then
    eig:reserve({ size=num_of_images })
  end

  for i = 1,#images_matrix do
    local ibeg = 0
    local iend
    for j = 1,#partitions do
      iend = ibeg + #images_matrix[i] * partitions[j]/100
      if iend > #images_matrix[i] then iend = #images_matrix[i] end
      if (j % 2) == 1 then
	for k = ibeg+1,iend do
	  print(string.format("  Subject#%02d Image#%02d for Training", i, k))
	  eig:add_image({ images_matrix[i][k] })
	  table.insert(images_for_training, images_matrix[i][k])
	  table.insert(subject_for_training, i)
	end
      else
	for k = ibeg+1,iend do
	  print(string.format("  Subject#%02d Image#%02d for Testing", i, k))
	  table.insert(images_for_testing, images_matrix[i][k])
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
