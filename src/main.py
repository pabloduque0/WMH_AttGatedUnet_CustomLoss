import os
import SimpleITK as sitk
from keras.models import load_model
import numpy as np
import imageparser
import metrics
import subprocess
import cv2
import shlex
from shutil import copyfile


REMOVE_TOP_PRCTG = 0.05
REMOVE_BOT_PRCTG = 0.12
SLICE_SHAPE = (200, 200)

ENSSEMBLE_SIZE = 3

T1_NAME = "T1.nii.gz"
T1_BET_NAME = "T1_bet.nii.gz"
T1_MASK = "T1_bet_mask.nii.gz"
T1_COREG_MASK = "T1_bet_mask_rsfl.nii.gz"
T1_COREG_BRAIN = "T1_rsfl.nii.gz"

FLAIR_NAME = "FLAIR.nii.gz"
FLAIR_BET_NAME = "FLAIR_bet.nii.gz"
FLAIR_ENHANCED_NAME = "FLAIR-enhanced.nii.gz"

SLICER_PATH = "/slicer/" #old slicer
EDISON_PATH = "/edison/"
MODEL_FOLDER = "/wmhseg_example/models"


inputDir = '/input'
outputDir = '/output'

copyfile(os.path.join(inputDir, "pre", T1_NAME), os.path.join(outputDir, T1_NAME))
copyfile(os.path.join(inputDir, "pre", FLAIR_NAME), os.path.join(outputDir, FLAIR_NAME))

# Get masked images
base_command = "hd-bet -i {} -o {} -device cpu -mode fast -tta 0"
process = subprocess.Popen(shlex.split(base_command.format(os.path.join(inputDir, "pre", T1_NAME),
                                                           os.path.join(outputDir, T1_BET_NAME))),
                           stdout=subprocess.PIPE)
output, error = process.communicate()

# Co-register app
co_register_command = "/coregister_app/coregister_app -3dspath {} -indir {} -images {} {} {} -outdir {}".format(SLICER_PATH,
                                                                                                                        outputDir,
                                                                                                                      FLAIR_NAME,
                                                                                                                      T1_NAME, T1_MASK,
                                                                                                                      outputDir)

co_reg_process = subprocess.Popen(shlex.split(co_register_command), stdout=subprocess.PIPE)
output, error = co_reg_process.communicate()

contrast_command = "/edison_app/edison_app -edisonpath {} -indir {} -images {} {} -outdir {}".format(EDISON_PATH,
                                                                                                               outputDir,
                                                                                                               FLAIR_NAME,
                                                                                                               T1_COREG_MASK,
                                                                                                               outputDir)

contrast_process = subprocess.Popen(shlex.split(contrast_command), stdout=subprocess.PIPE)
output, error = contrast_process.communicate()


# Load the image
flair = sitk.GetArrayFromImage(sitk.ReadImage(os.path.join(outputDir, FLAIR_ENHANCED_NAME)))
initial_size = flair.shape[1:]
parser = imageparser.ImageParser()

common_mask = sitk.GetArrayFromImage(sitk.ReadImage(os.path.join(outputDir, T1_COREG_MASK)))

flair = parser.preprocess_dataset_flair(flair, SLICE_SHAPE, common_mask, REMOVE_TOP_PRCTG, REMOVE_BOT_PRCTG)
flair = np.expand_dims(flair, axis=-1)

t1 = sitk.GetArrayFromImage(sitk.ReadImage(os.path.join(outputDir, T1_COREG_BRAIN)))
t1 = parser.preprocess_dataset_t1(t1, SLICE_SHAPE, common_mask, REMOVE_TOP_PRCTG, REMOVE_BOT_PRCTG)
t1 = np.expand_dims(t1, axis=-1)

data = np.concatenate([t1, flair], axis=-1)

model = load_model(os.path.join(MODEL_FOLDER, "model_20200818_f1_new_weight_v2_0_0.hdf5"),
                       custom_objects={"dice_coefficient_loss": metrics.dice_coef_loss,
                                       "dice_coefficient": metrics.dice_coef,
                                       "custom_dice_f1_loss": metrics.custom_dice_and_f1_loss,
                                       "lession_recall": metrics.obj_recall,
                                       "f1_score": metrics.obj_f1})

first_slice_brain, last_slice_brain = parser.get_first_last_slice(common_mask)
REMOVE_TOP = int(np.ceil(common_mask.shape[0] * REMOVE_TOP_PRCTG))
REMOVE_BOT = int(np.ceil(common_mask.shape[0] * REMOVE_BOT_PRCTG))
prediction = model.predict(data)
bot_pad = np.zeros((first_slice_brain + REMOVE_BOT, *SLICE_SHAPE, 1))
top_pad = np.zeros(((common_mask.shape[0] - last_slice_brain) + REMOVE_TOP - 1, *SLICE_SHAPE, 1))


prediction = np.concatenate([bot_pad, prediction, top_pad], axis=0)
prediction = parser.resize_slices([prediction], initial_size)
prediction = np.asanyarray(prediction)

result_img = sitk.GetImageFromArray(np.squeeze(prediction))
sitk.WriteImage(result_img, os.path.join(outputDir, 'result.nii.gz'))



