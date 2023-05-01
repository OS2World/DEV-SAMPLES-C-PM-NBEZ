/*********************************************************************
 *                                                                   *
 * MODULE NAME :  nbez.h                 AUTHOR:  Rick Fishman       *
 * DATE WRITTEN:  12-05-92                                           *
 *                                                                   *
 * DESCRIPTION:                                                      *
 *                                                                   *
 *  Common definitions and function prototypes for NBEZ.EXE          *
 *                                                                   *
 * HISTORY:                                                          *
 *                                                                   *
 *  12-05-92 - Copied source from the NBBASE sample.                 *
 *                                                                   *
 *  Rick Fishman                                                     *
 *  Code Blazers, Inc.                                               *
 *  4113 Apricot                                                     *
 *  Irvine, CA. 92720                                                *
 *  CIS ID: 72251,750                                                *
 *                                                                   *
 *********************************************************************/

#define ID_NBWINFRAME           1
#define ID_NB                   10

#define IDD_PAGE1               1000
#define EF_1                    1001

#define IDD_PAGE2               2000
#define EF_2                    2001

#define IDD_PAGE3               3000
#define EF_3                    3001

#define IDD_PAGE4               4000
#define EF_4                    4001

#define NOTEBOOK_WINCLASS       "NoteBookSample"

#define PROGRAM_TITLE           "Notebook Control Sample Program"

typedef struct _NBPAGE              // VARIABLES USED FOR A NOTEBOOK PAGE
{
    PFNWP    pfnwpDlg;              // Window procedure address for the dialog
    PSZ      szStatusLineText;      // Text to go on status line
    PSZ      szTabText;             // Text to go on major tab
    ULONG    idDlg;                 // ID of the dialog box for this page

} NBPAGE, *PNBPAGE;

/****************************************************************************
 *                        E N D   O F   S O U R C E                         *
 ****************************************************************************/
