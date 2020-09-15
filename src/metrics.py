from keras import backend as K
import tensorflow as tf
import numpy as np
import tensorflow_addons as tfa


def custom_dice_coefficient(y_true, y_pred, recall_weight=0.7):
    recall_weight = tf.Variable(recall_weight, dtype=tf.float32)
    regular_dice = dice_coefficient(y_true, y_pred)
    recall = lession_recall(y_true, y_pred)
    recall = tf.cast(recall, dtype=tf.float32)
    recall_addition = recall * regular_dice * recall_weight
    return regular_dice + recall_addition

def custom_dice_f1(y_true, y_pred, f1_weight=1.7):
    f1_weight = tf.Variable(f1_weight, dtype=tf.float32)
    regular_dice = dice_coefficient(y_true, y_pred)
    f1 = f1_score(y_true, y_pred)
    f1 = tf.cast(f1, dtype=tf.float32)
    f1_addition = f1 * regular_dice * f1_weight
    return regular_dice + f1_addition


def lession_recall(y_true, y_pred):
    conn_comp_true = tfa.image.connected_components(tf.cast(tf.squeeze(y_true, axis=[-1]), tf.bool))
    conn_comp_pred = conn_comp_true * tf.cast(tf.squeeze(y_pred, axis=[-1]), tf.int32)

    n_conn_comp_true, _ = tf.unique(K.flatten(conn_comp_true))
    n_conn_comp_pred, _ = tf.unique(K.flatten(conn_comp_pred))
    n_conn_comp_true = tf.size(input=n_conn_comp_true) - 1
    n_conn_comp_pred = tf.size(input=n_conn_comp_pred) - 1

    recall = tf.cond(pred=tf.equal(n_conn_comp_true, tf.Variable(0)),
                     true_fn=lambda: tf.Variable(1.0, dtype=tf.float64), false_fn=lambda: n_conn_comp_pred / n_conn_comp_true)
    return recall

def f1_score(y_true, y_pred):

    conn_comp_true = tfa.image.connected_components(tf.cast(tf.squeeze(y_true, axis=[-1]), tf.bool))
    conn_comp_pred = conn_comp_true * tf.cast(tf.squeeze(y_pred, axis=[-1]), tf.int32)

    full_n_pred = tfa.image.connected_components(tf.cast(tf.squeeze(y_pred, axis=[-1]), tf.bool))
    full_n_pred, _ = tf.unique(K.flatten(full_n_pred))
    n_full_n_pred = tf.size(input=full_n_pred) - 1

    n_conn_comp_true, _ = tf.unique(K.flatten(conn_comp_true))
    n_conn_comp_pred, _ = tf.unique(K.flatten(conn_comp_pred))
    n_conn_comp_true = tf.size(input=n_conn_comp_true) - 1
    n_conn_comp_pred = tf.size(input=n_conn_comp_pred) - 1

    recall = tf.cond(pred=tf.equal(n_conn_comp_true, tf.Variable(0)),
                     true_fn=lambda: tf.Variable(1.0, dtype=tf.float64),
                     false_fn=lambda: n_conn_comp_pred / n_conn_comp_true)

    # precision = (number of detections that intersect with WMH) / (number of all detections)


    precision = tf.cond(pred=tf.equal(n_full_n_pred, tf.Variable(0)),
                     true_fn=lambda: tf.Variable(1.0, dtype=tf.float64),
                     false_fn=lambda: n_conn_comp_pred / n_full_n_pred)

    f1 = tf.cond(pred=tf.equal(precision + recall, tf.Variable(0.0, dtype=tf.float64)),
                        true_fn=lambda: tf.Variable(0.0, dtype=tf.float64),
                        false_fn=lambda: 2.0 * (precision * recall) / (precision + recall))
    return tf.cast(f1, dtype=tf.float32)

def f1_loss(y_true, y_pred):
    return -f1_score(y_true, y_pred)


def custom_dice_coefficient_loss(y_true, y_pred):
    return -custom_dice_coefficient(y_true, y_pred)


def custom_dice_f1_loss(y_true, y_pred):
    return -custom_dice_f1(y_true, y_pred)


def dice_coefficient(y_true, y_pred, smooth=0.1):
    y_true_f = K.flatten(y_true)
    y_pred_f = K.flatten(y_pred)

    intersection = K.sum(y_pred_f * y_true_f)
    return (2. * intersection + smooth) / (K.sum(y_true_f) + K.sum(y_pred_f) + smooth)


def dice_coefficient_loss(y_true, y_pred):
    return -dice_coefficient(y_true, y_pred)

def sigmoid(x):
    return 1. / (1. + K.exp(-x))

def segmentation_recall(y_true, y_pred):
    y_true_f = K.flatten(y_true)
    y_pred_f = K.flatten(y_pred)

    recall = K.sum(y_pred_f * y_true_f) / tf.cast(K.sum(y_true_f), tf.float32)
    return recall


custom_dice_coef = custom_dice_coefficient
custom_dice_loss = custom_dice_coefficient_loss
dice_coef = dice_coefficient
dice_coef_loss = dice_coefficient_loss

pixel_recall = segmentation_recall

obj_recall = lession_recall
obj_f1 = f1_score
loss_f1 = f1_loss
custom_dice_and_f1_loss = custom_dice_f1_loss