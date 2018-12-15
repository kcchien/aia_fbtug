# AIAcademy 台中分校技術領袖班 第一期 期末專題 - FBTUG Image

## Team introduction

- Team ID: Project_bt071_group29
- Team Name: FBTUG_Img_t1
- Team Members:
  - 王柏鈞
  - 洪嘉聰
  - 陳正宜
  - 樂以虎
  - 簡光正

## 系統建置 (Model Training)

### Yolo v3

- Keras Yolo3

### Tiny Yolo v2

- Darkflow

  - [Darkflow](https://github.com/thtrieu/darkflow)

### MobileNet-SSD

- Tensorflow

## 系統建置 (Raspberry Pi)

### 硬體 Hardware

- Raspberry Pi 3 B+
- Intel Movidius (1 代)
- Linux PC (Compile graph file for Intel Movidius)

### 軟體 Software

#### Raspberry Pi

- Operating System
  - Raspbian Stretch
- Intel Movidius SDK (v2.08.01)
  - [Intel NCSCK Documentation](https://movidius.github.io/ncsdk/)
  - [Intel NCSDK](https://github.com/movidius/ncsdk)
  - [Intel NC appzoo](https://github.com/movidius/ncappzoo)
- OpenCV (3.3.0, Intel NCSCK v2 預設)
  - [Raspbian Stretch: Install OpenCV 3 + Python on your Raspberry Pi](https://www.pyimagesearch.com/2017/09/04/raspbian-stretch-install-opencv-3-python-on-your-raspberry-pi/)
  - [Optimizing OpenCV on the Raspberry Pi](https://www.pyimagesearch.com/2017/10/09/optimizing-opencv-on-the-raspberry-pi/)
- Tensorflow 1.9.0 (Intel NCSDK v2 目前只支援到 1.9.0) for ARM

```bash
cd ~

# Uninstall any previous tensorflow in system
sudo pip3 uninstall tensorflow

# Get opencv 1.9.0 for arm cpu
wget https://github.com/lhelontra/tensorflow-on-arm/releases/download/v1.9.0/tensorflow-1.9.0-cp35-none-linux_armv7l.whl

# Install tensorflow with pip3
sudo pip3 install tensorflow-1.9.0-cp35-none-linux_armv7l.whl

# Remove for saving space
rm tensorflow-1.9.0-cp35-none-linux_armv7l.whl
```

- Node-RED

```bash

# Install Node-RED for Raspberry Pi

bash <(curl -sL https://raw.githubusercontent.com/node-red/raspbian-deb-package/master/resources/update-nodejs-and-nodered)
```

Node-RED flow, copy following and import to Node-RED

```json
[
  {
    "id": "6e761ff2.a900a",
    "type": "tab",
    "label": "Flow 1",
    "disabled": false,
    "info": ""
  },
  {
    "id": "53b7af17.57706",
    "type": "serial out",
    "z": "6e761ff2.a900a",
    "name": "機械手臂",
    "serial": "82f25eea.46497",
    "x": 2000,
    "y": 380,
    "wires": []
  },
  {
    "id": "b1037ff6.4c16",
    "type": "camerapi-takephoto",
    "z": "6e761ff2.a900a",
    "filemode": "1",
    "filename": "pepper.jpg",
    "filedefpath": "0",
    "filepath": "/home/pi/aia_project/",
    "fileformat": "jpeg",
    "resolution": "3",
    "rotation": "0",
    "fliph": "0",
    "flipv": "0",
    "brightness": "50",
    "contrast": "0",
    "sharpness": "0",
    "quality": "80",
    "imageeffect": "none",
    "exposuremode": "auto",
    "iso": "0",
    "agcwait": "0.5",
    "led": "0",
    "awb": "auto",
    "name": "800x600 Camera",
    "x": 370,
    "y": 240,
    "wires": [["55d0c084.4a50c", "aa28262f.ee2818", "cfa9fb54.b0e638"]]
  },
  {
    "id": "aa28262f.ee2818",
    "type": "pythonshell in",
    "z": "6e761ff2.a900a",
    "name": "",
    "pyfile": "/home/pi/aia_project/inference_auto.py",
    "virtualenv": "/home/pi/.virtualenvs/cv",
    "continuous": true,
    "stdInData": false,
    "x": 600,
    "y": 340,
    "wires": [["a27d061a.6ea728"]]
  },
  {
    "id": "a27d061a.6ea728",
    "type": "switch",
    "z": "6e761ff2.a900a",
    "name": "",
    "property": "payload",
    "propertyType": "msg",
    "rules": [
      {
        "t": "cont",
        "v": "aia#",
        "vt": "str"
      },
      {
        "t": "cont",
        "v": "#image#",
        "vt": "str"
      },
      {
        "t": "else"
      }
    ],
    "checkall": "true",
    "repair": false,
    "outputs": 3,
    "x": 770,
    "y": 340,
    "wires": [["b873bb3a.5014c8"], ["43d991f6.5104"], ["f972fae9.17e2d8"]]
  },
  {
    "id": "73c29a52.5c3f04",
    "type": "debug",
    "z": "6e761ff2.a900a",
    "name": "",
    "active": true,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "payload",
    "x": 1090,
    "y": 320,
    "wires": []
  },
  {
    "id": "babe596e.03e968",
    "type": "ui_button",
    "z": "6e761ff2.a900a",
    "name": "",
    "group": "b8e34c4a.d055e",
    "order": 1,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "SNAP !",
    "color": "",
    "bgcolor": "green",
    "icon": "linked_camera",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 180,
    "y": 240,
    "wires": [["b1037ff6.4c16"]]
  },
  {
    "id": "e16ede8f.fa6e1",
    "type": "template",
    "z": "6e761ff2.a900a",
    "name": "",
    "field": "payload",
    "fieldType": "msg",
    "format": "handlebars",
    "syntax": "mustache",
    "template": "<img src=\"data:image/jpg;base64,{{{payload}}}\">",
    "output": "str",
    "x": 1120,
    "y": 240,
    "wires": [["9e7c0ed2.9d001"]]
  },
  {
    "id": "9e7c0ed2.9d001",
    "type": "ui_template",
    "z": "6e761ff2.a900a",
    "group": "8e9a166c.e3c998",
    "name": "Camera Picture",
    "order": 1,
    "width": "12",
    "height": "8",
    "format": "<div ng-bind-html=\"msg.payload\"></div>",
    "storeOutMessages": true,
    "fwdInMessages": true,
    "templateScope": "local",
    "x": 1280,
    "y": 240,
    "wires": [[]]
  },
  {
    "id": "2189f0ce.2e94f",
    "type": "base64",
    "z": "6e761ff2.a900a",
    "name": "",
    "action": "str",
    "property": "payload",
    "x": 980,
    "y": 240,
    "wires": [["e16ede8f.fa6e1"]]
  },
  {
    "id": "c78d7a29.f636c8",
    "type": "file in",
    "z": "6e761ff2.a900a",
    "name": "讀取拍攝照片",
    "filename": "",
    "format": "",
    "chunk": false,
    "sendError": false,
    "x": 820,
    "y": 240,
    "wires": [["2189f0ce.2e94f"]]
  },
  {
    "id": "55d0c084.4a50c",
    "type": "change",
    "z": "6e761ff2.a900a",
    "name": "",
    "rules": [
      {
        "t": "set",
        "p": "filename",
        "pt": "msg",
        "to": "payload",
        "tot": "msg"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 610,
    "y": 240,
    "wires": [["c78d7a29.f636c8"]]
  },
  {
    "id": "cfa9fb54.b0e638",
    "type": "ui_toast",
    "z": "6e761ff2.a900a",
    "position": "top right",
    "displayTime": "3",
    "highlight": "",
    "outputs": 0,
    "ok": "OK",
    "cancel": "",
    "topic": "拍攝照片",
    "name": "",
    "x": 610,
    "y": 200,
    "wires": []
  },
  {
    "id": "f972fae9.17e2d8",
    "type": "ui_toast",
    "z": "6e761ff2.a900a",
    "position": "top right",
    "displayTime": "3",
    "highlight": "",
    "outputs": 0,
    "ok": "OK",
    "cancel": "",
    "topic": "Inference",
    "name": "",
    "x": 1110,
    "y": 640,
    "wires": []
  },
  {
    "id": "e7c56a11.353fa8",
    "type": "template",
    "z": "6e761ff2.a900a",
    "name": "",
    "field": "payload",
    "fieldType": "msg",
    "format": "handlebars",
    "syntax": "mustache",
    "template": "<img src=\"data:image/jpg;base64,{{{payload}}}\">",
    "output": "str",
    "x": 1400,
    "y": 600,
    "wires": [["4ba65970.2ab208"]]
  },
  {
    "id": "4ba65970.2ab208",
    "type": "ui_template",
    "z": "6e761ff2.a900a",
    "group": "95aed0aa.aa324",
    "name": "Predicted Picture",
    "order": 1,
    "width": "12",
    "height": "8",
    "format": "<div ng-bind-html=\"msg.payload\"></div>",
    "storeOutMessages": true,
    "fwdInMessages": true,
    "templateScope": "local",
    "x": 1570,
    "y": 600,
    "wires": [[]]
  },
  {
    "id": "60c86809.bda158",
    "type": "base64",
    "z": "6e761ff2.a900a",
    "name": "",
    "action": "str",
    "property": "payload",
    "x": 1260,
    "y": 600,
    "wires": [["e7c56a11.353fa8"]]
  },
  {
    "id": "43d991f6.5104",
    "type": "file in",
    "z": "6e761ff2.a900a",
    "name": "讀取推論照片",
    "filename": "/home/pi/aia_project/pepper_img_out/pepper.jpg",
    "format": "",
    "chunk": false,
    "sendError": false,
    "x": 1100,
    "y": 600,
    "wires": [["60c86809.bda158"]]
  },
  {
    "id": "b873bb3a.5014c8",
    "type": "function",
    "z": "6e761ff2.a900a",
    "name": "",
    "func": "msg.payload = msg.payload.split('#');\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "x": 910,
    "y": 320,
    "wires": [
      [
        "73c29a52.5c3f04",
        "e16bc734.f2c0c8",
        "3b7d2c59.3d9844",
        "bd5b4108.d6ba6",
        "93311c2c.8517c",
        "64e25052.171d6"
      ]
    ]
  },
  {
    "id": "e16bc734.f2c0c8",
    "type": "ui_text",
    "z": "6e761ff2.a900a",
    "group": "b8e34c4a.d055e",
    "order": 2,
    "width": 0,
    "height": 0,
    "name": "",
    "label": "Inference Time",
    "format": "{{msg.payload[3]}} ms",
    "layout": "row-spread",
    "x": 1100,
    "y": 360,
    "wires": []
  },
  {
    "id": "44e853e4.ae021c",
    "type": "ui_button",
    "z": "6e761ff2.a900a",
    "name": "初始化手臂 AIA",
    "group": "b8e34c4a.d055e",
    "order": 5,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "初始化手臂AIA",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "AIA",
    "payloadType": "str",
    "topic": "",
    "x": 1700,
    "y": 240,
    "wires": [["53b7af17.57706"]]
  },
  {
    "id": "f272d5dc.720778",
    "type": "ui_button",
    "z": "6e761ff2.a900a",
    "name": "自動模式 auto",
    "group": "b8e34c4a.d055e",
    "order": 6,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "自動模式auto",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "auto",
    "payloadType": "str",
    "topic": "",
    "x": 1700,
    "y": 280,
    "wires": [["53b7af17.57706"]]
  },
  {
    "id": "3264da58.8498f6",
    "type": "ui_button",
    "z": "6e761ff2.a900a",
    "name": "離開自動模式 out",
    "group": "b8e34c4a.d055e",
    "order": 7,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "離開自動模式out",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "out",
    "payloadType": "str",
    "topic": "",
    "x": 1690,
    "y": 320,
    "wires": [["53b7af17.57706"]]
  },
  {
    "id": "4c1f271.db93cd8",
    "type": "ui_button",
    "z": "6e761ff2.a900a",
    "name": "終止控制 over",
    "group": "b8e34c4a.d055e",
    "order": 8,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "終止控制over",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "over",
    "payloadType": "str",
    "topic": "",
    "x": 1700,
    "y": 360,
    "wires": [["53b7af17.57706"]]
  },
  {
    "id": "3b7d2c59.3d9844",
    "type": "ui_text",
    "z": "6e761ff2.a900a",
    "group": "b8e34c4a.d055e",
    "order": 3,
    "width": 0,
    "height": 0,
    "name": "",
    "label": "Label",
    "format": "{{msg.payload[1]}}",
    "layout": "row-spread",
    "x": 1070,
    "y": 400,
    "wires": []
  },
  {
    "id": "bd5b4108.d6ba6",
    "type": "ui_text",
    "z": "6e761ff2.a900a",
    "group": "b8e34c4a.d055e",
    "order": 4,
    "width": 0,
    "height": 0,
    "name": "",
    "label": "Confidence",
    "format": "{{msg.payload[2]}}",
    "layout": "row-spread",
    "x": 1090,
    "y": 440,
    "wires": []
  },
  {
    "id": "371876ec.81e6aa",
    "type": "ui_text",
    "z": "6e761ff2.a900a",
    "group": "b8e34c4a.d055e",
    "order": 4,
    "width": 0,
    "height": 0,
    "name": "",
    "label": "X",
    "format": "{{msg.payload}}",
    "layout": "row-spread",
    "x": 1230,
    "y": 480,
    "wires": []
  },
  {
    "id": "ea750108.b6544",
    "type": "ui_text",
    "z": "6e761ff2.a900a",
    "group": "b8e34c4a.d055e",
    "order": 4,
    "width": 0,
    "height": 0,
    "name": "",
    "label": "Y",
    "format": "{{msg.payload}}",
    "layout": "row-spread",
    "x": 1230,
    "y": 520,
    "wires": []
  },
  {
    "id": "93311c2c.8517c",
    "type": "change",
    "z": "6e761ff2.a900a",
    "name": "X",
    "rules": [
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "payload[4]",
        "tot": "msg"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 1070,
    "y": 480,
    "wires": [["371876ec.81e6aa", "53b7af17.57706"]]
  },
  {
    "id": "64e25052.171d6",
    "type": "change",
    "z": "6e761ff2.a900a",
    "name": "Y",
    "rules": [
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "payload[5]",
        "tot": "msg"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 1070,
    "y": 520,
    "wires": [["ea750108.b6544", "bb7086e1.e2a228"]]
  },
  {
    "id": "92e8c3ad.809ff",
    "type": "ui_button",
    "z": "6e761ff2.a900a",
    "name": "ACT 4",
    "group": "b8e34c4a.d055e",
    "order": 0,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "ACT 4",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "act4",
    "payloadType": "str",
    "topic": "",
    "x": 1730,
    "y": 480,
    "wires": [["53b7af17.57706"]]
  },
  {
    "id": "97a377c8.060998",
    "type": "change",
    "z": "6e761ff2.a900a",
    "name": "X 100",
    "rules": [
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "100",
        "tot": "str"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 1730,
    "y": 400,
    "wires": [["53b7af17.57706"]]
  },
  {
    "id": "b95f1995.839a18",
    "type": "change",
    "z": "6e761ff2.a900a",
    "name": "Y 200",
    "rules": [
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "200",
        "tot": "str"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 1730,
    "y": 440,
    "wires": [["53b7af17.57706"]]
  },
  {
    "id": "2d7bc623.91747a",
    "type": "delay",
    "z": "6e761ff2.a900a",
    "name": "",
    "pauseType": "delay",
    "timeout": "500",
    "timeoutUnits": "milliseconds",
    "rate": "1",
    "nbRateUnits": "1",
    "rateUnits": "second",
    "randomFirst": "1",
    "randomLast": "5",
    "randomUnits": "seconds",
    "drop": false,
    "x": 1590,
    "y": 440,
    "wires": [["b95f1995.839a18"]]
  },
  {
    "id": "c938bf70.22bbe",
    "type": "ui_button",
    "z": "6e761ff2.a900a",
    "name": "X Y 測試",
    "group": "b8e34c4a.d055e",
    "order": 0,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "X Y 測試",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 1420,
    "y": 440,
    "wires": [["2d7bc623.91747a", "97a377c8.060998"]]
  },
  {
    "id": "bb7086e1.e2a228",
    "type": "delay",
    "z": "6e761ff2.a900a",
    "name": "",
    "pauseType": "delay",
    "timeout": "500",
    "timeoutUnits": "milliseconds",
    "rate": "1",
    "nbRateUnits": "1",
    "rateUnits": "second",
    "randomFirst": "1",
    "randomLast": "5",
    "randomUnits": "seconds",
    "drop": false,
    "x": 1730,
    "y": 520,
    "wires": [["53b7af17.57706"]]
  },
  {
    "id": "82f25eea.46497",
    "type": "serial-port",
    "z": "",
    "serialport": "/dev/ttyUSB0",
    "serialbaud": "9600",
    "databits": "8",
    "parity": "none",
    "stopbits": "1",
    "newline": "\\n",
    "bin": "false",
    "out": "char",
    "addchar": false,
    "responsetimeout": "10000"
  },
  {
    "id": "b8e34c4a.d055e",
    "type": "ui_group",
    "z": "",
    "name": "Actions",
    "tab": "8b6b274c.365968",
    "order": 3,
    "disp": true,
    "width": "6",
    "collapse": false
  },
  {
    "id": "8e9a166c.e3c998",
    "type": "ui_group",
    "z": "",
    "name": "Live Camera",
    "tab": "8b6b274c.365968",
    "disp": true,
    "width": "12",
    "collapse": false
  },
  {
    "id": "95aed0aa.aa324",
    "type": "ui_group",
    "z": "",
    "name": "Predicted",
    "tab": "8b6b274c.365968",
    "disp": true,
    "width": "12",
    "collapse": false
  },
  {
    "id": "8b6b274c.365968",
    "type": "ui_tab",
    "z": "",
    "name": "Home",
    "icon": "dashboard"
  }
]
```

#### Linux PC (Compile Graph)

```bash

# Compiling tensorflow to Intel NCS graph file
mvNCCompile built_graph/tiny-yolo-voc.pb -s 12 -in input -on output -o built_graph/tiny-yolo-voc.graph
```

## 系統建置 (機械手臂)

- 機械手臂
  - OWI Robotic Arm (http://owirobot.com/robotic-arm-edge-1/)
- Arduino
- ZigBee
- Commands define
  - AIA: Initialize command
  - auto: Enter auto mode
  - out: Exit auto mode
  - over: Terminate
  - x: Coordinate of X point
  - y: Coordinate of Y point
  - act4: Testing action

## Testing on Raspberry Pi

```bash
# Inference single image
inference_single_image.py -i pepper.jpg

# Inference through live video with USB webcam
livevideo_ncs.py

# Inference through live video with Pi Camera module
livevideo_ncs.py -p 1
```

## Useful References

### Darkflow: How to get a WORKING protobuf file (.pb) from a ckpt file

https://github.com/thtrieu/darkflow/issues/698

You can test a training state or a trained dataset in 2 ways:

Directly with this line:

```bash
./flow --imgdir sample_img/ --model cfg/tiny-yolo-voc-1c.cfg --load -1
```

or

You can first save your training set and after this show the result directly from the .pb file.

```bash
./flow --model cfg/tiny-yolo-voc-1c.cfg --load -1 --savepb
```

```bash
./flow --pbLoad built_graph/tiny-yolo-voc-1c.pb --metaLoad built_graph/tiny-yolo-voc-1c.meta --imgdir sample_img/
```
