-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

dofile("calculate_from_variance.lua")
dofile("orl_images_matrix.lua")

images_matrix = load_orl_images_matrix()
partitions = { 100, 0, 0 }
percentages = { 80 }
results = calculate_eigenfaces_for_given_variance(images_matrix, partitions, percentages)
for i = 1,#results do
  print(results[i].." components for "..percentages[i].."% of variance")
end
