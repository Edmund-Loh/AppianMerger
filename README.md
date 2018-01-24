# AppianMerger

## Background
Exporting and Deployment of Appian Objects requires the UUID or DataType Namspace to be included in the application XML file.
Without the UUID or the DT Namespace, the application will not be able to list the objects. 

## Objective
To reduce manual work for scanning of application XML file for existing UUID or DT Namespace of exported Appian objects. 
To insert the UUID of the exported objects into the respective object groups if the UUID doesn't exist.

## 
```bash
./app <Path of Object Directory> <Path of Application XML>
```




