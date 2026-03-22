#include "pch.h"
#include "DiffContext.h"
#include "CompareStats.h"
#include "DiffThread.h"
#include "DiffWrapper.h"
#include "FileFilterHelper.h"
#include "FilterErrorMessages.h"
#include "DirScan.h"
#include "paths.h"
#include "DirItem.h"
#include <iostream>
#include <Poco/Thread.h>
#include <ctime>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

std::vector<std::wstring> ParseQuotedArgs(const std::wstring& input)
{
	std::vector<std::wstring> tokens;
	std::wstring current;
	bool inQuotes = false;

	for (size_t i = 0; i < input.length(); ++i)
	{
		wchar_t ch = input[i];
		if (ch == L'"')
		{
			inQuotes = !inQuotes;
			if (!inQuotes && !current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
		}
		else if (iswspace(ch) && !inQuotes)
		{
			if (!current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += ch;
		}
	}

	if (!current.empty())
		tokens.push_back(current);

	return tokens;
}

std::wstring FormatFileSize(uint64_t size)
{
	if (size == DirItem::FILE_SIZE_NONE)
		return L"N/A";

	// Use locality's localized formatting if needed, or simple format
	if (size < 1024)
		return std::to_wstring(size) + L" B";
	else if (size < 1024 * 1024)
		return std::to_wstring(size / 1024) + L" KB";
	else if (size < 1024 * 1024 * 1024)
		return std::to_wstring(size / (1024 * 1024)) + L" MB";
	else
		return std::to_wstring(size / (1024 * 1024 * 1024)) + L" GB";
}

std::wstring FormatDateTime(const Poco::Timestamp& timestamp)
{
	if (timestamp == Poco::Timestamp::TIMEVAL_MIN)
		return L"N/A";

	time_t rawtime = timestamp.epochTime();
	struct tm timeinfo;
	localtime_s(&timeinfo, &rawtime);

	wchar_t buffer[80];
	wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &timeinfo);
	return buffer;
}

std::wstring GetStatusAbbr(const DIFFITEM& di, int nDirs)
{
	if (di.diffcode.isResultError())
		return L"Error";
	else if (di.diffcode.isResultAbort())
		return L"Aborted";
	else if (di.diffcode.isResultFiltered())
		return L"Skipped";
	else if (di.diffcode.isSideFirstOnly())
		return L"Left Only";
	else if (di.diffcode.isSideSecondOnly())
		return (nDirs < 3) ? L"Right Only" : L"Middle Only";
	else if (di.diffcode.isSideThirdOnly())
		return L"Right Only";
	else if (nDirs > 2 && !di.diffcode.existsFirst())
		return L"No Left";
	else if (nDirs > 2 && !di.diffcode.existsSecond())
		return L"No Middle";
	else if (nDirs > 2 && !di.diffcode.existsThird())
		return L"No Right";
	else if (di.diffcode.isResultSame())
		return L"Identical";
	else if (di.diffcode.isResultDiff())
		return L"Different";
	return L"Unknown";
}

void PrintDiffItemTree(DIFFITEM* pdi, int indent, int nDirs, bool showDetails = false)
{
	if (!pdi)
		return;

	std::wstring indentStr(indent * 2, L' ');
	std::wstring itemName;

	// Get item name
	if (nDirs < 3)
	{
		if (pdi->diffcode.existsFirst())
			itemName = pdi->diffFileInfo[0].filename;
		else if (pdi->diffcode.existsSecond())
			itemName = pdi->diffFileInfo[1].filename;
	}
	else
	{
		if (pdi->diffcode.existsFirst())
			itemName = pdi->diffFileInfo[0].filename;
		else if (pdi->diffcode.existsSecond())
			itemName = pdi->diffFileInfo[1].filename;
		else if (pdi->diffcode.existsThird())
			itemName = pdi->diffFileInfo[2].filename;
	}

	std::wstring status = GetStatusAbbr(*pdi, nDirs);
	std::wstring typeStr = pdi->diffcode.isDirectory() ? L"[DIR] " : L"[FILE]";
	std::wcout << indentStr << typeStr << L" " << itemName << L": " << status;

	if (showDetails)
	{
		std::wcout << L" | ";

		// Show size information with professional formatting
		if (!pdi->diffcode.isDirectory())
		{
			if (nDirs < 3)
			{
				if (pdi->diffcode.existsFirst())
					std::wcout << L"L:" << FormatFileSize(pdi->diffFileInfo[0].size) << L" ";
				if (pdi->diffcode.existsSecond())
					std::wcout << L"R:" << FormatFileSize(pdi->diffFileInfo[1].size) << L" ";
			}
			else
			{
				if (pdi->diffcode.existsFirst())
					std::wcout << L"L:" << FormatFileSize(pdi->diffFileInfo[0].size) << L" ";
				if (pdi->diffcode.existsSecond())
					std::wcout << L"M:" << FormatFileSize(pdi->diffFileInfo[1].size) << L" ";
				if (pdi->diffcode.existsThird())
					std::wcout << L"R:" << FormatFileSize(pdi->diffFileInfo[2].size) << L" ";
			}
			std::wcout << L"| ";
		}

		// Show date information with WinMerge's professional formatting
		if (nDirs < 3)
		{
			if (pdi->diffcode.existsFirst())
				std::wcout << L"L:" << FormatDateTime(pdi->diffFileInfo[0].mtime) << L" ";
			if (pdi->diffcode.existsSecond())
				std::wcout << L"R:" << FormatDateTime(pdi->diffFileInfo[1].mtime) << L" ";
		}
		else
		{
			if (pdi->diffcode.existsFirst())
				std::wcout << L"L:" << FormatDateTime(pdi->diffFileInfo[0].mtime) << L" ";
			if (pdi->diffcode.existsSecond())
				std::wcout << L"M:" << FormatDateTime(pdi->diffFileInfo[1].mtime) << L" ";
			if (pdi->diffcode.existsThird())
				std::wcout << L"R:" << FormatDateTime(pdi->diffFileInfo[2].mtime) << L" ";
		}

		// Show diff counts
		if (pdi->diffcode.isResultDiff() && !pdi->diffcode.isDirectory())
		{
			std::wcout << L"| Diffs:" << pdi->nsdiffs;
			if (pdi->nidiffs > 0)
				std::wcout << L" Ignored:" << pdi->nidiffs;
		}
	}

	std::wcout << std::endl;

	DIFFITEM* child = pdi->GetFirstChild();
	while (child)
	{
		PrintDiffItemTree(child, indent + 1, nDirs, showDetails);
		child = child->GetFwdSiblingLink();
	}
}

int main()
{
#ifdef _MSC_VER
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int dm = CMP_CONTENT; // Default compare method
	PathContext paths(_T(""), _T("")); // Default empty paths
	FileFilterHelper filter;
	filter.SetMaskOrExpression(_T("*.*"));
	std::unique_ptr<CDiffContext> pCtx; // Store context for comparison results

	std::wcout << L"WinMerge folder comparison test tool\n";
	std::wcout << L"Type 'h' for help.\n";

	std::wstring cmd;
	while (true)
	{
		std::wcout << L"> ";
		std::getline(std::wcin, cmd);
		if (cmd.empty())
			continue;

		if (cmd[0] == L'q')
		{
			break;
		}
		if (cmd[0] == L'h')
		{
			std::wcout << L"\nAvailable commands:\n";
			std::wcout << L"  p <left-path> <right-path>   : Set folder paths to compare\n";
			std::wcout << L"  f <filter-mask>              : Set file mask filter (e.g., *.c;*.h)\n";
			std::wcout << L"  m <compare-method>           : Set compare method (FullContents, Date, etc.)\n";
			std::wcout << L"  c                            : Start folder comparison\n";
			std::wcout << L"  d [detail]                   : Display comparison results (add 'detail' for Date, Size, Diff count)\n";
			std::wcout << L"  q                            : Quit the program\n";
			std::wcout << L"  h                            : Show this help message\n\n";
		}
		else if (cmd[0] == L'p') // Set path
		{
			std::vector<std::wstring> args = ParseQuotedArgs(cmd.substr(2));
			if (args.size() == 2 || args.size() == 3)
			{
				paths = PathContext(args);
			}
			else
			{
				std::wcout << L"Usage: p \"left_path\" \"right_path\"\n";
			}
		}
		else if (cmd[0] == L'f') // Set file mask
		{
			std::wstring mask = cmd.substr(2);
			filter.SetMaskOrExpression(mask.c_str());
			if (filter.GetErrorList().size() > 0)
			{
				for (const auto*error : filter.GetErrorList())
					std::wcout << FormatFilterErrorSummary(*error) << "\n";
			}
		}
		else if (cmd[0] == L'm') // Set method
		{
			std::wstring method = cmd.substr(2);
			if (method == L"FullContents") dm = CMP_CONTENT;
			else if (method == L"QuickContents") dm = CMP_QUICK_CONTENT;
			else if (method == L"BinaryContents") dm = CMP_BINARY_CONTENT;
			else if (method == L"Date") dm = CMP_DATE;
			else if (method == L"DateSize") dm = CMP_DATE_SIZE;
			else if (method == L"Size") dm = CMP_SIZE;
			else if (method == L"Existence") dm = CMP_EXISTENCE;
			else {
				std::wcout << L"Unknown compare method\n";
				continue;
			}
		}
		else if (cmd[0] == L'c') // Compare
		{
			CompareStats cmpstats(paths.GetSize());

			pCtx = std::make_unique<CDiffContext>(paths, dm);

			DIFFOPTIONS options = {0};
			options.nIgnoreWhitespace = false;
			options.bIgnoreBlankLines = false;
			options.bFilterCommentsLines = false;
			options.bIgnoreCase = false;
			options.bIgnoreEol = false;

			pCtx->InitDiffItemList();
			pCtx->CreateCompareOptions(dm, options);
			pCtx->m_iGuessEncodingType = 0;
			pCtx->m_bIgnoreSmallTimeDiff = true;
			pCtx->m_bStopAfterFirstDiff = false;
			pCtx->m_nQuickCompareLimit = 4 * 1024 * 1024;
			pCtx->m_bPluginsEnabled = false;
			pCtx->m_bWalkUniques = true;
			pCtx->m_pCompareStats = &cmpstats;
			pCtx->m_bRecursive = true;
			pCtx->m_piFilterGlobal = &filter;
			filter.SetDiffContext(pCtx.get());

			CDiffThread diffThread;
			diffThread.SetContext(pCtx.get());
			diffThread.SetCollectFunction([](DiffFuncStruct* myStruct) {
				bool casesensitive = false;
				int depth = myStruct->context->m_bRecursive ? -1 : 0;
				PathContext paths = myStruct->context->GetNormalizedPaths();
				String subdir[3] = { _T(""), _T(""), _T("") };
				DirScan_GetItems(paths, subdir, myStruct,
					casesensitive, depth, nullptr, myStruct->context->m_bWalkUniques);
				});
			diffThread.SetCompareFunction([](DiffFuncStruct* myStruct) {
				DirScan_CompareItems(myStruct, nullptr);
				});
			diffThread.CompareDirectories();

			while (diffThread.GetThreadState() != CDiffThread::THREAD_COMPLETED)
			{
				Poco::Thread::sleep(200);
				std::wcout << L"Comparing " << cmpstats.GetComparedItems() << L" items...\r";
			}
			std::wcout << L"\nComparison completed.\n";
		}
		else if (cmd[0] == L'd') // Display results
		{
			if (!pCtx)
			{
				std::wcout << L"No comparison results available. Run 'c' command first.\n";
				continue;
			}

			DIFFITEM* pos = pCtx->GetFirstDiffPosition();
			if (!pos)
			{
				std::wcout << L"No items found.\n";
				continue;
			}

			bool showDetails = false;
			if (cmd.length() > 2)
			{
				std::wstring option = cmd.substr(2);
				if (option == L"detail" || option == L"details")
					showDetails = true;
			}

			std::wcout << L"\nComparison Results";
			if (showDetails)
				std::wcout << L" (Detailed)";
			std::wcout << L":\n";
			std::wcout << L"==================\n";
			int nDirs = pCtx->GetCompareDirs();
			while (pos)
			{
				PrintDiffItemTree(pos, 0, nDirs, showDetails);
				pos = pos->GetFwdSiblingLink();
			}
		}
		else
		{
			std::wcout << L"Unknown command: " << cmd << std::endl;
		}
	}

	return 0;
}
