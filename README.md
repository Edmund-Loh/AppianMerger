# AppianMerger

## Background
Exporting and Deployment of Appian Objects requires the UUID or DataType Namspace to be included in the application XML file.
Without the UUID or the DT Namespace, the application will not be able to list the objects. 

## Objective
To reduce manual work for scanning of application XML file for existing UUID or DT Namespace of exported Appian objects. 
To insert the UUID of the exported objects into the respective object groups if the UUID doesn't exist.

## Usage
```bash
./app <Path of Object Directory> <Path of Application XML>
```
|Arguments|Description|
|:-----------------------|:---|
|Path of Object Directory|Path of Unzipped Directory of exported Appian Objects which contains objects folders|
|Path of Application XML|Path of checked out Application XML file eg.appian-package/bgp-app/application/_a-0000db0e-a3c0-8000-0315-010000010000_2041.xml|




