-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

-- images_matrix = { { subject1-image1, subject1-image2, ... }
--		     { subject2-image1, subject2-image2, ... } ... }
-- percentages = { 50, 70, 80, 90, ... }
function calculate_eigenfaces_for_given_variance(images_matrix, partitions, percentages)
  -- Preparing partition
  do
    local str_partitions = ""
    for i = 1,#partitions do
      if i > 1 then
	str_partitions = str_partitions .. ", "
      end
      str_partitions = str_partitions .. partitions[i]
    end
    -- print("Preparing partition { "..str_partitions.." }...")
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

  -- add the images depending the partitions
  for i = 1,#images_matrix do
    local ibeg = 0
    local iend
    for j = 1,#partitions do
      iend = ibeg + #images_matrix[i] * partitions[j]/100
      if iend > #images_matrix[i] then iend = #images_matrix[i] end
      if (j % 2) == 1 then
	for k = ibeg+1,iend do
	  --print(string.format("  Subject#%02d Image#%02d for Training", i, k))
	  eig:add_image({ images_matrix[i][k] })
	  table.insert(images_for_training, images_matrix[i][k])
	  table.insert(subject_for_training, i)
	end
      else
	for k = ibeg+1,iend do
	  --print(string.format("  Subject#%02d Image#%02d for Testing", i, k))
	  table.insert(images_for_testing, images_matrix[i][k])
	  table.insert(subject_for_testing, i)
	end
      end
      ibeg = iend
    end
  end

  -- Calculate eigenfaces
  -- print("Calculating components...")
  -- io.flush()

  local results = {}
  for i = 1,#percentages do
    local num_eigenfaces = eig:calculate_eigenfaces({ variance=percentages[i]/100 })
    table.insert(results, num_eigenfaces)
  end
  return results
end
