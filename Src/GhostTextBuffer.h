/**
 * @file  GhostTextBuffer.h
 *
 * @brief Declaration of CGhostTextBuffer (subclasses CCrystalTextBuffer to handle ghost lines)
 */
// ID line follows -- this is updated by SVN
// $Id: GhostTextBuffer.h 5514 2008-06-24 08:47:25Z kimmov $

#ifndef __GHOSTTEXTBUFFER_H__
#define __GHOSTTEXTBUFFER_H__

#include "ccrystaltextbuffer.h"


/////////////////////////////////////////////////////////////////////////////

/**
 * We use the current ccrystalEditor flags 
 *
 * This flag must be cleared and set in GhostTextBuffer.cpp 
 * and MergeDoc.cpp (Rescan) only.
 *
 * GetLineColors (in MergeEditView) reads it to choose the line color.
 */
enum GHOST_LINEFLAGS
{
	LF_GHOST = 0x00400000L, /**< Ghost line. */
};

/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer command target

/**
 * @brief A class handling ghost lines.
 * Features offered with this class : 
 * <ul>
 *  <li> apparent/real line conversion 
 *  <li> insertText/deleteText working with ghost lines 
 *  <li> AddUndoRecord/Undo/Redo working with ghost lines 
 *  <li> insertGhostLine function 
 * </ul>
 */
class EDITPADC_CLASS CGhostTextBuffer : public CCrystalTextBuffer
{
public:
	DECLARE_DYNCREATE (CGhostTextBuffer)

protected :
#pragma pack(push, 1)
	//  Nested class declarations
	struct SLineInfo
	{
		TCHAR *m_pcLine;
		int m_nLength, m_nMax;
		int m_nEolChars; // # of eolchars
		DWORD m_dwFlags;
		DWORD m_dwRevisionNumber;

		int FullLength() const { return m_nLength + m_nEolChars; }
		int Length() const { return m_nLength; }

		SLineInfo ()
		{
			memset (this, 0, sizeof (SLineInfo));
		};
	};

	enum
	{
		UNDO_INSERT = 0x0001,
		UNDO_BEGINGROUP = 0x0100
	};

	/** 
	Support For Descriptions On Undo/Redo Actions

	We need a structure to remember richer information position
	and the number of real lines inserted/deleted (to set ghost lines during undo)

	This flags are parameters of AddUndoRecord ; so AddUndoRecord
	is not the virtual version of CCrystalTextBuffer::AddUndoRecord

	The text is duplicated (already in CCrystalTextBuffer::SUndoRecord),
	and it is not useful. If someone finds a clean way to correct this...
	*/
	struct SUndoRecord
	{
		DWORD m_dwFlags;

		// Undo records store file line numbers, not screen line numbers
		// File line numbers do not count ghost lines
		// (ghost lines are lines with no text and no EOL chars, which are
		// used by WinMerge as left-only or right-only placeholders)
		// All the stored line number needed are real !

		CPoint m_ptStartPos, m_ptEndPos;  // Block of text participating
		int m_ptStartPos_nGhost, m_ptEndPos_nGhost;

		// Redo records store file line numbers, not screen line numbers
		// they store the file number of the previous real line
		// and (apparentLine - ComputeApparentLine(previousRealLine))

		CPoint m_redo_ptStartPos, m_redo_ptEndPos;  // Block of text participating
		int m_redo_ptStartPos_nGhost, m_redo_ptEndPos_nGhost;

		int m_nRealLinesCreated;         //  number of lines created during insertion 
		                                 //  (= total of real lines after - total before)
		int m_nRealLinesInDeletedBlock;  //  number of real lines in the deleted block 
		                                 // (<> total of real lines after - total before  
		                                 //  as first/end line may be just truncated, not removed)
		int m_nAction;                   //  For information only: action type
		CDWordArray *m_paSavedRevisonNumbers;

private :
		// TCHAR   *m_pcText;
		// Since in most cases we have 1 character here,
		// we should invent a better way. Note: 2 * sizeof(WORD) <= sizeof(TCHAR*)
		//
		// Here we will use the following trick: on Win32 platforms high-order word
		// of any pointer will be != 0. So we can store 1 character strings without
		// allocating memory.
		//
		struct TextBuffer
		{
			int size;
			TCHAR data[1];
		};
		union
		{
			TextBuffer *m_pszText; // For cases when we have > 1 character strings
			TCHAR m_szText[2];     // For single-character strings

		};

public :
		SUndoRecord () // default constructor
		{
			memset (this, 0, sizeof (SUndoRecord));
		}
		SUndoRecord (const SUndoRecord & src) // copy constructor
		{
			memset (this, 0, sizeof (SUndoRecord));
			(*this)=src;
		}
		SUndoRecord & operator=(const SUndoRecord & src) // copy assignment
		{
			m_dwFlags = src.m_dwFlags;
			m_ptStartPos = src.m_ptStartPos;
			m_ptStartPos_nGhost = src.m_ptStartPos_nGhost;
			m_ptEndPos = src.m_ptEndPos;
			m_ptEndPos_nGhost = src.m_ptEndPos_nGhost;
			m_nAction = src.m_nAction;
			m_redo_ptStartPos = src.m_redo_ptStartPos;
			m_redo_ptStartPos_nGhost = src.m_redo_ptStartPos_nGhost;
			m_redo_ptEndPos = src.m_redo_ptEndPos;
			m_redo_ptEndPos_nGhost = src.m_redo_ptEndPos_nGhost;
			m_nRealLinesCreated = src.m_nRealLinesCreated;
			m_nRealLinesInDeletedBlock = src.m_nRealLinesInDeletedBlock;
			SetText(src.GetText(), src.GetTextLength());
			INT_PTR size = src.m_paSavedRevisonNumbers->GetSize();
			if (!m_paSavedRevisonNumbers)
				m_paSavedRevisonNumbers = new CDWordArray();
			m_paSavedRevisonNumbers->SetSize(size);
			INT_PTR i;
			for (i = 0; i < size; i++)
				(*m_paSavedRevisonNumbers)[i] = (*src.m_paSavedRevisonNumbers)[i];
			return *this;
		}
		~SUndoRecord () // destructor
		{
			FreeText();
			if (m_paSavedRevisonNumbers)
				delete m_paSavedRevisonNumbers;
		}

		void SetText (LPCTSTR pszText, int cchText);
		void FreeText ();

		LPCTSTR GetText () const
		{
			// See the m_szText/m_pszText definition
			// Check if m_pszText is a pointer by removing bits having
			// possible char value
			if (((INT_PTR)m_pszText >> 16) != 0)
				return m_pszText->data;
			return m_szText;
		}
		int GetTextLength () const
		{
			if (((INT_PTR)m_pszText >> 16) != 0)
				return m_pszText->size;
			return 1;
		}
	};

#pragma pack(pop)

protected:
	/** 
	We need another array with our richer structure.

	We share the positions with the CCrystalTextBuffer object. 
	We share m_bUndoGroup, its utility is to check we opened the UndoBeginGroup.
	We share m_nUndoBufSize which is the max buffer size.
	*/
	CArray < SUndoRecord, SUndoRecord & >m_aUndoBuf;
	/** 
	This one must be duplicated because the flag UNDO_BEGINGROUP needs to be set in both 
	CGhostTextBuffer::m_aUndoBuf and CCrystalTextBuffer::m_aUndoBuf CArrays 
	*/
	BOOL m_bUndoBeginGroup;

	// [JRT] Support For Descriptions On Undo/Redo Actions
	virtual void AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos,
                              LPCTSTR pszText, int cchText, int nRealLinesChanged, int nActionType = CE_ACTION_UNKNOWN, CDWordArray *paSavedRevisonNumbers = NULL);

private:
	// A RealityBlock is a block of lines with no ghost lines
	struct RealityBlock { int nStartReal; int nStartApparent; int nCount; };
	// The array of reality blocks is kept in order
	CArray < RealityBlock, RealityBlock& > m_RealityBlocks;



	// Operations
private:
	BOOL InternalInsertGhostLine (CCrystalTextView * pSource, int nLine);
	BOOL InternalDeleteGhostLine (CCrystalTextView * pSource, int nLine, int nCount);
public :
	// Construction/destruction code
	CGhostTextBuffer ();
	virtual BOOL InitNew (CRLFSTYLE nCrlfStyle = CRLF_STYLE_DOS);

	/** 
	This should work in base code as ghost lines are real empty lines
	but maybe it doesn't (if there is an assert to check there is an EOL,
	or if it adds the default EOL)
	*/
	virtual void GetTextWithoutEmptys (int nStartLine, int nStartChar,
			int nEndLine, int nEndChar, CString &text,
			CRLFSTYLE nCrlfStyle =CRLF_STYLE_AUTOMATIC);


	// Text modification functions
	virtual BOOL InsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText, int cchText, int &nEndLine, int &nEndChar, int nAction = CE_ACTION_UNKNOWN, BOOL bHistory =TRUE);
	virtual BOOL DeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int nAction = CE_ACTION_UNKNOWN, BOOL bHistory =TRUE);
	BOOL InsertGhostLine (CCrystalTextView * pSource, int nLine);

	// Undo/Redo
	virtual BOOL Undo (CCrystalTextView * pSource, CPoint & ptCursorPos);
	virtual BOOL Redo (CCrystalTextView * pSource, CPoint & ptCursorPos);

	// Undo grouping
	virtual void BeginUndoGroup (BOOL bMergeWithPrevious = FALSE);
	virtual void FlushUndoGroup (CCrystalTextView * pSource);

public:
	/** 
	Code for mapping between file line numbers (real line numbers)
	and screen line numbers (apparent line numbers).

	This is needed to handle ghost lines (ones with no text or EOL chars)
	which WinMerge uses for left-only or right-only lines.
	*/
	int ApparentLastRealLine() const;
	int ComputeRealLine(int nApparentLine) const;
	int ComputeApparentLine(int nRealLine) const;
	/** richer position information   yApparent = apparent(yReal) - yGhost */
	int ComputeRealLineAndGhostAdjustment(int nApparentLine, int& decToReal) const;
	/** richer position information   yApparent = apparent(yReal) - yGhost */
	int ComputeApparentLine(int nRealLine, int decToReal) const;

	/** for loading file */
	void FinishLoading();
	/** for saving file */ 
	void RemoveAllGhostLines();


private:
	void RecomputeRealityMapping();
	/** 
	Code to set EOL, if the status ghost/real of the line changes 

	We should call a CCrystalTextBuffer function to add the correct EOL
	(if CCrystalTextBuffer keeps the default EOL for the file)
	*/
	void RecomputeEOL(CCrystalTextView * pSource, int nStartLine, int nEndLine);
	/** For debugging purpose */
	void checkFlagsFromReality(BOOL bFlag) const;

protected:
	virtual void OnNotifyLineHasBeenEdited(int nLine);


protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrystalTextBuffer)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CCrystalTextBuffer)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP ()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


#endif //__GHOSTTEXTBUFFER_H__
