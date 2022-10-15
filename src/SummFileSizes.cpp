#include "SummFileSizes.h"

SummFileSizes::SummFileSizes()
{
    //ctor
}
SummFileSizes::~SummFileSizes()
{
    //dtor
}


void SummFileSizes::addSize(const wxULongLong& n, size_t n2)
{
//	std::lock_guard<std::mutex> guard(g_queue_mutex);
    TotalSize+= n;
    TotalSize+= n2;
}

void SummFileSizes::addFileCount(int n){
//	std::lock_guard<std::mutex> guard(g_queue_mutex);
    FileCount+=n;
}

void SummFileSizes::addDirCount(int n){
//	std::lock_guard<std::mutex> guard(g_queue_mutex);
    DirCount+=n;
}


wxString SummFileSizes::PrintSizedHuman( bool Fit , unsigned short maxElements, bool Compact2 )
{
    ///returns human readable size in TB, GB, MB, KB, B
    ///Compact2 compacts the display of max 2 elements to format 1.1
    if(Compact2 && maxElements!=2) Compact2=0;
    unsigned short maxElements_used = 0, Compact2_element=0;

	auto TotalSizeCopy= TotalSize;
    wxString returnString;

    wxULongLong splitBytes[5];
    splitBytes[4] =  (TotalSizeCopy/amTB);
    splitBytes[3] = (TotalSizeCopy%amTB)/amGB;
    splitBytes[2] = (TotalSizeCopy%amGB)/amMB;
    splitBytes[1] = (TotalSizeCopy%amMB)/amKB;
    splitBytes[0] = (TotalSizeCopy%amKB);

    for(int i_name=4; i_name>=0; i_name--){
        if(Fit && splitBytes[i_name]==0 ){
        } else {
            if(maxElements_used<maxElements)
            {
                maxElements_used++;
                if(maxElements_used!=1 && !Compact2) returnString<< ", ";
                if(!Compact2 || maxElements_used==1) returnString<< splitBytes[i_name];
                else  returnString<< int( (float(splitBytes[i_name].ToDouble())/1024)*10 );
                if(!Compact2) returnString<< " " + Names[i_name];
                if(Compact2 && maxElements_used==1){
                    returnString<< ".";
                    Compact2_element = i_name;
                }
            }

        }
    }
    if(Compact2){
        returnString<< " " +Names[Compact2_element];
    }
	if(Fit && TotalSize==0) returnString= "0 B";
	return returnString;
}

void SummFileSizes::setSize(const wxULongLong& n)
{///just a wrapper for TotalSize
	TotalSize= n;
}

bool SummFileSizes::IsNotZero()
{
    if(TotalSize!=0) return 1;
    return 0;
}

