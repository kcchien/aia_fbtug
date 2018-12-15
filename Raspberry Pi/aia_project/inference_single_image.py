import os
import cv2
import math
import numpy as np
import argparse
import mvnc.mvncapi as mvnc
import warnings
from colors import get_color

warnings.filterwarnings("ignore")

# Variable to store commandline arguments
ARGS = None

# OpenCV object for video capture
camera = None
vs = None

# ---- Step 0: Define necessary functions for later use -------------


def sigmoid(x):
    return 1. / (1. + np.exp(-x))


def softmax(x):
    e_x = np.exp(x - np.max(x))
    out = e_x / e_x.sum()
    return out


def iou(boxA, boxB):
    # Determine the coordinates of the intersection rectangle
    xA = max(boxA[0], boxB[0])
    yA = max(boxA[1], boxB[1])
    xB = min(boxA[2], boxB[2])
    yB = min(boxA[3], boxB[3])

    # Compute the area of intersection
    intersection_area = (xB - xA + 1) * (yB - yA + 1)

    # Compute the area of both rectangles
    boxA_area = (boxA[2] - boxA[0] + 1) * (boxA[3] - boxA[1] + 1)
    boxB_area = (boxB[2] - boxB[0] + 1) * (boxB[3] - boxB[1] + 1)

    # Compute the IOU
    iou = intersection_area / float(boxA_area + boxB_area - intersection_area)

    return iou


def non_maximal_suppression(thresholded_predictions, iou_threshold):
    print("Non maximal suppression........")
    nms_predictions = []
    nms_predictions.append(thresholded_predictions[0])

    i = 1
    while i < len(thresholded_predictions):
        n_boxes_to_check = len(nms_predictions)
        to_delete = False

        j = 0
        while j < n_boxes_to_check:
            curr_iou = iou(
                thresholded_predictions[i][0], nms_predictions[j][0])
            if(curr_iou > iou_threshold):
                to_delete = True
            j = j + 1

        if to_delete == False:
            nms_predictions.append(thresholded_predictions[i])
        i = i + 1

    return nms_predictions


def read_labels():
    classes = []
    with open('labels.txt', 'r') as file:
        lines = file.read().splitlines()
        for line in lines:
            classes.append(line)
    
    return classes


# ---- Step 1: Open the enumerated device and get a handle to it -------------

def open_ncs_device():
    # Look for enumerated NCS device(s); quit program if none found.
    devices = mvnc.enumerate_devices()
    if len(devices) == 0:
        print("No devices found")
        quit()

    # Get a handle to the first enumerated device and open it
    device = mvnc.Device(devices[0])
    device.open()

    return device


# ---- Step 2: Load a graph file onto the NCS device -------------------------

def load_graph(device):
    print("Load graph file to device........")
    # Read the graph file into a buffer
    with open(ARGS.graph, mode='rb') as f:
        graph_buffer = f.read()

    # Initialize Graph object
    graph = mvnc.Graph('graph')

    # Allocate the graph to the device and create input/output Fifos with default options in one call
    input_fifo, output_fifo = graph.allocate_with_fifos(device, graph_buffer)

    return graph, input_fifo, output_fifo


# ---- Step 3: Pre-process the images ----------------------------------------

def pre_process_image(frame):
    print("Pre process image........")
    # Resize image [Image size if defined by choosen network, during training]
    resized_image = cv2.resize(frame, tuple(
        ARGS.dim), interpolation=cv2.INTER_CUBIC)
    
    image_data = np.array(resized_image, dtype='f')

    # Normalization [0,255] -> [0,1]
    image_data /= 255.

    # Add batch dimension
    image_array = np.expand_dims(image_data, 0)

    return image_array


# ---- Step 4: Read & print inference results from the NCS -------------------

def infer_image(graph, input_fifo, output_fifo, img, frame):
    
    #score_threshold = 0.45
    score_threshold = 0.20
    iou_threshold = 0.25
    
    print("Inference image........")
    # Write the image to the input queue and queue the inference in one call
    graph.queue_inference_with_fifo_elem(input_fifo, output_fifo, img, None)

    # Get the results from the output queue
    output, userobj = output_fifo.read_elem()

    # Get execution time
    inference_time = graph.get_option(mvnc.GraphOption.RO_TIME_TAKEN)
    
    predictions = output

    input_width = frame.shape[1]
    input_height = frame.shape[0]

    w_scale = input_width / ARGS.dim[0]
    h_scale = input_height / ARGS.dim[1]

    input_image = frame

    #n_classes = 80
    n_classes = 4
    n_grid_cells = 13
    n_b_boxes = 5
    n_b_box_coord = 4

    classes = read_labels()

    # Pre-computed YOLOv2 shapes of the k=5 B-Boxes
    anchors = [0.57273, 0.677385, 1.87446, 2.06253, 3.33843,
               5.47434, 7.88282, 3.52778, 9.77052, 9.16828]

    thresholded_predictions = []

    # IMPORTANT: reshape to have shape = [13 x 13 x (5 B-Boxes) x (4 Coords + 1 Obj score + 20 Class scores)]
    #predictions = np.reshape(predictions, (13, 13, 5, 85))
    predictions = np.reshape(predictions, (13, 13, 5, 9))

    # IMPORTANT: Compute the coordinates and score of the B-Boxes by considering the parametrization of YOLOv2
    for row in range(n_grid_cells):
        for col in range(n_grid_cells):
            for b in range(n_b_boxes):

                tx, ty, tw, th, tc = predictions[row, col, b, :5]

                # IMPORTANT: (416 img size) / (13 grid cells) = 32!
                center_x = (float(col) + sigmoid(tx)) * 32.0
                center_y = (float(row) + sigmoid(ty)) * 32.0

                roi_w = np.exp(tw) * anchors[2 * b + 0] * 32.0
                roi_h = np.exp(th) * anchors[2 * b + 1] * 32.0

                final_confidence = sigmoid(tc)

                # Find best class
                class_predictions = predictions[row, col, b, 5:]
                class_predictions = softmax(class_predictions)

                class_predictions = tuple(class_predictions)
                
                best_class = class_predictions.index(max(class_predictions))
                best_class_score = class_predictions[best_class]

                # Flip the coordinates on both axes
                left = int(center_x - (roi_w / 2.))
                right = int(center_x + (roi_w / 2.))
                top = int(center_y - (roi_h / 2.))
                bottom = int(center_y + (roi_h / 2.))
                
                if((final_confidence * best_class_score) > score_threshold):
                    thresholded_predictions.append(
                        [[left, top, right, bottom], final_confidence * best_class_score, classes[best_class]])

    # Sort the B-boxes by their final score
    thresholded_predictions.sort(key=lambda tup: tup[1], reverse=True)

    # Non maximal suppression
    nms_predictions = []

    if(len(thresholded_predictions) > 0):
        nms_predictions = non_maximal_suppression(
            thresholded_predictions, iou_threshold)

    # Draw final B-Boxes and label on input image
    biggest_object = (0, ())
    
    # nms_predictions 是 predict完成的 list
    for i in range(len(nms_predictions)):
        
        # nms_predictions:  [[left, right ,top , bottom], confidence, best_class_name]
        # nms_predictions:  [[101, 211, 175, 333], 0.32828456752606966, '1_pepper_young']
        
        # left, right ,top , bottom
        xmin = int(nms_predictions[i][0][0] * w_scale)
        ymin = int(nms_predictions[i][0][1] * h_scale)
        xmax = int(nms_predictions[i][0][2] * w_scale)
        ymax = int(nms_predictions[i][0][3] * h_scale)

        # Dimention of B-Box
        box_width = xmax - xmin
        box_height = ymax - ymin
        
        # Center point of B-Box
        centroid = (int(box_width / 2 + xmin), int(box_height / 2 + ymin))
        
        # Confidence
        confidence = '{:.2f}%'.format(nms_predictions[i][1] * 100)
        
        # Get best class name
        best_class_name = nms_predictions[i][2]
        
        # Show predict information
        print('class: ' + best_class_name + 
              ', confidence: ' + str(confidence) + 
              ', took: ' + str(np.sum(inference_time)) + ' ms' + 
              ', center coodinate: ' + str(centroid) +
              ', top left coodinate: ' + str(xmin) + ', ' + str(ymax) 
             )
        
        
        # Compose label text of B-Box
        label_str = best_class_name + ': ' + confidence
        
        # Calclate label text size
        text_size = cv2.getTextSize(label_str, cv2.FONT_HERSHEY_SIMPLEX, 1.1e-3 * input_image.shape[0], 3)
       
        width, height = text_size[0][0], text_size[0][1]
 
        # B-Box color
        b_box_color = get_color(classes.index(best_class_name))
        
        # Label background region
        region = np.array([[xmin-3,ymin], 
                            [xmin-3,ymin-height-26], 
                            [xmin+width+13, ymin-height-26], 
                            [xmin+width+13, ymin]], dtype='int32')  
        
        # Put a class rectangle with B-Box coordinates and a class label on the image
        input_image = cv2.circle(input_image, centroid, 5, b_box_color, -1)
        
        # Draw B-Box
        input_image = cv2.rectangle(
            input_image, (xmin, ymin), (xmax, ymax), b_box_color, 3)

        cv2.fillPoly(img=input_image, pts=[region], color=b_box_color)
        
        #cv2.putText(input_image, label_str, (xmin + 5, ymin - 10),
        #            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,0,0), 1)

        cv2.putText(img=input_image, 
                    text=label_str, 
                    org=(xmin+13, ymin-13), 
                    fontFace=cv2.FONT_HERSHEY_SIMPLEX, 
                    fontScale=1e-3 * input_image.shape[0], 
                    color=(0,0,0), 
                    thickness=2)

        #cv2.imshow('Pepper', input_image)
        
        img_path, img_filename = os.path.split(ARGS.image)
        
        cv2.imwrite(os.path.join('pepper_img_out', img_filename), input_image)

    # If a display is available, show the image on which inference was performed
    if 'DISPLAY' in os.environ:
        cv2.imshow('NCS live inference', input_image)


# ---- Step 5: Unload the graph and close the device -------------------------

def close_ncs_device(device, graph, input_fifo, output_fifo):
    output_fifo.destroy()
    input_fifo.destroy()
    graph.destroy()
    device.close()
    device.destroy()
    print("Intel NCS device closed........")


# ---- Main function (entry point for this script ) --------------------------

def main():
    print("Inference started........")
    
    device = open_ncs_device()
    graph, input_fifo, output_fifo = load_graph(device)

    img = cv2.imread(ARGS.image)
    img = img.astype(np.float32)

    img_orig = np.copy(img)
    img_orig_dimensions = img_orig.shape

    img = pre_process_image(img)

    # Inference
    infer_image(graph, input_fifo, output_fifo, img, img_orig)

    close_ncs_device(device, graph, input_fifo, output_fifo)


# ---- Define 'main' function as the entry point for this script -------------

if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        description="Detect objects on a LIVE camera feed using \
                         Intel Neural Compute Stick.")

    parser.add_argument('-g', '--graph', type=str,
                        default='built_graph/yolov2-tiny-farmbot.graph',
                        help="Absolute path to the neural network graph file.")

    parser.add_argument("-i", "--image", type=str,
                        required=True,
                        help="whether or not the Raspberry Pi camera should be used")

    parser.add_argument('-D', '--dim', type=int,
                        nargs='+',
                        default=[416, 416],
                        help="Image dimensions. ex. -D 416 416")

    ARGS = parser.parse_args()

    main()

# ==== End of file ===========================================================
