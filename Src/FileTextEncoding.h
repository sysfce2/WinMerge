/**
 * @file  FileTextEncoding.h
 *
 * @brief Declaration of FileTextEncoding structure
 */
// ID line follows -- this is updated by SVN
// $Id: FileTextEncoding.h 4627 2007-10-15 20:59:45Z kimmov $

#ifndef FileTextEncoding_h_included
#define FileTextEncoding_h_included

/**
 * @brief Text encoding (eg, UTF-8, or CP-1252)
 */
struct FileTextEncoding
{
	int m_codepage; /**< 8bit codepage, if applicable, -1 is unknown or N/A */
	int m_unicoding; /**< Unicode encoding (assumes 0 is none, use values from ucr::CODESET) */
	bool m_bom; /**< Unicode byte marker */
	bool m_guessed; /**< Whether encoding was guessed from content */

	FileTextEncoding();
	void Clear();
	void SetCodepage(int codepage);
	void SetUnicoding(int unicoding);
	String GetName() const;

	static int Collate(const FileTextEncoding & fte1, const FileTextEncoding & fte2);
};


#endif // FileTextEncoding_h_included
