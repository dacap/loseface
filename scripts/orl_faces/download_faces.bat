@echo off
rem   This directory should contain all folders in the following .zip file:
rem 
rem     http://www.cl.cam.ac.uk/research/dtg/attarchive/pub/data/att_faces.zip
rem 
rem   It contains the original photos of the ORL database to create the
rem   training patterns.

wget http://www.cl.cam.ac.uk/research/dtg/attarchive/pub/data/att_faces.zip
unzip att_faces.zip
