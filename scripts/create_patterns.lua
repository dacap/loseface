-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

dofile("divide_images_matrix.lua")

function create_patterns(images_matrix, inputs, partitions, outputfile_prefix)
  -- Add images to calculate eigenfaces
  print("Adding images to calculate eigenfaces...")
  io.flush()

  local num_of_images, 
  	images_for_training, images_for_testing,
  	subject_for_training, subject_for_testing
    = divide_images_matrix(images_matrix, partitions)

  -- Create the eigenfaces class
  local eig = img.Eigenfaces()
  eig:reserve(#images_for_training)
  for i = 1,#images_for_training do
    eig:add_image(images_for_training[i])
  end

  -- Calculate eigenfaces
  print("Calculating eigenfaces...")
  print("Components = "..inputs)
  io.flush()
  eig:calculate_eigenfaces({ components=inputs })
  
  -- Create the PatternSet with all subjects
  print("Creating patterns...")
  io.flush()

  -- Create training and testing sets
  local training_set = ann.PatternSet()
  local testing_set = ann.PatternSet()
  local eigenpoints

  eigenpoints = eig:project_in_eigenspace(images_for_training)
  -- print(string.format("eigenpoints=%d, subject_for_training=%d, images_for_training=%d", 
  -- 		      #eigenpoints, #subject_for_training, #images_for_training))
  for i = 1,#eigenpoints do
    -- print("  >>> new in training_set "..subject_for_training[i])
    training_set:add_pattern(eigenpoints[i], { subject_for_training[i] })
  end

  eigenpoints = eig:project_in_eigenspace(images_for_testing)
  for i = 1,#eigenpoints do
    -- print("  >>> new in testing_set "..subject_for_testing[i])
    testing_set:add_pattern(eigenpoints[i], { subject_for_testing[i] })
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
