-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
--
-- Description:
--   This file defines the calculate_eigenfaces_for_given_variance()
--   function used in "orl_patterns_variance.lua" and
--   "cidisinc_patterns_variance.lua" scripts.
--
-- Usage:
--   You cannot use this script directly, it should be included
--   in another script to have calculate_eigenfaces_for_given_variance() function available.

dofile("divide_images_matrix.lua")

-- calculate_eigenfaces_for_given_variance:
--   Auxiliary function to calculate the number of eigenfaces
--   needed to maintain a certain level of variance with a
--   given images-matrix.
--
-- Parameters:
--   images_matrix: As in divide_images_matrix
--   partitions: As in divide_images_matrix
--   percentages: Array of percentages to be tested
--                E.g. percentages = { 50, 70, 80, 90, ... }
--
-- Returns:
--   An array where the i-element correspond to how many eigenfaces
--   are needed for the percentages[i] level of variance. 
--
function calculate_eigenfaces_for_given_variance(images_matrix, partitions, percentages)

  local images_for_training, images_for_testing,
  	subject_for_training, subject_for_testing
    = divide_images_matrix(images_matrix, partitions)

  -- Create the eigenfaces class
  local eig = img.Eigenfaces()
  eig:reserve(#images_for_training)
  for i = 1,#images_for_training do
    eig:add_image(images_for_training[i])
  end

  local results = {}
  for i = 1,#percentages do
    local num_eigenfaces = eig:calculate_eigenfaces({ variance=percentages[i]/100 })
    table.insert(results, num_eigenfaces)
  end

  return results
end
