#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

DIR *dpdf;
struct dirent *epdf;

bool gTestFlag = false;

bool directory_exists( const std::string &directory ){
    if( !directory.empty() ){
        if( access(directory.c_str(), 0) == 0 ){
            struct stat status;
            stat( directory.c_str(), &status );
            if( status.st_mode & S_IFDIR )
                return true;
        }
    }
    return false;
}

// TODO: Retrieve Object Details
string get_object_name( const std::string &filepath ){
    string result = "";
    tinyxml2::XMLDocument obj_doc;

    tinyxml2::XMLError eResult = obj_doc.LoadFile(filepath.c_str());
    
    if (eResult != tinyxml2::XML_SUCCESS) 
    {               
        cout << "Invalid Application XML File" << endl;
        return result;
    }

    tinyxml2::XMLElement* root = obj_doc.FirstChildElement("contentHaul")->FirstChildElement();

    if ( root == nullptr)
    {
        cout << "Invalid Application XML File" << endl;
        return result;
    }

    return root->Value();
}

string get_xsd_name( const std::string &filepath ){

    tinyxml2::XMLDocument obj_doc;

    tinyxml2::XMLError eResult = obj_doc.LoadFile(filepath.c_str());

    tinyxml2::XMLElement* root = obj_doc.FirstChildElement("xsd:schema");

    tinyxml2::XMLElement* child = root->FirstChildElement("xsd:complexType");

    string ns(root->Attribute("targetNamespace") );
    return "{" + ns + "}" + child->Attribute("name");
}

int main(int argc, char* argv[])
{
    if ( argc < 3 ){
        cout << endl;
        cout << "Usage:" << endl;
        cout << "./appianMerger objectsDirectory applicationXml [options...]" << endl;
        cout << endl;
        cout << "Options:" << endl;
        cout << "\t-t \t\t" << "Test Only, No Change To Application XML" << endl << endl;
        return 0;
    }

    if ( argc == 4 ){
        if (strcmp(argv[3], "-t") == 0)
        {
            cout << "Test Only" << endl;
            gTestFlag = true;
        }
    }

    string oriRootPathStr (argv[1]);
    
    tinyxml2::XMLDocument xml_doc;

    tinyxml2::XMLError eResult = xml_doc.LoadFile(argv[2]);
    
    if (eResult != tinyxml2::XML_SUCCESS) 
    {
        cout << "Invalid Application XML File" << endl;
        return -1;
    }

    tinyxml2::XMLElement* root = xml_doc.FirstChildElement("applicationHaul")
        ->FirstChildElement("application")
        ->FirstChildElement("associatedObjects")
        ->FirstChildElement("globalIdMap");

    if ( root == nullptr)
    {
        cout << "Invalid Application XML File" << endl;
        return -1;
    }
    
    // Backup
    if (!gTestFlag)
        xml_doc.SaveFile("backup.xml");

    for (tinyxml2::XMLElement* itemChild = root->FirstChildElement(); itemChild != NULL; itemChild = itemChild->NextSiblingElement())
    {
    // do something with each child element
    tinyxml2::XMLElement* typeChild = itemChild->FirstChildElement();

    string typeStr = typeChild->GetText();
    
    string tempObjPathStr = oriRootPathStr;
    tempObjPathStr.append("/");
    tempObjPathStr.append(typeStr);
    
    if (directory_exists(tempObjPathStr))
    {
        //cout << "----- " <<  tempObjPathStr << " -----" << endl;
        dpdf = opendir(tempObjPathStr.c_str());
        if (dpdf != NULL){
        while ((epdf = readdir(dpdf)) != NULL){
            string filename = epdf->d_name;
            if(filename.substr(filename.find_last_of(".") + 1) == "xml" || filename.substr(filename.find_last_of(".") + 1) == "xsd") {
                //std::cout << "XML..." << filename.substr(0,filename.find_last_of(".")) << " Length..." << filename.length() << std::endl;

                tinyxml2::XMLElement* uuidsChild = typeChild->NextSiblingElement();
                
                bool isUUidExists = false;
                for (tinyxml2::XMLElement* uuidChild = uuidsChild->FirstChildElement(); uuidChild != NULL; uuidChild = uuidChild->NextSiblingElement())
                {
                    string uuid = uuidChild->GetText();

                    if ( filename.substr(filename.find_last_of(".") + 1) == "xml" && uuid == filename.substr(0,filename.find_last_of(".")))
                    {
                        isUUidExists = true;
                        break;
                    }
                    else if ( filename.substr(filename.find_last_of(".") + 1) == "xsd" && uuid == get_xsd_name(tempObjPathStr + "/" + filename))
                    {
                        isUUidExists = true;
                        break;
                    }
                }

                if ( isUUidExists)
                {
                    std::cout << typeStr << " object [" << filename << "] exists" << endl;
                }
                else
                {
                    std::cout << typeStr << " object [" << filename << "] not exists" << endl;
                    XMLElement *nodeUuid = xml_doc.NewElement("uuid");
                    if ( filename.substr(filename.find_last_of(".") + 1) == "xml")
                        nodeUuid->SetText((filename.substr(0,filename.find_last_of("."))).c_str());
                    else
                        nodeUuid->SetText(get_xsd_name(tempObjPathStr + "/" + filename).c_str());        
                    uuidsChild->InsertEndChild(nodeUuid);
                }

            } else
            {
                if ( filename.compare(".") != 0 && filename.compare("..") != 0 && filename.compare(".DS_Store"))
                std::cout << "ALERT! File excluded - " << filename << endl;
            }
        }
    }
    closedir(dpdf);
    }

    }
    
    if (!gTestFlag)
        xml_doc.SaveFile(argv[2]);


    cout << "Successful" << endl;

    return 0;
}