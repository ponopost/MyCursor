// mycsr.h

// resource
#define IDC_STATIC		-1
#define IDD_ARROWLIST	1000
#define IDD_IBEAMLIST	1001
#define IDD_NOICON		1002
#define IDD_LOCKRES		1003
#define IDD_IBEAMAREA	1004
#define IDD_ARROWL		2000
#define IDD_ARROWM		2001
#define IDD_ARROWS		2002
#define IDD_ARROWLL		2003
#define IDD_ARROWML		2004
#define IDD_ARROWSL		2005
#define IDD_ARROW3D1	2006
#define IDD_ARROW3D2	2007
#define IDD_IBEAML		3000
#define IDD_IBEAMM		3001
#define IDD_IBEAMS		3002
#define IDD_IBEAM1		3003
#define IDD_IBEAM2		3004
#define IDD_IBEAM3		3005
#define IDD_IBEAM4		3006

// prototype
LRESULT CALLBACK MainWndProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK SetupDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL LoadIniFile( VOID );
BOOL SaveIniFile( VOID );
BOOL LoadSystemCursor( VOID );
BOOL SaveSystemCursor( VOID );
BOOL CursorChangeArrow( int );
BOOL CursorChangeIBeam( int );
BOOL MemoryToCursor( LPBYTE, LPCSTR );
BOOL CursorToMemory( LPCSTR, LPBYTE );

// [EOF]
