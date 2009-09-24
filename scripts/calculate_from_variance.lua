-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

dofile("divide_images_matrix.lua")

-- Parameters:
-- images_matrix = { { subject1-image1, subject1-image2, ... }
--		     { subject2-image1, subject2-image2, ... } ... }
--
-- percentages = { 50, 70, 80, 90, ... }
--
function calculate_eigenfaces_for_given_variance(images_matrix, partitions, percentages)

  local num_of_images, 
  	images_for_training, images_for_testing,
  	subject_for_training, subject_for_testing
    = divide_images_matrix(images_matrix, partitions)

  -- Create the eigenfaces class
  local eig = img.Eigenfaces()
  eig:reserve({ size=num_of_images })
  eig:add_image(images_for_training)

  local results = {}
  for i = 1,#percentages do
    local num_eigenfaces = eig:calculate_eigenfaces({ variance=percentages[i]/100 })
    table.insert(results, num_eigenfaces)
  end

  return results
end
