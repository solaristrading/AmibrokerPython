////////////////////////////////////////////////////
// Plugin.h
// Standard header file for all AmiBroker plug-ins
//
// Version 2.1a
///////////////////////////////////////////////////////////////////////
// Copyright (c) 2001-2010 AmiBroker.com. All rights reserved. 
//
// Users and possessors of this source code are hereby granted a nonexclusive, 
// royalty-free copyright license to use this code in individual and commercial software.
//
// AMIBROKER.COM MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE CODE FOR ANY PURPOSE. 
// IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND. 
// AMIBROKER.COM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOURCE CODE, 
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
// IN NO EVENT SHALL AMIBROKER.COM BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, OR 
// CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
// WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
// 
// Any use of this source code must include the above notice, 
// in the user documentation and internal comments to the code.
///////////////////////////////////////////////////////////////////////
//
// Version 1.0 : 2002-11-18 TJ
// Version 2.0 : 2009-07-30 TJ  
//       added support for 
//			* 64-bit date time format 
//			* float volume/open int
//			* 2 user fields (Aux) in Quotation structure
//			* 100 new user fields (fundamentals) in StockInfo
//			* proper alignment for 64-bit platforms (8 byte boundary) 
// Version 2.1 : 2010-03-31 TJ
//          * struct PluginNotification has new field 
//            StockInfo * pCurrentSINew
//            and old field pCurrentSI type has changed to StockInfoFormat4 *
//       
// Version 2.1a: 
//          * added #pragma(pack) to ensure AmiVar struct packing compatible with AmiBroker
// 


////////////////////////////////////////////////////

#ifndef PLUGIN_H
#define PLUGIN_H 1

// Possible types of plugins
// currently 4 types are defined
// PLUGIN_TYPE_AFL - for AFL function plugins
// PLUGIN_TYPE_DATA - for data feed plugins (requires 3.81 or higher)
// PLUGIN_TYPE_AFL_AND_DATA - for combined AFL/Data plugins
// PLUGIN_TYPE_OPTIMIZER - for optimization engine plugins (requires v5.12 or higher)

#define PLUGIN_TYPE_AFL 1
#define PLUGIN_TYPE_DATA 2
#define PLUGIN_TYPE_AFL_AND_DATA 3
#define PLUGIN_TYPE_OPTIMIZER 4

// all exportable functions must have undecorated names 
#ifdef _USRDLL
#define PLUGINAPI extern "C" __declspec(dllexport)
#else
#define PLUGINAPI extern "C" __declspec(dllimport)
#endif


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//
// Structures and functions
// COMMON for all kinds of AmiBroker plugins 
//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// 64-bit date/time integer
#define DATE_TIME_INT unsigned __int64


// define signed and unsigned one byte types
typedef unsigned char UBYTE;
typedef signed char SBYTE;

// useful macros for empty values
#define EMPTY_VAL (-1e10f)
#define IS_EMPTY( x ) ( x == EMPTY_VAL )
#define NOT_EMPTY( x ) ( x != EMPTY_VAL )

#define PIDCODE(a, b, c, d ) ( ((a)<<24) | ((b)<<16) | ((c)<<8) | (d) )

// PluginInfo structure holds
// general information about plugin 
struct PluginInfo {
						int		nStructSize;   // this is sizeof( struct PluginInfo )
						int		nType;		   // plug-in type currently 1 - indicator is the only one supported
						int		nVersion;	   // plug-in version coded to int as MAJOR*10000 + MINOR * 100 + RELEASE
						int		nIDCode;	   // ID code used to uniquely identify the data feed (set it to zero for AFL plugins)
						char	szName[ 64 ];  // long name of plug-in displayed in the Plugin dialog
						char    szVendor[ 64 ];// name of the plug-in vendor
						int		nCertificate;  // certificate code - set it to zero for private plug-ins
						int		nMinAmiVersion;// minimum required AmiBroker version (should be >= 380000 -> AmiBroker 3.8)
				};


// the list of AmiVar types
enum { VAR_NONE, VAR_FLOAT, VAR_ARRAY, VAR_STRING, VAR_DISP };

// AmiVar is a variant-like structure/union
// that holds any AFL value
// type member holds variable type (see VAR_ enum above)
#pragma pack( push, 2 )
typedef struct AmiVar
{
    int type;
    union 
    {
        float   val;
        float   *array;
        char    *string;
		void	*disp;
    };
} AmiVar;
#pragma pack(pop)

///////////////////////////////////////////////////
// COMMON EXPORTED FUNCTONS
//
// Each AmiBroker plug-in DLL must export the following
// functions:
// 1. GetPluginInfo	- called when DLL is loaded
// 2. Init - called when AFL engine is being initialized 
// 3. Release - called when AFL engine is being closed

PLUGINAPI int GetPluginInfo( struct PluginInfo *pInfo );
PLUGINAPI int Init(void);
PLUGINAPI int Release(void);


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//
// Structures and functions
// for AFL Plugins 
//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// SiteInterface structure
// defines call-back function pointers
// the structure is filled with correct
// pointers by the AmiBroker and passed to DLL via SetSiteInterface() function call
// 
// SiteInterface is used as a way to call-back AmiBroker built-in functions
//

struct SiteInterface 
{
				int			nStructSize;
				int			(*GetArraySize) (void);	   
				float *		(*GetStockArray)( int nType );
				AmiVar		(*GetVariable) ( const char *pszName );
				void		(*SetVariable) ( const char *pszName, AmiVar newValue );
				AmiVar		(*CallFunction) ( const char *szName, int nNumArgs, AmiVar *ArgsTable );
				AmiVar		(*AllocArrayResult) (void);
				void *		(*Alloc) (unsigned int nSize);
				void		(*Free) (void *pMemory);
				DATE_TIME_INT* (*GetDateTimeArray) (void);	  // new in 5.30
};

// FunDesc structure
// holds the pointer to actual user-defined function
// that can be called by AmiBroker.
// It holds also the number of array, string, float and default arguments
// for the function and the default values
//
typedef struct FunDesc
{
    AmiVar (*Function)( int NumArgs, AmiVar *ArgsTable );
    UBYTE   ArrayQty;       // number of Array arguments required   
    UBYTE   StringQty;      // number of String arguments required
    SBYTE   FloatQty;       // number of float args 
    UBYTE   DefaultQty;     // number of default float args
    float   *DefaultValues; // the pointer to defaults table 
} FunDesc;


// FunctionTag struct
// holds the Name of the function and the corresponding
// FunDesc structure.
// This structure is used to define function table
// that is retrieved by AmiBroker via GetFunctionTable() call
// when AFL engine is initialized.
// That way new function names are added to the AFL symbol table
// and they become accessible.

typedef struct FunctionTag
{
	char	*Name;
	FunDesc	 Descript;
} FunctionTag;


// Indicator plugin exported functions:
// 1. GetFunctionTable - called when AFL engine is being initialized 
// 1. SetSiteInteface - called when AFL engine is being initialized 
//
// Each function may be called multiple times.
//
// The order of calling functions during intialization is
// as follows:
//
// SetSiteInterface -> GetFunctionTable	-> Init -> 
// ... normal work ....
// Release
//
// This cycle may repeat multiple times
// 
// All functions in the plug in DLL use _cdecl calling convention
// (the default for C compiler)

PLUGINAPI int GetFunctionTable( FunctionTag **ppFunctionTable );
PLUGINAPI int SetSiteInterface( struct SiteInterface *pInterface );

////////////////////////////////////////
// Global-scope data for indicator plugins
////////////////////////////////////////

// FunctionTable should be defined 
// in the implementation file of your functions
extern FunctionTag gFunctionTable[];
extern int		   gFunctionTableSize;

// Site interface is defined in Plugin.cpp
extern struct SiteInterface gSite;




/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//
// Structures and functions
// for DATA FEED plugins 
//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


#define DATE_EOD_TICKS   15
#define DATE_EOD_MINUTES 63
#define DATE_EOD_HOURS   31

#define PERIODICITY_1SEC	 1
#define PERIODICITY_1MIN	60
#define PERIODICITY_1HOUR 3600
#define PERIODICITY_EOD  86400

/* codes for Notify() function */
/* describe reason of notification: database is loaded/unloaded, settings are changed */
#define REASON_DATABASE_LOADED		1
#define REASON_DATABASE_UNLOADED	2
#define REASON_SETTINGS_CHANGE		4
#define REASON_STATUS_RMBCLICK		8	/* user has clicked on plugin status pane */


//
// The WM_USER_STREAMING_UPDATE MESSAGE
// is used to notify AmiBroker that new streaming data
// arrived
//
#define WM_USER_STREAMING_UPDATE (WM_USER + 13000)



////////////////////////////////////
// NEW data structures
// AmiBroker 5.27 and higher
//
// featuring 64 bit date time stamp



#define DAILY_MASK			0x000007FFffffFFC0i64

struct PackedDate {
						// lower 32 bits
						unsigned int IsFuturePad:1;	// bit marking "future data"
						unsigned int Reserved:5;	// reserved set to zero
						unsigned int MicroSec:10;	// microseconds	0..999
						unsigned int MilliSec:10;	// milliseconds	0..999
						unsigned int Second: 6;		// 0..59

						// higher 32 bits
                        unsigned int Minute : 6;    // 0..59                       63 is reserved as EOD marker
                        unsigned int Hour : 5;      // 0..23                       31 is reserved as EOD marker
                        unsigned int Day : 5;       // 1..31
                        unsigned int Month : 4;     // 1..12
                        unsigned int Year : 12;		// 0..4095
                                             
                    };

// 8 byte (64 bit) date time stamp
union AmiDate
{
						DATE_TIME_INT	Date;
						struct PackedDate PackDate;
};


// 40-bytes 8-byte aligned
struct Quotation {
						union AmiDate DateTime;	// 8 byte
                        float   Price;
                        float   Open;
                        float   High;
                        float   Low;
						float   Volume;
						float	OpenInterest;
						float	AuxData1;
						float	AuxData2;
                 };


// legacy (pre-5.27) data plugin structures
// and conversion functions
#include "plugin_legacy.h"



enum { CATEGORY_MARKET, CATEGORY_GROUP, CATEGORY_SECTOR, CATEGORY_INDUSTRY, CATEGORY_WATCHLIST };

struct InfoSite
			{
				int			nStructSize;
				int			(*GetStockQty)( void );
				struct StockInfoFormat4 * (*AddStock)( const char *pszTicker );
				int			(*SetCategoryName)( int nCategory, int nItem, const char *pszName );
				const char *(*GetCategoryName)( int nCategory, int nItem );
			    int			(*SetIndustrySector) ( int nIndustry, int nSector );
				int			(*GetIndustrySector) ( int nIndustry );
				struct StockInfo * (*AddStockNew)( const char *pszTicker );  // only available if called from AmiBroker 5.27 or higher
			};

#define RI_LAST			(1L<<0)
#define RI_OPEN			(1L<<1)
#define RI_HIGHLOW		(1L<<2)
#define RI_TRADEVOL		(1L<<3)
#define RI_TOTALVOL		(1L<<4)
#define RI_OPENINT		(1L<<5)
#define RI_PREVCHANGE	(1L<<6)
#define RI_BID			(1L<<7)
#define RI_ASK			(1L<<8)
#define RI_EPS			(1L<<9)
#define RI_DIVIDEND		(1L<<10)
#define RI_SHARES		(1L<<11)
#define RI_52WEEK		(1L<<12)
#define RI_DATEUPDATE	(1L<<13)
#define RI_DATECHANGE	(1L<<14)

#define RI_STATUS_UPDATE (1L<<0)
#define RI_STATUS_BIDASK (1L<<1)
#define RI_STATUS_TRADE  (1L<<2)
#define RI_STATUS_BARSREADY (1L<<3)
#define RI_STATUS_INCOMPLETE (1L<<4)
#define RI_STATUS_NEW_BID (1L<<5)
#define RI_STATUS_NEW_ASK (1L<<6)

struct RecentInfo
{
	int		nStructSize;
	char	Name[ 64 ];
	char	Exchange[8];
	int		nStatus;
	int		nBitmap;	// describes which fields are valid
	float	fOpen;
	float	fHigh;
	float	fLow;
	float	fLast;
	int		iTradeVol;
	int		iTotalVol;
	float	fOpenInt;
	float	fChange;
	float	fPrev;
	float	fBid;
	int		iBidSize;
	float	fAsk;
	int		iAskSize;
	float	fEPS;
	float	fDividend;
	float	fDivYield;
	int		nShares; // shares outstanding
	float	f52WeekHigh;
	int		n52WeekHighDate;
	float	f52WeekLow;
	int		n52WeekLowDate;
	int		nDateChange; // format YYYYMMDD
	int		nTimeChange; // format HHMMSS
	int		nDateUpdate; // format YYYYMMDD
	int		nTimeUpdate; // format HHMMSS
	float	fTradeVol;	// NEW 5.27 field
	float	fTotalVol;	// NEW 5.27 field 
};

// context structure for GetQuotesEx
struct GQEContext
{
	int		nStructSize;
};

struct PluginStatus
{
	int			nStructSize;
	int			nStatusCode;
	COLORREF	clrStatusColor;
	char		szLongMessage[ 256 ];
	char		szShortMessage[ 32 ];
};


// REQUIRED: GetQuotes function, MUST be exported by every data plugin 
// This is LEGACY function for version 5.26 and below
PLUGINAPI int GetQuotes( LPCTSTR pszTicker, int nPeriodicity, int nLastValid, int nSize, struct QuotationFormat4 *pQuotes );

// GetQuotesEx function is functional equivalent fo GetQuotes but
// handles new Quotation format with 64 bit date/time stamp and floating point volume/open int
// and new Aux fields
// it also takes pointer to context that is reserved for future use (can be null)
// Called by AmiBroker 5.27 and above 
PLUGINAPI int GetQuotesEx( LPCTSTR pszTicker, int nPeriodicity, int nLastValid, int nSize, struct Quotation *pQuotes, GQEContext *pContext );

// GetExtra data is optional function for retrieving non-quotation data
PLUGINAPI AmiVar GetExtraData( LPCTSTR pszTicker, LPCTSTR pszName, int nArraySize, int nPeriodicity, void* (*pfAlloc)(unsigned int nSize) );

// Configure function is called when user presses "Configure" button
// in File->Database Settings
PLUGINAPI int Configure( LPCTSTR pszPath, struct InfoSite *pSite );

// GetRecentInfo function is optional, used only by real-time plugins 
PLUGINAPI struct RecentInfo * GetRecentInfo( LPCTSTR pszTicker );

// new API function, optional, only for RT plugins
PLUGINAPI BOOL IsBackfillComplete( LPCTSTR pszTicker );

// GetSymbolLimit function is optional, used only by real-time plugins
PLUGINAPI int GetSymbolLimit( void );

// GetPluginStatus function is optional, used mostly by few real-time plugins
PLUGINAPI int GetPluginStatus( struct PluginStatus *status );

// SetDatabasePath is now obsolete. 
// You should NOT implement it in newly written plugins.
// Implement Notify() instead.
// This function is called when new database is loaded
PLUGINAPI int SetDatabasePath( LPCTSTR pszPath );

// SetTimeBase function is called when user is changing
// base time interval in File->Database Settings
PLUGINAPI int SetTimeBase( int nTimeBase );

struct _IntradaySettings {
	int					TimeShift; // in hours
	int					FilterAfterHours; //
	DATE_TIME_INT		SessionStart; // bit encoding HHHHH.MMMMMM.0000	 hours << 10 | ( minutes << 4 ) 
	DATE_TIME_INT		SessionEnd;	  // 			  //
	int					FilterWeekends;		   
	int					DailyCompressionMode; // 0 - exchange, 1 - local, 2 -session based
	DATE_TIME_INT		NightSessionStart;
	DATE_TIME_INT		NightSessionEnd;
};


struct _Workspace {
	int		DataSource;	 /* 0 - use preferences, 1 - local, ID of plugin */
	int		DataLocalMode;	/* 0 - use preferences, 1 - store locally, 2 - don't */
	int		NumBars;
	int		TimeBase;
	// the place were OLD intraday settings were located
	int		ReservedB[ 8 ];
	BOOL	AllowMixedEODIntra;
	BOOL	RequestDataOnSave;
	BOOL	PadNonTradingDays;	 
	int		ReservedC;
	struct  _IntradaySettings IS;
	int		ReservedD;
};


#define MAX_SYMBOL_LEN 48
struct StockInfo {		
                        char    ShortName[MAX_SYMBOL_LEN];
						char	AliasName[MAX_SYMBOL_LEN];
						char	WebID[MAX_SYMBOL_LEN];
                        char    FullName[128];
                        char    Address[128];
						char	Country[64];
						char	Currency[4];		/* ISO 3 letter currency code */
						int		DataSource;			/* the ID of the data plug-in, 0 - accept workspace settings */
                        int     DataLocalMode;		/* local mode of operation - 0 - accept workspace settings, 1 - store locally, 2 - don't store locally */
						int		MarketID;
						int		GroupID;
						int		IndustryID;
						int		GICS;
                        int     Flags;          /* continuous etc.*/     
                        int     MoreFlags;          /*  */
						float	MarginDeposit;		/* new futures fields - active if SI_MOREFLAGS_FUTURES is set */
						float	PointValue;
						float	RoundLotSize;
						float	TickSize;		/* new futures fields - active if SI_MOREFLAGS_FUTURES is set */
						int		Decimals;		  /* number of decimal places to display */
						short	LastSplitFactor[ 2 ];	 // /* [ 0 ] - new, [ 1 ] - old */
						DATE_TIME_INT LastSplitDate;	 // at 16-byte boundary
                        DATE_TIME_INT DividendPayDate;
                        DATE_TIME_INT ExDividendDate;         /* div date */
						float	SharesFloat;//                        int     Code;
						float	SharesOut; //int     StockQty;
						float   DividendPerShare;	//
						float   BookValuePerShare;
						float   PEGRatio;			// PE Growth ratio
						float   ProfitMargin;	
						float	OperatingMargin;    
						float	OneYearTargetPrice;
						float	ReturnOnAssets;
						float	ReturnOnEquity;
						float	QtrlyRevenueGrowth;	 /* year over year */
						float	GrossProfitPerShare;
						float	SalesPerShare;	   // ttn Sales Revenue
						float	EBITDAPerShare;
						float	QtrlyEarningsGrowth;
						float	InsiderHoldPercent;
						float	InstitutionHoldPercent;
						float	SharesShort;
						float	SharesShortPrevMonth;
						float	ForwardEPS;	// from Forward P/E
						float	EPS;	// ttm EPS
						float	EPSEstCurrentYear;
						float	EPSEstNextYear;
						float	EPSEstNextQuarter;
						float   ForwardDividendPerShare;            
						float	Beta;	
						float	OperatingCashFlow;
						float	LeveredFreeCashFlow;
						float	ReservedInternal[ 28 ];
						float	UserData[ 100 ];
};

struct PluginNotification
{
	int		nStructSize;
	int		nReason;
	LPCTSTR pszDatabasePath;
	HWND	hMainWnd;
	struct  StockInfoFormat4	*pCurrentSI;
	struct  _Workspace			*pWorkspace;
	struct  StockInfo			*pCurrentSINew;	 // new as of API 2.1 (AmiBroker 5.30.1)
};

// Notify function (optional) is called when database is loaded, unloaded
// settings are changed, or right mouse button in the plugin status area is clicked
PLUGINAPI int Notify( struct PluginNotification *pNotifyData );

/////////////////////////////////////////////////////
// OPTIMIZER PLUGIN API
//////////////////////////////////////////////////////
#pragma pack( push, 4 )

struct OptimizeItem
{
	char	*Name;
	float	Default;
	float	Min;
	float	Max;
	float	Step;
	double	Current;
	float   Best;
};

#define MAX_OPTIMIZE_ITEMS 100 

struct OptimizeParams
{
	int		Mode;				// 0 - gets defaults, 1 - retrieves settings from formula (setup phase), 2 - optimization phase
	int		WalkForwardMode;	// 0 - none (regular optimization), 1-in-sample, 2 - out of sample
	int		Engine;				// optimization engine selected - 0 means - built-in exhaustive search
	int		Qty;				// number of variables to optimize
	int		LastQty;
	BOOL	CanContinue;		// boolean flag 1 - means optimization can continue, 0 - means aborted by pressing "Cancel" in progress dialog or other error
	BOOL	DuplicateCheck;		// boolean flag 1 - means that AmiBroker will first check if same param set wasn't used already
								// and if duplicate is found it won't run backtest, instead will return previously stored value
	int		Reserved;
	char	*InfoText;			// pointer to info text buffer (providing text display in the progress dialog)
	int		InfoTextSize;		// the size (in bytes) of info text buffer
	__int64	Step;				// current optimization step (used for progress indicator) - automatically increased with each iteration
	__int64	NumSteps;			// total number of optimization steps (used for progress indicator)
	double  TargetCurrent;
	double  TargetBest;
	int		TargetBestStep;		// optimization step in which best was achieved
	struct	OptimizeItem	Items[ MAX_OPTIMIZE_ITEMS ]; // parameters to optimize
};

#pragma pack( pop )


//////////////////////////////////////////////
// Optimizer Engine interface
PLUGINAPI int OptimizerInit( struct OptimizeParams *pParams );
PLUGINAPI int OptimizerRun( struct OptimizeParams *pParams, double (*pfEvaluateFunc)( void * ), void *pContext );
PLUGINAPI int OptimizerFinalize( struct OptimizeParams *pParams );
PLUGINAPI int OptimizerSetOption( const char *pszParam, AmiVar newValue );

///////////////////////////////////////////////////

#endif