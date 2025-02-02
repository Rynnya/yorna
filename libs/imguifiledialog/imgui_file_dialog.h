/*
MIT License

Copyright (c) 2018-2022 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef IMGUIFILEDIALOG_H
#define IMGUIFILEDIALOG_H

// compatible with 1.88 WIP
#define IMGUIFILEDIALOG_VERSION "v0.6.5"

#ifndef CUSTOM_IMGUIFILEDIALOG_CONFIG
#include "imgui_file_dialog_config.h"
#else // CUSTOM_IMGUIFILEDIALOG_CONFIG
#include CUSTOM_IMGUIFILEDIALOG_CONFIG
#endif // CUSTOM_IMGUIFILEDIALOG_CONFIG

// file style enum for file display (color, icon, font)
typedef int IGFD_FileStyleFlags; // -> enum IGFD_FileStyleFlags_
enum IGFD_FileStyleFlags_ // by evaluation / priority order
{
	IGFD_FileStyle_None					= 0,		// define none style
	IGFD_FileStyleByTypeFile			= (1 << 0),	// define style for all files
	IGFD_FileStyleByTypeDir				= (1 << 1),	// define style for all dir
	IGFD_FileStyleByTypeLink			= (1 << 2),	// define style for all link
	IGFD_FileStyleByExtention			= (1 << 3),	// define style by extention, for files or links
	IGFD_FileStyleByFullName			= (1 << 4),	// define style for particular file/dir/link full name (filename + extention)
	IGFD_FileStyleByContainedInFullName = (1 << 5),	// define style for file/dir/link when criteria is contained in full name
};

typedef int ImGuiFileDialogFlags; // -> enum ImGuiFileDialogFlags_
enum ImGuiFileDialogFlags_
{
	ImGuiFileDialogFlags_None							= 0,		// define none default flag
	ImGuiFileDialogFlags_ConfirmOverwrite				= (1 << 0),	// show confirm to overwrite dialog
	ImGuiFileDialogFlags_DontShowHiddenFiles			= (1 << 1),	// dont show hidden file (file starting with a .)
	ImGuiFileDialogFlags_DisableCreateDirectoryButton	= (1 << 2),	// disable the create directory button
	ImGuiFileDialogFlags_HideColumnType					= (1 << 3),	// hide column file type
	ImGuiFileDialogFlags_HideColumnSize					= (1 << 4),	// hide column file size
	ImGuiFileDialogFlags_HideColumnDate					= (1 << 5),	// hide column file date
	ImGuiFileDialogFlags_NoDialog						= (1 << 6),	// let the dialog embedded in your own imgui begin / end scope
	ImGuiFileDialogFlags_ReadOnlyFileNameField			= (1 << 7),	// don't let user type in filename field for file open style dialogs
	ImGuiFileDialogFlags_CaseInsensitiveExtention		= (1 << 8), // the file extentions treatments will not take into account the case 
	ImGuiFileDialogFlags_Modal							= (1 << 9), // modal
#ifdef USE_BOOKMARK
	ImGuiFileDialogFlags_DisableBookmarkMode			= (1 << 11),	// disable the bookmark mode
#endif // USE_BOOKMARK
	ImGuiFileDialogFlags_Default = ImGuiFileDialogFlags_ConfirmOverwrite
};

#ifdef __cplusplus

#include <imgui.h>

#include <cfloat>
#include <utility>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <regex>

namespace IGFD
{
#ifndef defaultSortField
#define defaultSortField FIELD_FILENAME
#endif // defaultSortField

#ifndef defaultSortOrderFilename
#define defaultSortOrderFilename true
#endif // defaultSortOrderFilename
#ifndef defaultSortOrderType
#define defaultSortOrderType true
#endif // defaultSortOrderType
#ifndef defaultSortOrderSize
#define defaultSortOrderSize true
#endif // defaultSortOrderSize
#ifndef defaultSortOrderDate
#define defaultSortOrderDate true
#endif // defaultSortOrderDate
#ifndef defaultSortOrderThumbnails
#define defaultSortOrderThumbnails true
#endif // defaultSortOrderThumbnails

#ifndef MAX_FILE_DIALOG_NAME_BUFFER 
#define MAX_FILE_DIALOG_NAME_BUFFER 1024
#endif // MAX_FILE_DIALOG_NAME_BUFFER

#ifndef MAX_PATH_BUFFER_SIZE
#define MAX_PATH_BUFFER_SIZE 1024
#endif // MAX_PATH_BUFFER_SIZE

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class FileDialogInternal;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class SearchManager
	{
	public:
		std::string puSearchTag;
		char puSearchBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";
		bool puSearchInputIsActive = false;

	public:
		void Clear();																							// clear datas
		void DrawSearchBar(FileDialogInternal& vFileDialogInternal);											// draw the search bar
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Utils
	{
	public:
		struct PathStruct
		{
			std::string path;
			std::string name;
			std::string ext;
			bool isOk = false;
		};

	public:
		static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
		static bool ReplaceString(std::string& str, const std::string& oldStr, const std::string& newStr);
		static bool IsDirectoryCanBeOpened(const std::string& name);			// by ex protected dirs (not user rights)
		static bool IsDirectoryExist(const std::string& name);
		static bool CreateDirectoryIfNotExist(const std::string& name);
		static PathStruct ParsePathFileName(const std::string& vPathFileName);
		static void AppendToBuffer(char* vBuffer, size_t vBufferLen, const std::string& vStr);
		static void ResetBuffer(char* vBuffer);
		static void SetBuffer(char* vBuffer, size_t vBufferLen, const std::string& vStr);
		static bool WReplaceString(std::wstring& str, const std::wstring& oldStr, const std::wstring& newStr);
		static std::vector<std::wstring> WSplitStringToVector(const std::wstring& text, char delimiter, bool pushEmpty);
		static std::string utf8_encode(const std::wstring& wstr);
		static std::wstring utf8_decode(const std::string& str);
		static std::vector<std::string> SplitStringToVector(const std::string& text, char delimiter, bool pushEmpty);
		static std::vector<std::string> GetDrivesList();
		static std::string LowerCaseString(const std::string& vString); // turn all text in lower case for search facilitie
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class FileStyle
	{
	public:
		ImVec4 color = ImVec4(0, 0, 0, 0);
		std::string icon;
		ImFont* font = nullptr;
		IGFD_FileStyleFlags flags = 0;

	public:
		FileStyle();
		FileStyle(const FileStyle& vStyle);
		FileStyle(const ImVec4& vColor, const std::string& vIcon = "", ImFont* vFont = nullptr);
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class FileInfos;
	class FilterManager
	{
	public:
		class FilterInfos
		{
		public:
			std::string filter;																				// simple filter
			std::regex filter_regex;																		// filter fo type regex
			std::set<std::string> collectionfilters;														// collections of filters
			std::string filter_optimized;																	// opitmized for case insensitive search
			std::set<std::string> collectionfilters_optimized;												// optimized collections of filters for case insensitive search
			std::vector<std::regex> collectionfilters_regex;												// collection of regex filter type

		public:
			void clear();																					// clear the datas
			bool empty() const;																				// is filter empty
			bool exist(const std::string& vFilter, bool vIsCaseInsensitive) const;							// is filter exist
			bool regex_exist(const std::string& vFilter) const;												// is regex filter exist
		};

	private:
#ifdef NEED_TO_BE_PUBLIC_FOR_TESTS
	public:
#endif
		std::vector<FilterInfos> prParsedFilters;
		std::unordered_map<IGFD_FileStyleFlags, std::unordered_map<std::string, std::shared_ptr<FileStyle>>> prFilesStyle;		// file infos for file extention only
		FilterInfos prSelectedFilter;

	public:
		std::string puDLGFilters;
		std::string puDLGdefaultExt;

	public:
		void ParseFilters(const char* vFilters);															// Parse filter syntax, detect and parse filter collection
		void SetSelectedFilterWithExt(const std::string& vFilter);											// Select filter
		
		bool prFillFileStyle(std::shared_ptr<FileInfos> vFileInfos)  const;									// fill with the good style
		
		void SetFileStyle(
			const IGFD_FileStyleFlags& vFlags,
			const char* vCriteria,
			const FileStyle& vInfos);																		// Set FileStyle
		void SetFileStyle(
			const IGFD_FileStyleFlags& vFlags,
			const char* vCriteria,
			const ImVec4& vColor,
			const std::string& vIcon,
			ImFont* vFont);																					// link file style to Color and Icon and Font
		bool GetFileStyle(
			const IGFD_FileStyleFlags& vFlags,
			const std::string& vCriteria,
			ImVec4* vOutColor,
			std::string* vOutIcon,
			ImFont** vOutFont);																				// Get Color and Icon for Filter
		void ClearFilesStyle();																				// clear prFileStyle

		bool IsCoveredByFilters(														// check if current file extention (vExt) is covered by current filter, ro by regex (vNameExt)
			const std::string& vNameExt, 
			const std::string& vExt, 
			bool vIsCaseInsensitive) const;																	
		bool DrawFilterComboBox(FileDialogInternal& vFileDialogInternal);									// draw the filter combobox
		FilterInfos GetSelectedFilter();																	// get the current selected filter
		std::string ReplaceExtentionWithCurrentFilter(const std::string& vFile) const;						// replace the extention of the current file by the selected filter
		void SetDefaultFilterIfNotDefined();																// define the first filter if no filter is selected
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class FileType
	{
	public:
		enum class ContentType {
			// The ordering will be used during sort.
			Invalid = -1,
			Directory = 0,
			File = 1,
			LinkToUnknown = 2, // link to something that is not a regular file or directory.
		};

	private:
		ContentType m_Content = ContentType::Invalid;
		bool m_Symlink = false;

	public:
		FileType() = default;
		FileType(const ContentType& vContentType, const bool& vIsSymlink)
			: m_Content(vContentType), m_Symlink(vIsSymlink)
		{}

		void SetContent(const ContentType& vContentType) { m_Content = vContentType; }
		void SetSymLink(const bool& vIsSymlink) { m_Symlink = vIsSymlink; }

		bool isValid () const { return m_Content != ContentType::Invalid; }
		bool isDir () const { return m_Content == ContentType::Directory; }
		bool isFile () const { return m_Content == ContentType::File; }
		bool isLinkToUnknown () const { return m_Content == ContentType::LinkToUnknown; }
		bool isSymLink() const { return m_Symlink; }

		// Comparisons only care about the content type, ignoring whether it's a symlink or not.
		bool operator== (const FileType& rhs) const { return m_Content == rhs.m_Content; }
		bool operator!= (const FileType& rhs) const { return m_Content != rhs.m_Content; }
		bool operator<  (const FileType& rhs) const { return m_Content < rhs.m_Content; }
		bool operator>  (const FileType& rhs) const { return m_Content > rhs.m_Content; }
	};

	class FileInfos
	{
	public:
		FileType fileType;    								// fileType		
		std::string filePath;								// path of the file
		std::string fileNameExt;							// filename of the file (file name + extention) (but no path)
		std::string fileNameExt_optimized;					// optimized for search => insensitivecase
		std::string fileExt;								// extention of the file
		size_t fileSize = 0;								// for sorting operations
		std::string formatedFileSize;						// file size formated (10 o, 10 ko, 10 mo, 10 go)
		std::string fileModifDate;							// file user defined format of the date (data + time by default)
		std::shared_ptr<FileStyle> fileStyle = nullptr;		// style of the file

	public:
		bool IsTagFound(const std::string& vTag) const;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class FileManager
	{
	public: // types
		enum class SortingFieldEnum		// sorting for filetering of the file lsit
		{
			FIELD_NONE = 0,				// no sorting preference, result indetermined haha..
			FIELD_FILENAME,				// sorted by filename
			FIELD_TYPE,					// sorted by filetype
			FIELD_SIZE,					// sorted by filesize (formated file size)
			FIELD_DATE,					// sorted by filedate
		};

	private:
		std::string prCurrentPath;											// current path (to be decomposed in prCurrentPathDecomposition
		std::vector<std::string> prCurrentPathDecomposition;				// part words
		std::vector<std::shared_ptr<FileInfos>> prFileList;					// base container
		std::vector<std::shared_ptr<FileInfos>> prFilteredFileList;			// filtered container (search, sorting, etc..)
		std::vector<std::shared_ptr<FileInfos>> prPathList;					// base container for path selection
		std::vector<std::shared_ptr<FileInfos>> prFilteredPathList;			// filtered container for path selection (search, sorting, etc..)
		std::vector<std::string>::iterator prPopupComposedPath;				// iterator on prCurrentPathDecomposition for Current Path popup
		std::string prLastSelectedFileName;									// for shift multi selection
		std::set<std::string> prSelectedFileNames;							// the user selection of FilePathNames
		bool prCreateDirectoryMode = false;									// for create directory widget

	public:
		char puVariadicBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";			// called by prSelectableItem
		bool puInputPathActivated = false;									// show input for path edition
		bool puDrivesClicked = false;										// event when a drive button is clicked
		bool puPathClicked = false;											// event when a path button was clicked
		char puInputPathBuffer[MAX_PATH_BUFFER_SIZE] = "";					// input path buffer for imgui widget input text (displayed in palce of composer)
		char puFileNameBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";			// file name buffer in footer for imgui widget input text
		char puDirectoryNameBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";		// directory name buffer in footer for imgui widget input text (when is directory mode)
		std::string puHeaderFileName;										// detail view name of column file
		std::string puHeaderFileType;										// detail view name of column type
		std::string puHeaderFileSize;										// detail view name of column size
		std::string puHeaderFileDate;										// detail view name of column date + time
		bool puSortingDirection[4] = {										// true => Ascending, false => Descending
			defaultSortOrderFilename,
			defaultSortOrderType,
			defaultSortOrderSize,
			defaultSortOrderDate };
		SortingFieldEnum puSortingField = SortingFieldEnum::FIELD_FILENAME;	// detail view sorting column
		bool puShowDrives = false;											// drives are shown (only on os windows)

		std::string puDLGpath;												// base path set by user when OpenDialog was called
		std::string puDLGDefaultFileName;									// base default file path name set by user when OpenDialog was called
		size_t puDLGcountSelectionMax = 1U; // 0 for infinite				// base max selection count set by user when OpenDialog was called
		bool puDLGDirectoryMode = false;									// is directory mode (defiend like : puDLGDirectoryMode = (filters.empty()))

		std::string puFsRoot;

	private:
		static std::string prRoundNumber(double vvalue, int n);											// custom rounding number
		static std::string prFormatFileSize(size_t vByteSize);											// format file size field
		static void prCompleteFileInfos(const std::shared_ptr<FileInfos>& FileInfos);					// set time and date infos of a file (detail view mode)
		void prRemoveFileNameInSelection(const std::string& vFileName);									// selection : remove a file name
		void prAddFileNameInSelection(const std::string& vFileName, bool vSetLastSelectionFileName);	// selection : add a file name
		void AddFile(const FileDialogInternal& vFileDialogInternal, 
			const std::string& vPath, const std::string& vFileName, const FileType& vFileType);		    // add file called by scandir
		void AddPath(const FileDialogInternal& vFileDialogInternal,
			const std::string& vPath, const std::string& vFileName, const FileType& vFileType);			// add file called by scandir

#if defined(USE_QUICK_PATH_SELECT)
		void ScanDirForPathSelection(const FileDialogInternal& vFileDialogInternal, const std::string& vPath);	// scan the directory for retrieve the path list
		void OpenPathPopup(const FileDialogInternal& vFileDialogInternal, std::vector<std::string>::iterator vPathIter);
#endif // USE_QUICK_PATH_SELECT

		void SetCurrentPath(std::vector<std::string>::iterator vPathIter);

		void ApplyFilteringOnFileList(
			const FileDialogInternal& vFileDialogInternal,
			std::vector<std::shared_ptr<FileInfos>>& vFileInfosList,
			std::vector<std::shared_ptr<FileInfos>>& vFileInfosFilteredList);
		void SortFields(
			const FileDialogInternal& vFileDialogInternal,
			std::vector<std::shared_ptr<FileInfos>>& vFileInfosList,
			std::vector<std::shared_ptr<FileInfos>>& vFileInfosFilteredList);									// will sort a column
		
	public:
		FileManager();
		bool IsComposerEmpty();
		size_t GetComposerSize();
		bool IsFileListEmpty();
		bool IsPathListEmpty();
		bool IsFilteredListEmpty();
		bool IsPathFilteredListEmpty();
		size_t GetFullFileListSize();
		std::shared_ptr<FileInfos> GetFullFileAt(size_t vIdx);
		size_t GetFilteredListSize();
		size_t GetPathFilteredListSize();
		std::shared_ptr<FileInfos> GetFilteredFileAt(size_t vIdx);
		std::shared_ptr<FileInfos> GetFilteredPathAt(size_t vIdx);
		std::vector<std::string>::iterator GetCurrentPopupComposedPath();
		bool IsFileNameSelected(const std::string& vFileName);
		std::string GetBack();
		void ClearComposer();
		void ClearFileLists();																			// clear file list, will destroy thumbnail textures
		void ClearPathLists();																			// clear path list, will destroy thumbnail textures
		void ClearAll();
		void ApplyFilteringOnFileList(const FileDialogInternal& vFileDialogInternal);
		void SortFields(const FileDialogInternal& vFileDialogInternal);									// will sort a column
		void OpenCurrentPath(const FileDialogInternal& vFileDialogInternal);							// set the path of the dialog, will launch the directory scan for populate the file listview
		bool GetDrives();																				// list drives on windows platform
		bool CreateDir(const std::string& vPath);														// create a directory on the file system
		std::string ComposeNewPath(std::vector<std::string>::iterator vIter);									// compose a path from the compose path widget
		bool SetPathOnParentDirectoryIfAny();															// compose paht on parent directory
		std::string GetCurrentPath();																	// get the current path
		void SetCurrentPath(const std::string& vCurrentPath);											// set the current path
		static bool IsFileExist(const std::string& vFile);
		void SetDefaultFileName(const std::string& vFileName);
		bool SelectDirectory(const std::shared_ptr<FileInfos>& vInfos);										// enter directory
		void SelectFileName(const FileDialogInternal& vFileDialogInternal, 
			const std::shared_ptr<FileInfos>& vInfos);															// select filename
		
		//depend of dirent.h
		void SetCurrentDir(const std::string& vPath);													// define current directory for scan
		void ScanDir(const FileDialogInternal& vFileDialogInternal, const std::string& vPath);			// scan the directory for retrieve the file list
		
	public:
		std::string GetResultingPath();
		std::string GetResultingFileName(FileDialogInternal& vFileDialogInternal);
		std::string GetResultingFilePathName(FileDialogInternal& vFileDialogInternal);
		std::map<std::string, std::string> GetResultingSelection();

	public:
		void DrawDirectoryCreation(const FileDialogInternal& vFileDialogInternal);						// draw directory creation widget
		void DrawPathComposer(const FileDialogInternal& vFileDialogInternal);							// draw path composer widget
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class BookMarkFeature
	{
	protected:
		BookMarkFeature();

#ifdef USE_BOOKMARK
	private:
		struct BookmarkStruct
		{
			std::string name;			// name of the bookmark
			
			// todo: the path could be relative, better if the app is movedn but bookmarked path can be outside of the app
			std::string path;			// absolute path of the bookmarked directory 

			bool defined_by_code = false;	// defined by code, can be used for rpevent serialization / deserialization
		};

	private:
		ImGuiListClipper prBookmarkClipper;
		std::vector<BookmarkStruct> prBookmarks;
		char prBookmarkEditBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";

	protected:
		float prBookmarkWidth = 200.0f;
		bool prBookmarkPaneShown = false;
		
	protected:
		void prDrawBookmarkButton();															// draw bookmark button
		bool prDrawBookmarkPane(FileDialogInternal& vFileDialogInternal, const ImVec2& vSize);	// draw bookmark Pane

	public:
		std::string SerializeBookmarks(								// serialize bookmarks : return bookmark buffer to save in a file
			const bool& vDontSerializeCodeBasedBookmarks = true);	// for avoid serialization of bookmarks added by code
		void DeserializeBookmarks(									// deserialize bookmarks : load bookmark buffer to load in the dialog (saved from previous use with SerializeBookmarks())
			const std::string& vBookmarks);							// bookmark buffer to load
		void AddBookmark(											// add a bookmark by code
			const std::string& vBookMarkName,						// bookmark name
			const std::string& vBookMarkPath);						// bookmark path
		bool RemoveBookmark(										// remove a bookmark by code, return true if succeed
			const std::string& vBookMarkName);						// bookmark name to remove

#endif // USE_BOOKMARK
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// file localization by input chat // widget flashing
	class KeyExplorerFeature
	{
	protected:
		KeyExplorerFeature();

#ifdef USE_EXPLORATION_BY_KEYS
	private:
		bool prLocateFileByInputChar_lastFound = false;
		ImWchar prLocateFileByInputChar_lastChar = 0;
		float prFlashAlpha = 0.0f;																// flash when select by char
		float prFlashAlphaAttenInSecs = 1.0f;													// fps display dependant
		int prLocateFileByInputChar_InputQueueCharactersSize = 0;
		size_t prFlashedItem = 0;																// flash when select by char
		size_t prLocateFileByInputChar_lastFileIdx = 0;

	protected:
		void prLocateByInputKey(FileDialogInternal& vFileDialogInternal);						// select a file line in listview according to char key
		bool prLocateItem_Loop(FileDialogInternal& vFileDialogInternal, ImWchar vC);			// restrat for start of list view if not found a corresponding file
		void prExploreWithkeys(FileDialogInternal& vFileDialogInternal, ImGuiID vListViewID);	// select file/directory line in listview accroding to up/down enter/backspace keys
		static bool prFlashableSelectable(																// custom flashing selectable widgets, for flash the selected line in a short time
			const char* label, bool selected = false, ImGuiSelectableFlags flags = 0,
			bool vFlashing = false, const ImVec2& size = ImVec2(0, 0));
		void prStartFlashItem(size_t vIdx);														// define than an item must be flashed
		bool prBeginFlashItem(size_t vIdx);														// start the flashing of a line in lsit view
		static void prEndFlashItem();																	// end the fleshing accrdoin to var prFlashAlphaAttenInSecs

	public:
		void SetFlashingAttenuationInSeconds(													// set the flashing time of the line in file list when use exploration keys
			float vAttenValue);																	// set the attenuation (from flashed to not flashed) in seconds
#endif // USE_EXPLORATION_BY_KEYS
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	typedef void* UserDatas;
	typedef std::function<void(const char*, UserDatas, bool*)> PaneFun;							// side pane function binding
	class FileDialogInternal
	{
	public:
		FileManager puFileManager;
		FilterManager puFilterManager;
		SearchManager puSearchManager;

	public:
		std::string puName;
		bool puShowDialog = false;
		ImVec2 puDialogCenterPos = ImVec2(0, 0);												// center pos for display the confirm overwrite dialog
		int puLastImGuiFrameCount = 0;															// to be sure than only one dialog displayed per frame
		float puFooterHeight = 0.0f;
		bool puCanWeContinue = true;															// events
		bool puOkResultToConfirm = false;														// to confim if ok for OverWrite
		bool puIsOk = false;
		bool puFileInputIsActive = false;														// when input text for file or directory is active
		bool puFileListViewIsActive = false;													// when list view is active
		std::string puDLGkey;
		std::string puDLGtitle;
		ImGuiFileDialogFlags puDLGflags = ImGuiFileDialogFlags_None;
		UserDatas puDLGuserDatas = nullptr;
		PaneFun puDLGoptionsPane = nullptr;
		float puDLGoptionsPaneWidth = 0.0f;
		bool puNeedToExitDialog = false;

		bool puUseCustomLocale = false;
		int puLocaleCategory = LC_ALL;	// locale category to use
		std::string puLocaleBegin; // the locale who will be applied at start of the display dialog
		std::string puLocaleEnd; // the locale who will be applaied at end of the display dialog

	public:
		void NewFrame();			// new frame, so maybe neded to do somethings, like reset events
		void EndFrame();			// end frame, so maybe neded to do somethings fater all
		void ResetForNewDialog();	// reset what is needed to reset for the openging of a new dialog
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class FileDialog : 
		public BookMarkFeature, 
		public KeyExplorerFeature
	{
	private:
		FileDialogInternal prFileDialogInternal;
		ImGuiListClipper prFileListClipper;
		ImGuiListClipper prPathListClipper;
		float prOkCancelButtonWidth = 0.0f;

	public:
		bool puAnyWindowsHovered = false;							// not remember why haha :) todo : to check if we can remove
		 
	public:
		// Singleton for easier accces form anywhere but only one dialog at a time
		// vCopy or vForce can be used for share a memory pointer in a new memory space like a dll module
		static FileDialog* Instance(FileDialog* vCopy = nullptr, bool vForce = false)	
		{
			static FileDialog _instance;
			static FileDialog* _instance_copy = nullptr;
			if (vCopy || vForce)
			{
				_instance_copy = vCopy;
			}
			if (_instance_copy)
			{
				return _instance_copy;
			}
			return &_instance;
		}

	public:
		FileDialog();												// ImGuiFileDialog Constructor. can be used for have many dialog at same tiem (not possible with singleton)
		virtual ~FileDialog();										// ImGuiFileDialog Destructor

		// standard dialog
		void OpenDialog(											// open simple dialog (path and fileName can be specified)
			const std::string& vKey,								// key dialog
			const std::string& vTitle,								// title
			const char* vFilters,									// filters
			const std::string& vPath,								// path
			const std::string& vFileName,							// defaut file name
			const int& vCountSelectionMax = 1,						// count selection max
			UserDatas vUserDatas = nullptr,							// user datas (can be retrieved in pane)
			ImGuiFileDialogFlags vFlags = 0);						// ImGuiFileDialogFlags 

		void OpenDialog(											// open simple dialog (path and filename are obtained from filePathName)
			const std::string& vKey,								// key dialog
			const std::string& vTitle,								// title
			const char* vFilters,									// filters
			const std::string& vFilePathName,						// file path name (will be decompsoed in path and fileName)
			const int& vCountSelectionMax = 1,						// count selection max
			UserDatas vUserDatas = nullptr,							// user datas (can be retrieved in pane)
			ImGuiFileDialogFlags vFlags = 0);						// ImGuiFileDialogFlags 

		// with pane
		void OpenDialog(											// open dialog with custom right pane (path and fileName can be specified)
			const std::string& vKey,								// key dialog
			const std::string& vTitle,								// title
			const char* vFilters,									// filters
			const std::string& vPath,								// path
			const std::string& vFileName,							// defaut file name
			const PaneFun& vSidePane,								// side pane
			const float& vSidePaneWidth = 250.0f,					// side pane width
			const int& vCountSelectionMax = 1,						// count selection max
			UserDatas vUserDatas = nullptr,							// user datas (can be retrieved in pane)
			ImGuiFileDialogFlags vFlags = 0);						// ImGuiFileDialogFlags 

		void OpenDialog(											// open dialog with custom right pane (path and filename are obtained from filePathName)
			const std::string& vKey,								// key dialog
			const std::string& vTitle,								// title
			const char* vFilters,									// filters
			const std::string& vFilePathName,						// file path name (will be decompsoed in path and fileName)
			const PaneFun& vSidePane,								// side pane
			const float& vSidePaneWidth = 250.0f,					// side pane width
			const int& vCountSelectionMax = 1,						// count selection max
			UserDatas vUserDatas = nullptr,							// user datas (can be retrieved in pane)
			ImGuiFileDialogFlags vFlags = 0);						// ImGuiFileDialogFlags 

		// Display / Close dialog form
		bool Display(												// Display the dialog. return true if a result was obtained (Ok or not)
			const std::string& vKey,								// key dialog to display (if not the same key as defined by OpenDialog => no opening)
			ImGuiWindowFlags vFlags = ImGuiWindowFlags_NoCollapse,	// ImGuiWindowFlags
			ImVec2 vMinSize = ImVec2(0, 0),							// mininmal size contraint for the ImGuiWindow
			ImVec2 vMaxSize = ImVec2(FLT_MAX, FLT_MAX));			// maximal size contraint for the ImGuiWindow
		void Close();												// close dialog

		// queries
		bool WasOpenedThisFrame(const std::string& vKey) const;		// say if the dialog key was already opened this frame
		bool WasOpenedThisFrame() const;							// say if the dialog was already opened this frame
		bool IsOpened(const std::string& vKey) const;				// say if the key is opened
		bool IsOpened() const;										// say if the dialog is opened somewhere
		std::string GetOpenedKey() const;							// return the dialog key who is opened, return nothing if not opened

		// get result
		bool IsOk() const;											// true => Dialog Closed with Ok result / false : Dialog closed with cancel result
		std::map<std::string, std::string> GetSelection();			// Open File behavior : will return selection via a map<FileName, FilePathName>
		std::string GetFilePathName();								// Save File behavior : will always return the content of the field with current filter extention and current path
		std::string GetCurrentFileName();							// Save File behavior : will always return the content of the field with current filter extention
		std::string GetCurrentPath();								// will return current path
		std::string GetCurrentFilter();								// will return selected filter
		UserDatas GetUserDatas() const;								// will return user datas send with Open Dialog

		// file style by extentions
		void SetFileStyle(											// SetExtention datas for have custom display of particular file type
			const IGFD_FileStyleFlags& vFlags,						// file style
			const char* vCriteria,									// extention filter to tune
			const FileStyle& vInfos);								// Filter Extention Struct who contain Color and Icon/Text for the display of the file with extention filter
		void SetFileStyle(											// SetExtention datas for have custom display of particular file type
			const IGFD_FileStyleFlags& vFlags,							// file style
			const char* vCriteria,									// extention filter to tune
			const ImVec4& vColor,									// wanted color for the display of the file with extention filter
			const std::string& vIcon = "",							// wanted text or icon of the file with extention filter
			ImFont *vFont = nullptr);                               // wantes font
		bool GetFileStyle(											// GetExtention datas. return true is extention exist
			const IGFD_FileStyleFlags& vFlags,							// file style
			const std::string& vCriteria,									// extention filter (same as used in SetExtentionInfos)
			ImVec4* vOutColor,										// color to retrieve
			std::string* vOutIcon = nullptr,						// icon or text to retrieve
            ImFont** vOutFont = nullptr);                           // font to retreive
		void ClearFilesStyle();										// clear extentions setttings

		void SetLocales(											// set locales to use before and after the dialog display
			const int& vLocaleCategory,								// set local category
			const std::string& vLocaleBegin,						// locale to use at begining of the dialog display
			const std::string& vLocaleEnd);							// locale to use at the end of the dialog display

	protected:
		void NewFrame();											// new frame just at begining of display
		void EndFrame();											// end frame just at end of display
		void QuitFrame();											// quit frame when qui quit the dialog

		// others
		bool prConfirm_Or_OpenOverWriteFileDialog_IfNeeded(
			bool vLastAction, ImGuiWindowFlags vFlags);				// treatment of the result, start the confirm to overwrite dialog if needed (if defined with flag)
	
	public:
		// dialog parts
		virtual void prDrawHeader();								// draw header part of the dialog (bookmark btn, dir creation, path composer, search bar)
		virtual void prDrawContent();								// draw content part of the dialog (bookmark pane, file list, side pane)
		virtual bool prDrawFooter();								// draw footer part of the dialog (file field, fitler combobox, ok/cancel btn's)

		// widgets components
#if defined(USE_QUICK_PATH_SELECT)
		virtual void DisplayPathPopup(ImVec2 vSize);				// draw path popup when click on a \ or /
#endif // USE_QUICK_PATH_SELECT
		virtual bool prDrawValidationButtons();						// draw validations btns, ok, cancel buttons
		virtual bool prDrawOkButton();								// draw ok button
		virtual bool prDrawCancelButton();							// draw cancel button
		virtual void prDrawSidePane(float vHeight);					// draw side pane
		virtual void prSelectableItem(int vidx, 
			std::shared_ptr<FileInfos> vInfos, 
			bool vSelected, const char* vFmt, ...);					// draw a custom selectable behavior item
		virtual void prDrawFileListView(ImVec2 vSize);				// draw file list view (default mode)

		// to be called only by these function and theirs overrides
		// - prDrawFileListView
		// - prDrawThumbnailsListView
		// - prDrawThumbnailsGridView
		void prBeginFileColorIconStyle(
			std::shared_ptr<FileInfos> vFileInfos, 
			bool& vOutShowColor, 
			std::string& vOutStr, 
			ImFont** vOutFont);										// begin style apply of filter with color an icon if any
		void prEndFileColorIconStyle(
			const bool& vShowColor,
			ImFont* vFont);											// end style apply of filter
	};
}

typedef IGFD::UserDatas IGFDUserDatas;
typedef IGFD::PaneFun IGFDPaneFun;
typedef IGFD::FileDialog ImGuiFileDialog;
#else // __cplusplus
typedef struct ImGuiFileDialog ImGuiFileDialog;
typedef struct IGFD_Selection_Pair IGFD_Selection_Pair;
typedef struct IGFD_Selection IGFD_Selection;
#endif // __cplusplus

// C Interface

#include <stdint.h>

#if defined _WIN32 || defined __CYGWIN__
#ifdef IMGUIFILEDIALOG_NO_EXPORT
#define API
#else // IMGUIFILEDIALOG_NO_EXPORT
#define API __declspec(dllexport)
#endif // IMGUIFILEDIALOG_NO_EXPORT
#else // defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define API  __attribute__((__visibility__("default")))
#else // __GNUC__
#define API
#endif // __GNUC__
#endif // defined _WIN32 || defined __CYGWIN__

#ifdef __cplusplus
#define IMGUIFILEDIALOG_API extern "C" API 
#else // __cplusplus
#define IMGUIFILEDIALOG_API
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// C API ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct IGFD_Selection_Pair
{
	char* fileName;
	char* filePathName;
};

IMGUIFILEDIALOG_API IGFD_Selection_Pair IGFD_Selection_Pair_Get();										// return an initialized IGFD_Selection_Pair			
IMGUIFILEDIALOG_API void IGFD_Selection_Pair_DestroyContent(IGFD_Selection_Pair* vSelection_Pair);		// destroy the content of a IGFD_Selection_Pair

struct IGFD_Selection
{
	IGFD_Selection_Pair* table;	// 0
	size_t count;						// 0U
};

IMGUIFILEDIALOG_API IGFD_Selection IGFD_Selection_Get();											// return an initialized IGFD_Selection
IMGUIFILEDIALOG_API void IGFD_Selection_DestroyContent(IGFD_Selection* vSelection);					// destroy the content of a IGFD_Selection

// constructor / destructor
IMGUIFILEDIALOG_API ImGuiFileDialog* IGFD_Create(void);												// create the filedialog context
IMGUIFILEDIALOG_API void IGFD_Destroy(ImGuiFileDialog* vContext);									// destroy the filedialog context

typedef void (*IGFD_PaneFun)(const char*, void*, bool*);											// callback fucntion for display the pane

IMGUIFILEDIALOG_API void IGFD_OpenDialog(					// open a standard dialog
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context
	const char* vKey,										// key dialog
	const char* vTitle,										// title
	const char* vFilters,									// filters/filter collections. set it to null for directory mode 
	const char* vPath,										// path
	const char* vFileName,									// defaut file name
	const int vCountSelectionMax,							// count selection max
	void* vUserDatas,										// user datas (can be retrieved in pane)
	ImGuiFileDialogFlags vFlags);							// ImGuiFileDialogFlags 

IMGUIFILEDIALOG_API void IGFD_OpenDialog2(					// open a standard dialog
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context
	const char* vKey,										// key dialog
	const char* vTitle,										// title
	const char* vFilters,									// filters/filter collections. set it to null for directory mode 
	const char* vFilePathName,								// defaut file path name (path and filename witl be extracted from it)
	const int vCountSelectionMax,							// count selection max
	void* vUserDatas,										// user datas (can be retrieved in pane)
	ImGuiFileDialogFlags vFlags);							// ImGuiFileDialogFlags 

IMGUIFILEDIALOG_API void IGFD_OpenPaneDialog(				// open a standard dialog with pane
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context
	const char* vKey,										// key dialog
	const char* vTitle,										// title
	const char* vFilters,									// filters/filter collections. set it to null for directory mode 
	const char* vPath,										// path
	const char* vFileName,									// defaut file name
	const IGFD_PaneFun vSidePane,							// side pane
	const float vSidePaneWidth,								// side pane base width
	const int vCountSelectionMax,							// count selection max
	void* vUserDatas,										// user datas (can be retrieved in pane)
	ImGuiFileDialogFlags vFlags);							// ImGuiFileDialogFlags 

IMGUIFILEDIALOG_API void IGFD_OpenPaneDialog2(				// open a standard dialog with pane
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context
	const char* vKey,										// key dialog
	const char* vTitle,										// title
	const char* vFilters,									// filters/filter collections. set it to null for directory mode 
	const char* vFilePathName,								// defaut file name (path and filename witl be extracted from it)
	const IGFD_PaneFun vSidePane,							// side pane
	const float vSidePaneWidth,								// side pane base width
	const int vCountSelectionMax,							// count selection max
	void* vUserDatas,										// user datas (can be retrieved in pane)
	ImGuiFileDialogFlags vFlags);							// ImGuiFileDialogFlags

IMGUIFILEDIALOG_API bool IGFD_DisplayDialog(				// Display the dialog
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context
	const char* vKey,										// key dialog to display (if not the same key as defined by OpenDialog => no opening)
	ImGuiWindowFlags vFlags,								// ImGuiWindowFlags
	ImVec2 vMinSize,										// mininmal size contraint for the ImGuiWindow
	ImVec2 vMaxSize);										// maximal size contraint for the ImGuiWindow

IMGUIFILEDIALOG_API void IGFD_CloseDialog(					// Close the dialog
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context			

IMGUIFILEDIALOG_API bool IGFD_IsOk(							// true => Dialog Closed with Ok result / false : Dialog closed with cancel result
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context		

IMGUIFILEDIALOG_API bool IGFD_WasKeyOpenedThisFrame(		// say if the dialog key was already opened this frame
	ImGuiFileDialog* vContext, 								// ImGuiFileDialog context		
	const char* vKey);

IMGUIFILEDIALOG_API bool IGFD_WasOpenedThisFrame(			// say if the dialog was already opened this frame
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context	

IMGUIFILEDIALOG_API bool IGFD_IsKeyOpened(					// say if the dialog key is opened
	ImGuiFileDialog* vContext, 								// ImGuiFileDialog context		
	const char* vCurrentOpenedKey);							// the dialog key

IMGUIFILEDIALOG_API bool IGFD_IsOpened(						// say if the dialog is opened somewhere	
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context		

IMGUIFILEDIALOG_API IGFD_Selection IGFD_GetSelection(		// Open File behavior : will return selection via a map<FileName, FilePathName>
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context		

IMGUIFILEDIALOG_API char* IGFD_GetFilePathName(				// Save File behavior : will always return the content of the field with current filter extention and current path, WARNINGS you are responsible to free it
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context				

IMGUIFILEDIALOG_API char* IGFD_GetCurrentFileName(			// Save File behavior : will always return the content of the field with current filter extention, WARNINGS you are responsible to free it
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context				

IMGUIFILEDIALOG_API char* IGFD_GetCurrentPath(				// will return current path, WARNINGS you are responsible to free it
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context					

IMGUIFILEDIALOG_API char* IGFD_GetCurrentFilter(			// will return selected filter, WARNINGS you are responsible to free it
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context						

IMGUIFILEDIALOG_API void* IGFD_GetUserDatas(				// will return user datas send with Open Dialog
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context											

IMGUIFILEDIALOG_API void IGFD_SetFileStyle(					// SetExtention datas for have custom display of particular file type
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context 
	IGFD_FileStyleFlags vFileStyleFlags,					// file style type
	const char* vFilter,									// extention filter to tune
	ImVec4 vColor,											// wanted color for the display of the file with extention filter
	const char* vIconText,									// wanted text or icon of the file with extention filter (can be sued with font icon)
	ImFont* vFont);											// wanted font pointer

IMGUIFILEDIALOG_API void IGFD_SetFileStyle2(				// SetExtention datas for have custom display of particular file type
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context 
	IGFD_FileStyleFlags vFileStyleFlags,					// file style type
	const char* vFilter,									// extention filter to tune
	float vR, float vG, float vB, float vA,					// wanted color channels RGBA for the display of the file with extention filter
	const char* vIconText,									// wanted text or icon of the file with extention filter (can be sued with font icon)
	ImFont* vFont);											// wanted font pointer

IMGUIFILEDIALOG_API bool IGFD_GetFileStyle(
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context 
	IGFD_FileStyleFlags vFileStyleFlags,					// file style type
	const char* vFilter,									// extention filter (same as used in SetExtentionInfos)
	ImVec4* vOutColor,										// color to retrieve
	char** vOutIconText,									// icon or text to retrieve, WARNINGS you are responsible to free it
	ImFont** vOutFont);										// font pointer to retrived

IMGUIFILEDIALOG_API void IGFD_ClearFilesStyle(				// clear extentions setttings
	ImGuiFileDialog* vContext);								// ImGuiFileDialog context

IMGUIFILEDIALOG_API void SetLocales(						// set locales to use before and after display
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context 
	const int vCategory,									// set local category
	const char* vBeginLocale,								// locale to use at begining of the dialog display
	const char* vEndLocale);								// locale to set at end of the dialog display

#ifdef USE_EXPLORATION_BY_KEYS
IMGUIFILEDIALOG_API void IGFD_SetFlashingAttenuationInSeconds(	// set the flashing time of the line in file list when use exploration keys
	ImGuiFileDialog* vContext,									// ImGuiFileDialog context 
	float vAttenValue);											// set the attenuation (from flashed to not flashed) in seconds
#endif

#ifdef USE_BOOKMARK
IMGUIFILEDIALOG_API char* IGFD_SerializeBookmarks(			// serialize bookmarks : return bookmark buffer to save in a file, WARNINGS you are responsible to free it
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context
	bool vDontSerializeCodeBasedBookmarks);					// for avoid serialization of bookmarks added by code

IMGUIFILEDIALOG_API void IGFD_DeserializeBookmarks(			// deserialize bookmarks : load bookmar buffer to load in the dialog (saved from previous use with SerializeBookmarks())
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context 
	const char* vBookmarks);								// bookmark buffer to load 

IMGUIFILEDIALOG_API void IGFD_AddBookmark(					// add a bookmark by code
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context
	const char* vBookMarkName,								// bookmark name
	const char* vBookMarkPath);								// bookmark path

IMGUIFILEDIALOG_API void IGFD_RemoveBookmark(					// remove a bookmark by code, return true if succeed
	ImGuiFileDialog* vContext,								// ImGuiFileDialog context
	const char* vBookMarkName);								// bookmark name to remove
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // IMGUIFILEDIALOG_H
