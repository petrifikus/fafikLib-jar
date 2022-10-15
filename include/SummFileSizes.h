#ifndef SUMMFILESIZES_H
#define SUMMFILESIZES_H

//#include <mutex>
class SummFileSizes
{
 public:
	wxULongLong TotalSize=0;
	unsigned FileCount=0;
	unsigned DirCount=0;

	void addSize(const wxULongLong& n, size_t n2=0);
	void setSize(const wxULongLong& n);
	void addFileCount(int n=1);
	void addDirCount(int n=1);
	wxString PrintSizedHuman( bool Fit=1 , unsigned short maxElements=-1, bool Compact2=0 );
	bool IsNotZero();
	bool IsDivide_able(){return IsNotZero();};

	 ///just a wrapper for TotalSize
	wxULongLong getSize() const { return TotalSize; }


	SummFileSizes& operator = (const wxULongLong& n){TotalSize= n; return *this;}
	SummFileSizes& operator = (const wxFileOffset& n){TotalSize= n; return *this;}
	SummFileSizes& operator += (const wxFileOffset& n){TotalSize+= n; return *this;}
	SummFileSizes& operator += (const size_t& n){TotalSize+= n; return *this;}

	SummFileSizes();
	virtual ~SummFileSizes();
 private:
	const unsigned amKB = 1024;
	const unsigned amMB = amKB*1024;
	const wxULongLong amGB = amMB*1024;
	const wxULongLong amTB = amGB*1024;
	wxString Names[5] = {"B","KB","MB","GB","TB"};
};

#endif // SUMMFILESIZES_H
