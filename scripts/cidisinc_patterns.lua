-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

dofile("create_patterns.lua")
dofile("cidisinc_images_matrix.lua")

-- Creates patterns using different partitions as training-images,
-- this is necessary to do cross-validation later
function create_patterns2(images_matrix, inputs)
  create_patterns(images_matrix, inputs, {80,20, 0}, "cidisinc_patterns/"..inputs.."_cross1")
  create_patterns(images_matrix, inputs, {60,20,20}, "cidisinc_patterns/"..inputs.."_cross2")
  create_patterns(images_matrix, inputs, {40,20,40}, "cidisinc_patterns/"..inputs.."_cross3")
  create_patterns(images_matrix, inputs, {20,20,60}, "cidisinc_patterns/"..inputs.."_cross4")
  create_patterns(images_matrix, inputs, { 0,20,80}, "cidisinc_patterns/"..inputs.."_cross5")
end

images_matrix = load_cidisinc_images_matrix()
create_patterns2(images_matrix, 13)