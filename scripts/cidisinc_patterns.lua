-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
--
-- Description:
--   This script converts the CIDISINC images to points in an eigenspace.
--   These points (patterns) can be used as input for a MLP neural net.
--   The faces are loaded from cidisinc_faces/ directory, and patterns are
--   saved in cidisinc_patterns/ directory.

dofile("create_patterns.lua")
dofile("cidisinc_images_matrix.lua")

-- create_patterns2:
--   Creates patterns using different partitions (folds) as training-images,
--   this is necessary to do k-fold cross-validation later
function create_patterns2(images_matrix, inputs)
  create_patterns(images_matrix, inputs, {80,20, 0}, "cidisinc_patterns/"..inputs.."_fold1")
  create_patterns(images_matrix, inputs, {60,20,20}, "cidisinc_patterns/"..inputs.."_fold2")
  create_patterns(images_matrix, inputs, {40,20,40}, "cidisinc_patterns/"..inputs.."_fold3")
  create_patterns(images_matrix, inputs, {20,20,60}, "cidisinc_patterns/"..inputs.."_fold4")
  create_patterns(images_matrix, inputs, { 0,20,80}, "cidisinc_patterns/"..inputs.."_fold5")
end

images_matrix = load_cidisinc_images_matrix()
create_patterns2(images_matrix, 13)
