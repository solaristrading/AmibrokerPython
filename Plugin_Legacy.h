////////////////////////////////////////
// Plugin_Legacy.h 
// Legacy data structures
//
// Version 2.1 
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

#ifndef PLUGIN_LEGACY_H
#define PLUGIN_LEGACY_H 1

#pragma pack( push, 2 )


////////////////////////////////////////
// Legacy data structures
//////////////////////////////////////////////
// The OLD formats for quotation and stock info structures
// was used by AmiBroker 5.26.x and earlier
////////////////////////////////////////

#define MAX_SYMBOL_LEN_FORMAT4 26


struct PackedDateFormat4 {
                        unsigned int Tick : 4;      // 0..11 - Tick is 5 seconds   15 is reserved as EOD marker
                        unsigned int Minute : 6;    // 0..59                       63 is reserved as EOD marker
                        unsigned int Hour : 5;      // 0..23                       31 is reserved as EOD marker
                        unsigned int Day : 5;       // 1..31
                        unsigned int Month : 4;     // 1..12
                        unsigned int Year : 8;  // 0..255
                                                // year is stored with as number ( real_year - 1900 )
                                                // so effective range is 1900...2155
                    };

struct QuotationFormat4 {
						union
                        {
                            struct  PackedDateFormat4  PackDate;   // Packed date
                            unsigned int        Date;
                        };
                        float   Price;
                        float   Open;
                        float   High;
                        float   Low;
                        int     Volume;
						union
						{
							struct
							{
								int     OpenInterest;
								UBYTE   Flags;
								UBYTE   PercentReduc;
							};

							INT_PTR SignalPtr;
						};

                 };

struct StockInfoFormat4 {
						float	MarginDeposit;		/* new futures fields - active if SI_MOREFLAGS_FUTURES is set */
						float	PointValue;
						float	RoundLotSize;
                        char    LastSplitFactor[ 2 ];  /* [ 0 ] - new, [ 1 ] - old */
                        char    FullName[64];
                        char    Address[128];
						char	Country[40];
						char	Currency[4];
						char	ReservedA;
						char	LastSplitYear; // 0..255 = Year - 1900 => 1900..2155
						char	LastSplitMonth;	// 1..12
						char	LastSplitDay;  // 1..31
						char	AliasName[16];
                        char    ShortName[MAX_SYMBOL_LEN_FORMAT4];
                        int     Flags;          /* MarketID etc.*/      
						float	SharesFloat;//                        int     Code;
                        float	SharesOut; //int     StockQty;
                        unsigned int  DividendPayDate;
                        float   BookValuePerShare;
                        unsigned int ExDividendDate;         /* div date */
                        float   DividendPerShare;	//
                        float   PEGRatio;			// PE Growth ratio
                        float   ProfitMargin;	
                        float	OperatingMargin;    
                        float	OneYearTargetPrice;
						int		IndustryID;
                        int     MoreFlags;          /* 0x80000000 - wartoôci w tysiâcach v2.52 */
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
						float	TickSize;		/* new futures fields - active if SI_MOREFLAGS_FUTURES is set */
#ifdef _M_X64
						char    WebID[8]; // ON 64 BIT PLATFORM THIS HAS TO BE 8 due to alignment requirements
#else
						char    WebID[6]; // ON 32 BIT PLATFORM THIS HAS TO BE 6 !!!!
#endif
						float   ForwardDividendPerShare;            /* zysk brutto kwart */
						int		DataSource;			/* the ID of the data plug-in, 0 - accept workspace settings */
                        int     DataLocalMode;		/* local mode of operation - 0 - accept workspace settings, 1 - store locally, 2 - don't store locally */
               			float	Beta;	
						float	OperatingCashFlow;
						float	LeveredFreeCashFlow;
						int		WatchListBits;		/* 32 watch lists */
						int		WatchListBits2;		/* another 32 watch lists */
					};


struct _IntradaySettingsFormat4 {
	int		TimeShift; // in hours
	int		FilterAfterHours; //
	unsigned int		SessionStart; // bit encoding HHHHH.MMMMMM.0000	 hours << 10 | ( minutes << 4 ) 
	unsigned int		SessionEnd;	  // 			  //
	BOOL	FilterWeekends;
	int		DailyCompressionMode; // 0 - exchange, 1 - local, 2 -session based
	unsigned int		NightSessionStart;
	unsigned int		NightSessionEnd;
};


struct _WorkspaceFormat4 {
	int		DataSource;	 /* 0 - use preferences, 1 - local, ID of plugin */
	int		DataLocalMode;	/* 0 - use preferences, 1 - store locally, 2 - don't */
	int		NumBars;
	int		TimeBase;
	struct  _IntradaySettingsFormat4 IS;
	BOOL	AllowMixedEODIntra;
	int		Reserved[ 16 ];
};

struct PluginNotificationFormat4
{
	int		nStructSize;
	int		nReason;
	LPCTSTR pszDatabasePath;
	HWND	hMainWnd;
	struct  StockInfoFormat4	*pCurrentSI;
	struct  _WorkspaceFormat4	*pWorkspace;
};

///////////// OLD STRUCTURES (PRE 4.53) /////////////

struct _WorkspaceOldFormat4 {
	int		DataSource;	 /* 0 - use preferences, 1 - local, ID of plugin */
	int		DataLocalMode;	/* 0 - use preferences, 1 - store locally, 2 - don't */
	int		NumBars;
	int		TimeBase;
	int		TimeShift; // in hours
	BOOL	FilterAfterHours; //
	int		SessionStart; // bit encoding HHHHH.MMMMMM.0000	 hours << 10 | ( minutes << 4 ) 
	int		SessionEnd;	  // 			  //
	BOOL	FilterWeekends;
};

struct PluginNotificationOldFormat4
{
	int		nStructSize;
	int		nReason;
	LPCTSTR pszDatabasePath;
	HWND	hMainWnd;
	struct  StockInfoFormat4	*pCurrentSI;
	struct  _WorkspaceOldFormat4	*pWorkspace;
};


/////////////////////////////////////////////////////////////////////
// End of legacy data structures
/////////////////////////////////////////////////////////////////////
#pragma pack( pop )


///////////////////////////////
//  Quote format conversion functions (old (format4) 32 bit date/time <-> new (format5), 64 bit date/time )
///////////////////////////////

inline void ConvertFormat4DateTime( unsigned int *src, DATE_TIME_INT *dst )
{
	PackedDateFormat4 *format4 = (PackedDateFormat4 *) src;
	PackedDate *format5 =		 (PackedDate *) dst;

	*dst = 0;
	format5->Year	= 1900 + format4->Year;
	format5->Month	= format4->Month;
	format5->Day	= format4->Day;
	format5->Hour	= format4->Hour;
	format5->Minute = format4->Minute;
	format5->Second = format4->Tick == 15 ? 63 : ( format4->Tick * 5 );
};

inline void ConvertFormat5DateTime( DATE_TIME_INT *src, unsigned int *dst )
{
	PackedDate *format5 =		 (PackedDate *) src;
	PackedDateFormat4 *format4 = (PackedDateFormat4 *) dst;

	format4->Year = format5->Year - 1900;
	format4->Month = format5->Month;
	format4->Day = format5->Day;
	format4->Hour = format5->Hour;
	format4->Minute = format5->Minute;
	format4->Tick = format5->Second == 63 ? 15 : ( format5->Second / 5 );
};

inline void ConvertFormat4Quote( QuotationFormat4 *src, Quotation *dst )
{
	ConvertFormat4DateTime( &src->Date,	&dst->DateTime.Date );
	dst->Open = src->Open;
	dst->High = src->High;
	dst->Low = src->Low;
	dst->Price = src->Price;
	dst->OpenInterest = (float) src->OpenInterest;
	dst->Volume = (float) src->Volume;
}

inline void ConvertFormat5Quote( Quotation *src, QuotationFormat4 *dst )
{
	ConvertFormat5DateTime( &src->DateTime.Date, &dst->Date );
	dst->Open = src->Open;
	dst->High = src->High;
	dst->Low = src->Low;
	dst->Price = src->Price;
	dst->OpenInterest = (int) src->OpenInterest;
	dst->Volume = (int) src->Volume;
}


#endif