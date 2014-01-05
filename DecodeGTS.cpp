// DecodeGTS.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "StationList.h"
#include "BulletinParser.h"
#include "MetUtils.h"
#include "WxData.h"

enum Mode { None, StationTable, OutputDir, LabelName };

int _tmain(int argc, _TCHAR* argv[])
{
    //need a master station table, an list of input files and an output file
    //Usage: DecodeGTS -x -s MasterStationList.txt -o c:\weather -l 2006080112 c:\rawfiles\2006080112.syn c:\rawfiles\2006080112.metar    
    //-x is a flag to set output to xml
	//-s is the location of the master station list
    //-o is the output directory
    //-l is the label to append to the error and decode files produced in the output directory
    //The rest is a list of input files for processing e.g. metars and synops
    Mode mode=None;
	bool isXMLFormat=false; //false=CSV, true=xml output file format
    String^ stationTable; //path and filename of master station list
    ArrayList^ inputFiles=gcnew ArrayList(); //path and filenames of input GTS files
    String^ labelName; //label to append onto output files
    String^ outputDir; //output directory to place processed files into
    for (int i=1; i<argc; i++) {
        String^ arg=gcnew String(argv[i]);
        if (arg[0]=='-') {
            switch (arg[1]) {
				case 'x': isXMLFormat=true; break;
                case 's': mode=StationTable; break;
                case 'o': mode=OutputDir; break;
                case 'l': mode=LabelName; break;
                default: mode=None;
            }
        }
        else {
            //int pos;
            switch (mode) {
                case None:
                    //inputFile=arg;
                    //pos=inputFile->LastIndexOf("\\");
                    //if (pos>=0) filename=inputFile->Substring(pos+1);
                    //else filename=inputFile;
					//arg could be a wildcard, so find the real names of all matching files
					{
						//NOTE: this allows wildcards in the filename but NOT the directory path
						String^ dir=Path::GetDirectoryName(arg);
						String^ wild=Path::GetFileName(arg);
						DirectoryInfo^ di=gcnew DirectoryInfo(dir);
						array<FileInfo^>^ argfiles=di->GetFiles(wild);
						for (IEnumerator^ e=argfiles->GetEnumerator(); e->MoveNext()==true; )
							inputFiles->Add(((FileInfo^)e->Current)->FullName);
						//inputFiles->Add(arg); //old code for no wildcards
					}
                    break;
                case StationTable:
                    stationTable=arg;
                    break;
                case OutputDir:
                    outputDir=arg;
                    if (!outputDir->EndsWith("\\")) outputDir=outputDir+"\\";
                    break;
                case LabelName:
                    labelName=arg;
                    break;
            }
            mode=None;
        }
    }

    //output synop csv file
    CStationList^ WMOStationList=gcnew CStationList(stationTable,true);
    CStationList^ ICAOStationList=gcnew CStationList(stationTable,false);
//Need to fix bulletin parser at some point - it creates a CSV file of all the data, an error file
//and also returns the data in a collection, which is a little inefficient
    CBulletinParser^ BulletinParser=gcnew CBulletinParser(WMOStationList,ICAOStationList);
    Collections::Hashtable^ data=gcnew Collections::Hashtable();

	String^ ext=".csv";
	if (isXMLFormat) ext=".xml";
    StreamWriter^ dataWriter=gcnew StreamWriter(outputDir+labelName+ext);
    IEnumerator^ fileEnum = inputFiles->GetEnumerator();
    while (fileEnum->MoveNext())
    {
        data->Clear();
        Console::WriteLine("Processing "+(String^)fileEnum->Current);
        BulletinParser->ParseFile((String^)fileEnum->Current,data,outputDir);
        System::Collections::IDictionaryEnumerator^ en=data->GetEnumerator();
        while (en->MoveNext())
        {
            //There has to be a better way of doing this. Maybe a check country virtual that
            //doesn't require the data to be cast and the virtual ToCSV could still be used
            //but on the Wx type.
            CWxData^ wx=(CWxData^)en->Value;
            if (wx->GetType()->FullName=="CMetarData")
            {
                CMetarData^ metar=(CMetarData^)wx;
                //filter out only the UK ones
                //if (metar->ICAO->StartsWith("EG"))
                //    dataWriter->WriteLine(metar->ToCSV());

				//if (isXMLFormat) dataWriter->WriteLine(metar->ToXML());
				//else
				dataWriter->WriteLine(metar->ToCSV());
            }
            else if (wx->GetType()->FullName=="CSynopData")
            {
                CSynopData^ synop=(CSynopData^)wx;
                //filter out only the UK ones
                //if ((synop->WMONumber>=3000)&&(synop->WMONumber<4000))
				//filter out only the ship reports
				//if (synop->ReportType==CSynopReportType::BBXX)
				//{
					if (isXMLFormat) dataWriter->WriteLine(synop->ToXML());
					else dataWriter->WriteLine(synop->ToCSV());
					//BulletinParser->WriteToDB(wx);
				//}
            }
        }
		dataWriter->Flush();
    }
    dataWriter->Close();

	return 0;
}

